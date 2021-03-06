#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <mpi.h>

#include "../globales.h"
#include "../listaEnlazadaSimple.h"

void GuardarPosiciones(char* nombre,int world_rank,int resto) {
    MPI_File fh;
    MPI_Offset offset;
    char *nombreFichero = (char*) malloc(sizeof(char)*200);
    char *buf = (char*) calloc(sizeof(char),200);
    int length;

    strcpy(nombreFichero, nombre);
    strcat(nombreFichero, ".pos");

    sprintf(buf,"ID: %d; Posicion(X: %d, Y: %d); Estado: %d\n",N_PERSONAS,MAX_X,MAX_Y,0);
    if (world_rank == 0){
      length = strlen(buf);
      length = length*N_PERSONAS_P;
    }
    MPI_Bcast(&length,1,MPI_INT,0,MPI_COMM_WORLD);
    offset = world_rank*length;

    MPI_File_open(MPI_COMM_WORLD, nombreFichero,MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
    MPI_File_seek(fh,offset,MPI_SEEK_CUR);

    MPI_File_write(fh,"\n",strlen("\n"),MPI_CHAR, MPI_STATUS_IGNORE);
    tipoNodoRef nodo_aux = *sanos;
    struct Persona* persona;
    while(nodo_aux != NULL) {
        persona = (struct Persona*) &nodo_aux->info;
        sprintf(buf,"ID: %d; Posicion(X: %d, Y: %d); Estado: %d\n", persona->id,persona->pos.x, persona->pos.y,persona->estado);
        MPI_File_write(fh,buf,strlen(buf),MPI_CHAR, MPI_STATUS_IGNORE);
        nodo_aux = nodo_aux->sig;
    }

    nodo_aux = *contagiados;
    while(nodo_aux != NULL) {
        persona = (struct Persona*) &nodo_aux->info;
        sprintf(buf,"ID: %d; Posicion(X: %d, Y: %d); Estado: %d\n", persona->id,persona->pos.x, persona->pos.y,persona->estado);
        MPI_File_write(fh,buf,strlen(buf),MPI_CHAR, MPI_STATUS_IGNORE);
        nodo_aux = nodo_aux->sig;
    }

    MPI_File_close(&fh);
    free(buf);
    free(nombreFichero);
}

void GuardarMetricas(char* nombre) {
    char* nombreFichero = (char*) malloc(sizeof(char)*200);
    strcpy(nombreFichero, nombre);
    strcat(nombreFichero, ".metricas");

    FILE* fp = fopen(nombreFichero, "w");

    M_SANOS = (float) N_SANOS / N_PERSONAS;
    M_CONTAGIADOS = (float) N_CONTAGIADOS / N_PERSONAS;
    M_RECUPERADOS =(float) N_RECUPERADOS /N_PERSONAS;
    M_FALLECIDOS = (float) N_FALLECIDOS / N_PERSONAS;
    if (N_CONTAGIADOS == 0){
        R0 = N_CONTAGIADOS;
    }
    else{
	R0= N_CONTAGIADOS-1;
    }

    fprintf(fp, "SANOS: %f\n", M_SANOS);
    fprintf(fp, "CONTAGIADOS: %f\n", M_CONTAGIADOS);
    fprintf(fp, "RECUPERADOS: %f\n", M_RECUPERADOS);
    fprintf(fp, "FALLECIDOS: %f\n", M_FALLECIDOS);
    fprintf(fp, "R0: %f\n", R0);

    fclose(fp);
    free(nombreFichero);
}

void GuardarDatos(int id_metricas, int flag,int resto,int world_rank, int world_size) {
    char* ruta = (char*) malloc(sizeof(char)*200);
    sprintf(ruta, "datos/%d_%d_%d_%d_%d_%d_%d/", N_PERSONAS, MAX_X, MAX_Y, TIEMPO_SIMULACION, ALFA, BETA, PORCENT_VACUNACION);

    if (world_rank == 0){
        mkdir(ruta, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    sprintf(ruta, "%s%d", ruta, id_metricas);

    GuardarPosiciones(ruta,world_rank, resto);

    if (flag == 1){

        if (world_rank == 0)
            MPI_Reduce(MPI_IN_PLACE,&N_SANOS,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
        else
            MPI_Reduce(&N_SANOS,&N_SANOS,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

        if (world_rank == 0)
            MPI_Reduce(MPI_IN_PLACE,&N_CONTAGIADOS,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
        else
            MPI_Reduce(&N_CONTAGIADOS,&N_CONTAGIADOS,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

        if (world_rank == 0)
            MPI_Reduce(MPI_IN_PLACE,&N_RECUPERADOS,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
        else
            MPI_Reduce(&N_RECUPERADOS,&N_RECUPERADOS,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

        if (world_rank == 0)
            MPI_Reduce(MPI_IN_PLACE,&N_FALLECIDOS,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
        else
            MPI_Reduce(&N_FALLECIDOS,&N_FALLECIDOS,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

        if (world_rank == 0)
            MPI_Reduce(MPI_IN_PLACE,&R0,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
        else
            MPI_Reduce(&R0,&R0,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

        if(world_rank == 0)
           GuardarMetricas(ruta);
    }

    free(ruta);
}
