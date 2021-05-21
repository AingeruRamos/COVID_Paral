#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>

#include "globales.h"
#include "persona.h"
#include "listaEnlazadaSimple.h"
#include "estado/estado.h"
#include "propagacion/propagacion.h"
#include "move/move.h"
#include "metricas/metricas.h"

void InicializarParametros(char** argv);
void InicializarGlobales(gsl_rng *r, int world_rank, int world_size,int resto);
void calcular_edad(ListaEnlazadaRef lista,int n,gsl_rng *r,int world_rank);

int main(int argc, char** argv) {

    int world_rank, world_size;
    int tiempo, n_metrica, flag, n_personas, resto, semilla,i;
    double ini_calc,fin_calc;

    n_metrica = 0;
    flag = 0;
    resto = 0;
    gsl_rng *r;

    srand(time(NULL));

    semilla=1;
    gsl_rng_env_setup();
    r = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(r, semilla);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if(argc != 9) {
        printf("Error en los parametros\n");
        return -1;
    }

    ini_calc=MPI_Wtime();

    InicializarParametros(argv);


    resto = N_PERSONAS % world_size;
    N_PERSONAS_P =(int) N_PERSONAS / world_size;

    if( world_rank < resto ){
            N_PERSONAS_P++;
    }

    InicializarGlobales(r, world_rank, world_size,resto);
    gsl_rng_free(r);

    for(tiempo=0; tiempo<TIEMPO_SIMULACION; tiempo++) {
//        TIME = tiempo;
        //Actualizar Estados
        EstadosPersonas(contagiados,sanos);
        VacunarPersonas(sanos);
        //Propagación
	AplicarPropagacion(world_size, world_rank);
        //Movimiento
        AplicarMovimiento(sanos);
        AplicarMovimiento(contagiados);

        if(TIEMPO_BATCH == 0 || (tiempo % TIEMPO_BATCH) == 0) {
            if (tiempo == (TIEMPO_SIMULACION - 1)){
                flag = 1;
                MPI_Barrier(MPI_COMM_WORLD);
            }
            GuardarDatos(n_metrica,flag,resto,world_rank,world_size);
            n_metrica++;
        }
    }

    liberarListaEnlazada(sanos);
    liberarListaEnlazada(contagiados);

    fin_calc=MPI_Wtime();

    if(world_rank==0){
        printf("El tiempo de ejecucion es:%1.2f segundos\n",fin_calc-ini_calc);
        fflush(stdin);
    }

    MPI_Finalize();

    return 0;
}

void InicializarParametros(char** argv) {
    char *v = NULL;

    N_PERSONAS = strtol(argv[1], &v, 10);

    MAX_X = strtol(argv[2], &v, 10);

    MAX_Y = strtol(argv[3], &v, 10);

    TIEMPO_SIMULACION = strtol(argv[4], &v, 10);

    ALFA = strtol(argv[5], &v, 10);

    BETA = strtol(argv[6], &v, 10);

    PORCENT_VACUNACION = strtol(argv[7], &v, 10);

    TIEMPO_BATCH = strtol(argv[8], &v, 10);

}

void InicializarGlobales(gsl_rng *r,int world_rank,int world_size,int resto) {
    float porcent_vacunacion;
    int i;
    struct Persona* nueva_persona;

    //INICIALIZAMOS LAS VARIABLES GLOBALES
    N_RECUPERADOS = 0;
    N_FALLECIDOS = 0;
    NODO_CONTAGIADO = 0;

    if (world_rank == 0){
        NODO_CONTAGIADO = rand() % world_size; //cada ejecucion el contagiado estará en un nodo distinto
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&NODO_CONTAGIADO, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (world_rank == NODO_CONTAGIADO){
        N_SANOS = N_PERSONAS_P-1;
        N_CONTAGIADOS = 1;
    }
    else{
        N_SANOS = N_PERSONAS_P;
        N_CONTAGIADOS = 0;
    }

    //printf("%d personas sanas de %d personas del nodo %d\n",N_SANOS,N_PERSONAS_P, world_rank);
    //printf("%d personas contagiadas de %d personas del nodo %d\n",N_CONTAGIADOS,N_PERSONAS_P,world_rank);
    if (world_rank == 0){
        porcent_vacunacion = (float) PORCENT_VACUNACION / 100;
        N_PERSONAS_VACU = (int) (N_PERSONAS * porcent_vacunacion);
        N_PERSONAS_VACU_P = (int) N_PERSONAS_VACU / world_size; //numero de personas que tiene que vacunar cada nodo
        N_PERSONAS_VACU_T = (int) N_PERSONAS_VACU_P / TIEMPO_SIMULACION;
    }

    MPI_Bcast( &N_PERSONAS_VACU_P, 1, MPI_INT, 0, MPI_COMM_WORLD);

    sanos = (ListaEnlazadaRef) malloc(sizeof(ListaEnlazada));
    contagiados = (ListaEnlazadaRef) malloc(sizeof(ListaEnlazada));

    crearVacia(sanos);


    //INICIALIZAMOS LAS LISTAS
    for(i=0; i<N_SANOS; i++) {
       if(resto > world_rank || resto == 0){
           nueva_persona = NuevaPersona(world_rank * N_PERSONAS_P+i, 0);
           insertarNodoFinal(sanos, nueva_persona);
       }
       else{
           nueva_persona = NuevaPersona(world_rank * N_PERSONAS_P+i+resto, 0);
           insertarNodoFinal(sanos, nueva_persona);
       }
    }

    crearVacia(contagiados);

    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == NODO_CONTAGIADO){
       if (resto <= NODO_CONTAGIADO && resto != 0){
           printf("El nodo contagiado es %d\n",NODO_CONTAGIADO);
           nueva_persona = NuevaPersona( world_rank * N_PERSONAS_P + N_SANOS+resto, 2);
           insertarNodoFinal(contagiados, nueva_persona);
       }
       else{
           printf("El nodo contagiado es %d\n",NODO_CONTAGIADO);
           nueva_persona = NuevaPersona( world_rank * N_PERSONAS_P + N_SANOS, 2);
           insertarNodoFinal(contagiados, nueva_persona);
       }
    }

    calcular_edad(sanos,N_SANOS,r,world_rank);

    if (world_rank == NODO_CONTAGIADO){
       calcular_edad(contagiados,N_CONTAGIADOS,r,world_rank);
    }

    M_SANOS = 1.0;
    M_CONTAGIADOS = 1.0;
    M_RECUPERADOS = 1.0;
    M_FALLECIDOS = 1.0;
    R0 = 1.0;

    MPI_Barrier(MPI_COMM_WORLD);
}


void calcular_edad(ListaEnlazadaRef lista,int n,gsl_rng *r, int world_rank){
    int edad,i;
    float mu;
    int cont=0;
    tipoNodoRef nodo = *lista;
    Persona *persona;

    mu=100;
    //media edad = alfa / (alfa + beta)

 //   for (i=0 ; i< n; i++){
    while(nodo != NULL){
       persona = (Persona*) &nodo->info;
        edad = round(mu * gsl_ran_beta(r, ALFA, BETA));
        persona->edad = edad;
        persona->p_muerte = calcular_p_morir(edad);
        printf("Persona %d del nodo %d \n",persona->id, world_rank);
        nodo = nodo->sig;
        cont++;
    }

   // printf("El contador es %d del world_rank %d\n", cont, world_rank);
}

