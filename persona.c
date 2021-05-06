#include "persona.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "globales.h"

Persona* NuevaPersona(int id, int estado) {

    float p_muerte;
    int edad;
    Persona* persona = (Persona*) malloc(sizeof(Persona));

    persona->id = id;

    persona->edad = 0;
    persona->estado = estado;
    persona->cont_incu = 0;
    persona->cont_recu = 0;
    persona->p_muerte = 0;

    persona->pos.x = rand() % MAX_X;
    persona->pos.y = rand() % MAX_Y;

    persona->vel.modulo = (int) (rand() % MAX_MODULE);
    float r = ((float) rand()/(float) RAND_MAX) * 2; //Número aleatorio entre 0..2
    persona->vel.ux = cos(r*M_PI); //En radianes
    persona->vel.uy = sin(r*M_PI);

    return persona;
}

Persona* CopiarPersona(Persona* p_origen) {
    Persona* persona = (Persona*) malloc(sizeof(Persona));
    persona->id = p_origen->id;
    persona->edad = p_origen->edad;
    persona->estado = p_origen->estado;
    persona->p_muerte = p_origen->p_muerte;

    persona->pos.x = p_origen->pos.x;
    persona->pos.y = p_origen->pos.y;

    persona->vel.modulo = p_origen->vel.modulo;
    persona->vel.ux = p_origen->vel.ux;
    persona->vel.uy = p_origen->vel.uy;

    return persona;
}

void PrintPersona(Persona* persona) {
    printf("ID: %d\n", persona->id);
    printf("EDAD: %d\n", persona->edad);
    printf("ESTADO: %d\n", persona->estado);
    printf("P_MUERTE: %f\n", persona->p_muerte);
    printf("POS: %d, %d\n", persona->pos.x, persona->pos.y);
    printf("VEL: M %d, UX %f, UY %f\n", persona->vel.modulo, persona->vel.ux, persona->vel.uy);
}


float calcular_p_morir(int edad){
  float prob;

  if( (edad >= 0) && (edad < 10) ){
     prob = 0;
  }
  else if( (edad >= 10) && (edad < 20) ){
     prob = 0.002;
  }
  else if( (edad >= 20) && (edad < 30) ){
     prob = 0.002;
  }
  else if( (edad >= 30) && (edad < 40) ){
     prob = 0.004;
  }
  else if( (edad >= 50) && (edad < 60) ){
     prob = 0.013;
  }
  else if( (edad >= 60) && (edad < 70) ){
     prob = 0.036;
  }
  else if( (edad >= 70) && (edad < 80) ){
     prob = 0.08;
  }
  else{
     prob = 0.148;
  }

  return prob;
}

