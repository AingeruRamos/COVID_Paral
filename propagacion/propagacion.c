#include "propagacion.h"

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../globales.h"
#include "../persona.h"
#include "../listaEnlazadaSimple.h"

#define MAX_TAM_PQT 4

float DistanciaEntrePersonas(struct Persona* p1, struct Persona* p2) {
    int dx;
    if(p1->pos.x > p2->pos.x) { dx = p1->pos.x-p2->pos.x; }
    else { dx = p2->pos.x-p1->pos.x; }

    int dy;
    if(p1->pos.y > p2->pos.y) { dy = p1->pos.y-p2->pos.y; }
    else { dy = p2->pos.y-p1->pos.y; }

    return sqrt(dx*dx+dy*dy);
}

int HayContagio(struct Persona* p1, struct Persona* p2) {
    int d, r;
    d = DistanciaEntrePersonas(p1, p2);
    if(d < RADIO_CONTAGIO) {
        r = ((float) rand()/(float) RAND_MAX);
	if(r < P_CONTAGIO) {
	    return 1;
	}
    }
    return 0;
}

int CrearPaquete(int* pqt, tipoNodoRef pqt_ini) {
    int tam_pqt = 0;
    tipoNodoRef pqt_fin = pqt_ini;

    int i=0;
    while(pqt_fin != NULL && tam_pqt < 4) {
	pqt[i*tam_pqt] = pqt_fin->info.pos.x;
	pqt[(i*tam_pqt)+1] = pqt_fin->info.pos.y;
	tam_pqt++;
	pqt_fin = pqt_fin->sig;
    }
    return tam_pqt;
}

void InicializarArrayCeros(int* array, int length) {
    int i;
    for(i=0; i<length; i++) {
	array[i] = 0;
    }
}

void AplicarPropagacion(int world_size, int world_rank) {

    //PARTE INTERNA DEL NODO
    tipoNodoRef nodo_sanos = *sanos;
    tipoNodoRef nodo_contagiados = *contagiados;
    tipoNodoRef nodo_aux;

    ListaEnlazadaRef contagios_nuevos = contagiados;

    struct Persona* persona_sana;
    struct Persona* persona_contagiada;
    struct Persona* persona_copia;

    int hayContagio;

    while(nodo_sanos != NULL) {
	hayContagio = 0;
        persona_sana = &nodo_sanos->info;
        while(nodo_contagiados != NULL && !hayContagio) {
            persona_contagiada = &nodo_contagiados->info;
	    if(HayContagio(persona_sana, persona_contagiada)) {
		hayContagio = 1;
		nodo_aux = nodo_sanos->sig;

		persona_copia = CopiarPersona(persona_sana);
                persona_copia->estado = 1;

                eliminarNodo(sanos, nodo_sanos);
		insertarNodoComienzo(contagios_nuevos, persona_copia);
		N_SANOS--;
		N_CONTAGIADOS++;
		R0++;
            }
            nodo_contagiados = nodo_contagiados->sig;
        }
	nodo_contagiados = *contagiados;
	if(!hayContagio) {
            nodo_sanos = nodo_sanos->sig;
   	} else {
	    nodo_sanos = nodo_aux;
	}
    }


    //PARTE CON COMUNICACION
    nodo_sanos = *sanos;

    int i, j, k;

    int n_sanos;

    int tam_pqt;
    int pos_array[2*MAX_TAM_PQT];

    int i_indice;
    int indice_array[MAX_TAM_PQT];

    int tam_res;
    int conta_array[MAX_TAM_PQT];

    Persona p_aux;

    for(i=0; i<world_size; i++) {
	if(world_rank == i) { //Nodo director
 	    nodo_sanos = *sanos;
	    n_sanos = mostrarNumeroNodos(sanos);
	    while(n_sanos > 0) {
		tam_pqt = CrearPaquete(pos_array, nodo_sanos);
		n_sanos -= tam_pqt;
		MPI_Bcast(&tam_pqt, 1, MPI_INT, i, MPI_COMM_WORLD);//Enviar el tamaño del paquete por Bcast
	        if(tam_pqt > 0) {
		    //Enviar pos_array por Bcast
		    MPI_Bcast(pos_array, 2*tam_pqt, MPI_INT, i, MPI_COMM_WORLD);
		    InicializarArrayCeros(conta_array, MAX_TAM_PQT);
	            for(j=0; j<world_size; j++) {
		        if(j != i) {
			    //Recibir tamaño de la respuesta
			    MPI_Recv(&tam_res, 1, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			    //Cargar la informacon recibida en un array buffer
			    MPI_Recv(&indice_array, tam_res, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			    for(k=0; k<tam_res; k++) {
				conta_array[indice_array[k]] = 1;
			    }
			}
		    }
		    for(k=0; k<tam_pqt; k++) { //Ver contagios en el paquete y gestionar
			persona_sana = &nodo_sanos->info;
			if(conta_array[k] == 1) {
			    if(nodo_sanos->sig != NULL) { nodo_aux = nodo_sanos->sig; }

			    persona_copia = CopiarPersona(persona_sana);
			    persona_copia->estado = 1;

			    eliminarNodo(sanos, nodo_sanos);
			    insertarNodoComienzo(contagios_nuevos, persona_copia);

			    nodo_sanos = nodo_aux;

			    N_SANOS--;
			    N_CONTAGIADOS++;
			} else {
			    if(nodo_sanos->sig != NULL) { nodo_sanos = nodo_sanos->sig; }
		    	}
		    }
		}
	    }
	    tam_pqt = 0;
	    MPI_Bcast(&tam_pqt, 1, MPI_INT, i, MPI_COMM_WORLD);
	} else { //Nodos esclavos
	    //Recibir el tamaño del paquete por Bcast
	    MPI_Bcast(&tam_pqt, 1, MPI_INT, i, MPI_COMM_WORLD);
	    while(tam_pqt > 0) {
	    	//Cargar la informacion recbida en un array buffer
		MPI_Bcast(pos_array, 2*tam_pqt, MPI_INT, i, MPI_COMM_WORLD);
		i_indice = 0;
		for(j=0; j<2*tam_pqt; j+=2) { //Calcular contagios en el paquete
		    p_aux.pos.x = pos_array[j];
		    p_aux.pos.y = pos_array[j+1];
		    nodo_contagiados = *contagiados;
		    while(nodo_contagiados != NULL) {
			persona_contagiada = &nodo_contagiados->info;
		        if(HayContagio(&p_aux, persona_contagiada)) {
			    indice_array[i_indice] = j/2;
			    i_indice++;
			    R0++;
			    break;
			}
			nodo_contagiados = nodo_contagiados->sig;
                    }
		}
		MPI_Send(&i_indice, 1, MPI_INT, i, 0, MPI_COMM_WORLD); //Enviar numero de contag
	    	MPI_Send(indice_array, i_indice, MPI_INT, i, 0, MPI_COMM_WORLD); //Enviar los indices de los contagiados*/

		MPI_Bcast(&tam_pqt, 1, MPI_INT, i, MPI_COMM_WORLD);
	    }
	}
    }

    contagiados = contagios_nuevos;
}
