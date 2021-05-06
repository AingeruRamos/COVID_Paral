#include "propagacion.h"

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

void AplicarPropagacion() {
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

    /*
	CREAR STRUCT MENSAJE (CHAR TIPO, INT NUMERO_PERSONAS, STRUCT POSICION* P)
	TIPO = 0 -> MENSAJE POSICION
	TIPO = 1 -> MENSAJE END
    */

    /*
	POR CADA NODO
	|	SE ELIGE NODO QUE ENVIA
	|	SI ERES NODO QUE ENVIA
	|	|	SELECCIONAS PAQUETE DE SANOS
	|	|	MIENTRAS QUEDEN SANOS
	|	|	|	BROADCAST MENSAJE POSICION DE UNA LISTA DE SANOS
	|	|	|	ESPERAS LA RESPUESTA POR GATHER O POR REDUCE_MAX
	|	|	|	SI RESPUESTA IGUAL 1 -> CONTAGIO
	|	|	|	SI RESPUESTA IGUAL 0 -> SANO
	|	|	|	SELECCIONAR SIGUIENTE PAQUETE DE SANOS
	|	|	-----------------------------------
	|	|	BROADCAST MENSAJE END
	|	-------------------------------------------
	|	SI ERES NODO QUE RECIVE
	|	|	CREAR FLAG = 0
	|	|	MIENTRAS FLAG IGUAL 0
	|	|	|	ESPERAR BROADCAST MENSAJE
	|	|	|	SI TIPO MENSAJE IGUAL POSICION
	|	|	|	|	RESERVAS ARRAY PARA RESULTADOS
	|	|	|	|	RECORRES LOS CONTAGIADOS EN BUSCA DE COINCIDENCIAS
	|	|	|	|	SI SE CONTAGIA -> PONER LA POSICION DEL ARRAY CORRESPONDIENTE A 1
	|	|	|	|	SI NO SE CONTAGIA -> PONER LAS POSICION DEL ARRAY CORRESPONDIENTE A 0
	|	|	|	|	¿ENVIAR EL ARRAY POR GATHER O CADA POSICION POR REDUCE_MAX?
	|	|	|	--------------------------
	|	|	|	SI TIPO MENSAJE IGUAL END -> FLAG = 1
	|	|	-----------------------------------
	|	-------------------------------------------
	---------------------------------------------------
	ACTUALIZAR PUNTERO DE LA LISTA DE CONTAGIADOS
    */

    contagiados = contagios_nuevos;
}
