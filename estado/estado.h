#ifndef _ESTADO_H_
#define _ESTADO_H_

#include "../listaEnlazadaSimple.h"

void EstadosPersonas(ListaEnlazadaRef contagiados, ListaEnlazadaRef sanos);
int ActualizarEstado(tipoNodoRef nodo,int *flag, ListaEnlazadaRef sanos, ListaEnlazadaRef contagiados);
void VacunarPersonas(ListaEnlazadaRef lista);
void PersonaSana(int id,ListaEnlazadaRef sanos, ListaEnlazadaRef contagiados);
#endif

