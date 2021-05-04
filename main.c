#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "globales.h"

#include "persona.h"
#include "listaEnlazadaSimple.h"
#include "estado/estado.h"
#include "propagacion/propagacion.h"
#include "move/move.h"
#include "metricas/metricas.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>

void InicializarParametros(char** argv);
void InicializarGlobales();
void calcular_edad(ListaEnlazadaRef lista,int n);

int main(int argc, char** argv) {

    if(argc != 9) {
        printf("Error en los parametros\n");
        return -1;
    }

    srand(time(NULL));

    InicializarParametros(argv);
    InicializarGlobales();

    int tiempo, n_metrica;
    n_metrica = 0;
    for(tiempo=0; tiempo<TIEMPO_SIMULACION; tiempo++) {
//        TIME = tiempo;
        //Actualizar Estados
        EstadosPersonas(contagiados,sanos);
        VacunarPersonas(sanos);
        //Propagación
	AplicarPropagacion();
        //Movimiento
        AplicarMovimiento(sanos);
        AplicarMovimiento(contagiados);

        if(TIEMPO_BATCH == 0 || (tiempo % TIEMPO_BATCH) == 0) {
            GuardarDatos(n_metrica);
            n_metrica++;
        }
    }
    liberarListaEnlazada(sanos);
    liberarListaEnlazada(contagiados);

    return 0;
}

void InicializarParametros(char** argv) {
    N_PERSONAS = (int) atoi(argv[1]);
    MAX_X = (int) atoi(argv[2]);
    MAX_Y = (int) atoi(argv[3]);
    TIEMPO_SIMULACION = (int) atoi(argv[4]);

    ALFA = (int) atoi(argv[5]);
    BETA = (int) atoi(argv[6]);
    PORCENT_VACUNACION = (int) atoi(argv[7]);

    TIEMPO_BATCH = (int) atoi(argv[8]);
}

void InicializarGlobales() {
    float porcent_vacunacion;

    N_SANOS = N_PERSONAS-1;
    N_CONTAGIADOS = 1;
    N_RECUPERADOS = 0;
    N_FALLECIDOS = 0;

    porcent_vacunacion = PORCENT_VACUNACION / 100;
    N_PERSONAS_VACU = (int) N_PERSONAS * porcent_vacunacion;
    N_PERSONAS_VACU_T = (int) N_PERSONAS_VACU / TIEMPO_SIMULACION;

    sanos = (ListaEnlazadaRef) malloc(sizeof(ListaEnlazada));
    contagiados = (ListaEnlazadaRef) malloc(sizeof(ListaEnlazada));

    int i;
    struct Persona* nueva_persona;

    crearVacia(sanos);
    for(i=0; i<N_SANOS; i++) {
        nueva_persona = NuevaPersona(i, 0);
        insertarNodoFinal(sanos, nueva_persona);
    }

    crearVacia(contagiados);
    nueva_persona = NuevaPersona(N_PERSONAS-1, 2);
    insertarNodoFinal(contagiados, nueva_persona);

    calcular_edad(sanos,N_SANOS);
    calcular_edad(contagiados,N_CONTAGIADOS);

    M_SANOS = 1.0;
    M_CONTAGIADOS = 1.0;
    M_RECUPERADOS = 1.0;
    M_FALLECIDOS = 1.0;
    R0 = 1.0;
}


void calcular_edad(ListaEnlazadaRef lista,int n){
    int semilla, edad,i;
    float mu;
    tipoNodoRef nodo = *lista;
    Persona *persona;
    gsl_rng *r;

    mu=100;
    semilla=1;

    gsl_rng_env_setup();
    r = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(r, semilla);

    //media edad = alfa / (alfa + beta)

    for (i=0 ; i< n; i++){
        persona = (Persona*) &nodo->info;
        edad = round(mu * gsl_ran_beta(r, ALFA, BETA));
        persona->edad = edad;
        persona->p_muerte = calcular_p_morir(edad);
        nodo = nodo->sig;
    }

    gsl_rng_free(r);
}

