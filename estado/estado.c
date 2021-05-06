#include "estado.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../globales.h"
#include "../persona.h"
#include "../listaEnlazadaSimple.h"


void EstadosPersonas(ListaEnlazadaRef contagiados, ListaEnlazadaRef sanos){
    tipoNodoRef nodo_aux = *contagiados;
    int flag, indice_borrar;
    flag=0;

    if (!estaVacia(*contagiados)){
       while(nodo_aux != NULL){

           indice_borrar=ActualizarEstado(nodo_aux,&flag,sanos,contagiados);
           if (flag == 1 && indice_borrar >= 0){
              eliminarNodoPosicionIndice(contagiados, indice_borrar);
           }
           flag = 0;

           nodo_aux = nodo_aux->sig;
       }
    }
}

int ActualizarEstado(tipoNodoRef nodo,int *flag, ListaEnlazadaRef sanos, ListaEnlazadaRef contagiados){  //devuelve indice si la persona se muere
    Persona *persona = (Persona*) &nodo->info;
    float muerte;


        if (persona->estado == 1){  //si la persona esta contagiada, cuando presenta sintomas
            if (persona->cont_incu == PERIODO_INCUBACION){
                persona->estado = 2;
                persona->cont_incu = 0;
             }
             else{
                persona->cont_incu++;
                return -1;
             }
	}

	if (persona->estado == 2){ //si la persona tiene covid, cuando se recupera
            if (persona->cont_recu == PERIODO_RECUPERACION){
                persona->estado = 3;
                N_RECUPERADOS++;
                persona->cont_recu = 0;
                PersonaSana(persona->id, sanos, contagiados);
            }
           else{
                muerte =((float) rand() /(float) RAND_MAX);
                if (muerte <  persona->p_muerte){
                    persona->estado = 5;
                    N_FALLECIDOS++;
                    *flag=1;
                    return persona->id;
                }
                else{
                    persona->cont_recu++;
                    return -2;
                }
            }
	}
}


void VacunarPersonas(ListaEnlazadaRef lista){
    tipoNodoRef nodo_aux = *lista;
    int vacunados,count;
    count = 0;

    vacunados = N_PERSONAS_VACU_T;
    while(nodo_aux != NULL && vacunados != 0){

        Persona *persona = (Persona*) &nodo_aux->info;

        if ((persona->estado == 0) || (persona->estado == 3)){
            persona->estado = 4; //si que quiere aplicar politica, switch
            count++;
            vacunados--;
        }

	nodo_aux = nodo_aux->sig;
    }
}


void PersonaSana(int indice, ListaEnlazadaRef sanos, ListaEnlazadaRef contagiados){
   tipoNodoRef nodo_sano = *sanos;
   tipoNodoRef nodo_contagiado = *contagiados;
   tipoNodoRef nodo_ant,nodo_aux;

   nodo_ant = NULL;

    while(nodo_contagiado != NULL){
        Persona *persona = (Persona*) &nodo_contagiado->info;
        if ( persona->id != indice){
            nodo_ant = nodo_contagiado;
            nodo_contagiado = nodo_contagiado->sig;
        }
        else{
            if (nodo_ant == NULL && nodo_contagiado->sig != NULL){  //si el nodo es el primero
                *contagiados = nodo_contagiado->sig;
                nodo_contagiado->sig = nodo_sano;
                *sanos = nodo_contagiado;
            }
            else if ( nodo_contagiado->sig == NULL && nodo_ant != NULL){  //si el nodo es el ultimo
                nodo_contagiado->sig = nodo_sano;
                nodo_ant->sig = NULL;
                *sanos = nodo_contagiado;
            }
            else if ( nodo_contagiado->sig == NULL && nodo_ant == NULL){ //si solo hay un nodo
                nodo_contagiado->sig = nodo_sano;
                *sanos = nodo_contagiado;
                *contagiados = NULL;
            }
            else{  //si el nodo esta por el medio
                nodo_aux = nodo_contagiado->sig;
                nodo_contagiado->sig = nodo_sano;
                nodo_ant->sig = nodo_aux;
                *sanos = nodo_contagiado;
            }
            break;
        }
    }
}

