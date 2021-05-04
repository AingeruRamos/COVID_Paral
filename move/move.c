#include "move.h"

#include <stdlib.h>
#include <math.h>

#include "../globales.h"
#include "../persona.h"
#include "../listaEnlazadaSimple.h"

void CalcularVectorEspejo(int ejeX, int ejeY, struct Velocidad* vel) {
    if(ejeX) { vel->uy *= -1; }
    if(ejeY) { vel->ux *= -1; }
}

void ActualizarPosicion(tipoNodoRef nodo) {
    struct Persona* persona = (struct Persona*) &nodo->info;

    persona->pos.x += round(persona->vel.modulo*persona->vel.ux);
    persona->pos.y += round(persona->vel.modulo*persona->vel.uy);

    int aux, hayColision;

    do {
        hayColision = 0;
        if(persona->pos.x < 0) { //Colisión pared izquierda
            persona->pos.x = -persona->pos.x;
            CalcularVectorEspejo(0, 1, &persona->vel);
            hayColision = 1;
        } else if(persona->pos.x > MAX_X) { //Colisión paed derecha
            aux = persona->pos.x - MAX_X;
            persona->pos.x = MAX_X - aux;
            CalcularVectorEspejo(0, 1, &persona->vel);
            hayColision = 1;
        }

        if(persona->pos.y < 0) { //Colisión  techo
            persona->pos.y = -persona->pos.y;
            CalcularVectorEspejo(1, 0, &persona->vel);
            hayColision = 1;
        } else if(persona->pos.y > MAX_Y) { //Colisión suelo
            aux = persona->pos.y - MAX_Y;
            persona->pos.y = MAX_Y - aux;
            CalcularVectorEspejo(1, 0, &persona->vel);
            hayColision = 1;
        }
    } while(hayColision);
}

void ActualizarVelocidad(tipoNodoRef nodo) {
    struct Persona* persona = (struct Persona*) &nodo->info;

    float r = ((float) rand()/(float) RAND_MAX);
    if(r < P_CAMBIO_VEL) {
        r = rand() % MAX_MODULE;
        persona->vel.modulo = (int) r;

        r = ((float) rand()/(float) RAND_MAX) * 2; //Número aleatorio entre 0..2

        persona->vel.ux = cos(r*M_PI); //En radianes
        persona->vel.uy = sin(r*M_PI);
    }
}

void AplicarMovimiento(ListaEnlazadaRef lista) {
    tipoNodoRef nodo_aux = *lista;
    while(nodo_aux != NULL) {
        ActualizarPosicion(nodo_aux);
        ActualizarVelocidad(nodo_aux);
        nodo_aux = nodo_aux->sig;
    }
}
