#include "propagacion.h"

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../globales.h"
#include "../persona.h"
#include "../listaEnlazadaSimple.h"

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

int CrearPaquete(Posicion* pqt, tipoNodoRef pqt_ini) {
    int tam_pqt = 0;
    tipoNodoRef pqt_fin = pqt_ini;

    while(pqt_fin != NULL && tam_pqt < 4) {
	pqt[tam_pqt] = pqt_fin->info.pos;
	tam_pqt++;
	pqt_fin = pqt_fin->sig;
    }

    tam_pqt++;
    pqt_ini = pqt_fin;
    return tam_pqt;
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

    int tam_pqt;
    Posicion pos_array[4];

    int i_indice;
    int indice_array[4];

    Persona p_aux;

    for(i=0; i<world_size; i++) {
	if(world_rank == i) {
 	    nodo_sanos = *sanos;
	    while(nodo_sanos != NULL) {
		tam_pqt = CrearPaquete(pos_array, nodo_sanos);
	        //Enviar el tamaño del paquete por Bcast
	        if(tam_pqt > 0) {
		    //Enviar pos_array por Bcast
	            for(j=0; j<world_size; j++) {
		        if(j != i) {
			    //Recibir tamaño de la respuesta
			    //Cargar la informacon recibida en un array buffer

			    i_indice = 0;
			    for(k=0; k<tam_pqt; k++) { //Ver contagios en el paquete y gestionar
				persona_sana = &nodo_sanos->info;
				if(indice_array[i_indice] == k) {
				    nodo_aux = nodo_sanos->sig;

				    persona_copia = CopiarPersona(persona_sana);
				    persona_copia->estado = 1;

				    eliminarNodo(sanos, nodo_sanos);
				    insertarNodoComienzo(contagios_nuevos, persona_copia);

				    nodo_sanos = nodo_aux;
				    i_indice++;
				} else {
				    nodo_sanos = nodo_sanos->sig;
			    	}
			    }
			}
		    }
	        }
	    }
	} else {
	    //Recibir el tamaño del paquete por Bcast
	    while(tam_pqt > 0) {
	    	//Cargar la informacion recbida en un array buffer

		i_indice = 0;
		for(i=0; i<tam_pqt; i++) { //Calcular contagios en el paquete
		    p_aux.pos = pos_array[i];
		    nodo_contagiados = *contagiados;
		    while(nodo_contagiados != NULL) {
			persona_contagiada = &nodo_contagiados->info;
		        if(HayContagio(&p_aux, persona_contagiada)) {
			    indice_array[i_indice] = i;
			    i_indice++;
			    break;
			}
			nodo_contagiados = nodo_contagiados->sig;
                    }
		}
		//Enviar numero que se han contagiado
	    	//Enviar los indices de los contagiados
	    }
	}
    }

    contagiados = contagios_nuevos;
}
