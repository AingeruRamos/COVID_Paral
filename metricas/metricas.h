#ifndef _METRICAS_H_
#define _METRICAS_H_

void GuardarPosiciones(char *ruta,int world_rank,int resto);
void GuardarMetricas(char *ruta);
void GuardarDatos(int n_metricas,int flag,int resto,int world_rank, int world_size);

#endif
