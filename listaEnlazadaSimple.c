//
//  listaEnlazadaSimple.c
//  listaEnlazadaSimple


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "listaEnlazadaSimple.h"

/*
 * Función presentada en clase de teoría
 * Todos los aspectos sobre funcionamiento y valor a devolver
 * están descritos en las diapositivas
 */
int
crearVacia(ListaEnlazadaRef raiz)
{
	
	*raiz = NULL;
	return 0;

}

/*
 * Función presentada en clase de teoría
 * Todos los aspectos sobre funcionamiento y valor a devolver
 * están descritos en las diapositivas
 */
int
estaVacia(ListaEnlazada raiz)
{

	return (raiz == NULL);

}

/*
 * Función presentada en clase de teoría
 * Todos los aspectos sobre funcionamiento y valor a devolver
 * están descritos en las diapositivas
 */
tipoNodoRef
creaNodo(tipoInfoRef info)
{

	tipoNodoRef nuevo;
	if ((nuevo = (tipoNodoRef)malloc(sizeof(tipoNodo))) == NULL) {
		#ifdef DEBUG
		fprintf(stderr, "Error: no reserva memoria!\n");
		#endif
	} else {
		nuevo->info = *info;
		nuevo->sig = NULL;
	}
	return nuevo;

}

/*
 * Función presentada en clase de teoría. Se debe implementar
 * la última versión presentada en clase y disponible en las diapositivas.
 * Todos los aspectos sobre funcionamiento y valor a devolver
 * están descritos en las diapositivas
 */
int
insertarDespuesDeNodo(ListaEnlazadaRef raiz, tipoNodoRef pos, tipoInfoRef info)
{

	tipoNodoRef nuevo, aux, ant;
	
	if ((nuevo = creaNodo(info)) == NULL)
		return -1;
	else {
		if (estaVacia(*raiz))
			*raiz = nuevo;
		else {
			aux = *raiz;
			while (aux != NULL && aux != pos) {
				ant = aux;
				aux = aux->sig;
			}
			if (aux != NULL && pos != NULL) {
				nuevo->sig = pos->sig;
				pos->sig = nuevo;
			}
			else if (pos == NULL)
				ant->sig = nuevo;
			else {	
				free (nuevo);
				#ifdef DEBUG
				fprintf(stderr, "Error: %p posicion inexistente\n", pos);
				#endif
				return -2;
			}
		}
		return 0;
	} 

}


/*
 * Función presentada en clase de teoría. Se debe implementar
 * la última versión presentada en clase y disponible en las diapositivas.
 * Todos los aspectos sobre funcionamiento y valor a devolver
 * están descritos en las diapositivas
 */
int
insertarAntesDeNodo(ListaEnlazadaRef raiz, tipoNodoRef pos, tipoInfoRef info)
{

	tipoNodoRef nuevo, ant;
		
	if ((nuevo = creaNodo(info)) == NULL)
		return -1;
	else {
		if (estaVacia(*raiz) || pos == NULL || pos == *raiz) {
			nuevo->sig = *raiz;
			*raiz = nuevo;
		} else {
			ant = *raiz;
			while (ant != NULL && ant->sig != pos)
				ant = ant->sig;
			if (ant != NULL) {
				nuevo->sig = pos;
				ant->sig = nuevo;
			} else {
				free (nuevo);
				#ifdef DEBUG
				fprintf(stderr, "Error: %p posicion inexistente\n", pos);
				#endif
				return -2;
			}
		}
		return 0;
	}

}

/*
 * Función presentada en clase de teoría. Se debe implementar
 * la última versión presentada en clase y disponible en las diapositivas.
 * Todos los aspectos sobre funcionamiento y valor a devolver
 * están descritos en las diapositivas
 */
int
insertarNodoComienzo(ListaEnlazadaRef raiz, tipoInfoRef info)
{

	tipoNodoRef nuevo;
		
	if ((nuevo = creaNodo(info)) == NULL)
		return -1;

    nuevo->sig = *raiz;
    *raiz = nuevo;
	
	return 0;
}

/*
 * Función presentada en clase de teoría. Se debe implementar
 * la última versión presentada en clase y disponible en las diapositivas.
 * Todos los aspectos sobre funcionamiento y valor a devolver
 * están descritos en las diapositivas
 */
int
insertarNodoFinal(ListaEnlazadaRef raiz, tipoInfoRef info)
{

	tipoNodoRef nuevo, ultimo;
	
	ultimo = *raiz;	
	
   	if (!estaVacia(*raiz))
    	while (ultimo->sig != NULL)
      		ultimo = ultimo->sig;

	if ((nuevo = creaNodo(info)) == NULL)
		return -1;

  	// El ultimo nodo está apuntado por ultimo
    if (estaVacia(*raiz)) {
    	*raiz = nuevo;
        ultimo = nuevo;
  	} else {
        ultimo->sig = nuevo;
        ultimo = nuevo;  // Siempre dejamos apuntado el último nodo
	}

	return 0;
}

/*
 * Esta función recibe una lista (por referencia), un índice entero y una
 * variable de tipoInfo por referencia. Debe insertar un nuevo nodo
 * en la posición indicada por el índice entero (implica recorrido contando
 * nodos hasta el valor indicado por indice). Ojo, hay que garantizar que 
 * esa posición existe, es decir, que si la lista tiene 5 nodos, (los numeramos
 * de 0 a 4), las posiciones válidas son 0, 1, 2, 3, 4, 5 (la posición 5
 * es equivalente a añadir al final).
 * Si la posición no existe, no se puede hacer la inserción (en nuestro ejemplo,
 * la posición -1 ya no existe, y la posición 6 tampoco).
 * Se puede hacer uso de alguna de las funciones implementadas, aunque implique 
 * ineficiencias (recorridos adicionales de la lista enlazada). Opcional 
 * escribir una versión eficiente que no implique repetir recorridos.
 */
int
insertarNodoPosicionIndice(ListaEnlazadaRef raiz, int indice, tipoInfoRef info)
{

	tipoNodoRef ind, ant, nuevo;
	int counter;	


	if (!estaVacia(*raiz) && indice >= 0 && indice < mostrarNumeroNodos(raiz)) {
		ind = *raiz;
		ant = NULL;
		counter = 0;

		while (ind->sig != NULL && counter < indice) {
			ant = ind;			
			ind = ind->sig;
			counter++;
		}
	}
		
	if ((nuevo = creaNodo(info)) == NULL)
		return -1;

    if (ant == NULL) {
		//Primera posicion
    	nuevo->sig = *raiz;
       	*raiz = nuevo;
		return 0;
  	} else {
		nuevo->sig = ind;
		ant->sig = nuevo;
		return 0;
	}

	return -1;

}


/*
 * Función presentada en clase de teoría. Se debe implementar
 * la última versión presentada en clase y disponible en las diapositivas.
 * Todos los aspectos sobre funcionamiento y valor a devolver
 * están descritos en las diapositivas
 */
int
eliminarNodo(ListaEnlazadaRef raiz, tipoNodoRef pos)
{

	tipoNodoRef ant;	
	if (estaVacia(*raiz))
		return -1;
	else if (pos == NULL)
		return -2;
	else if (pos == *raiz) {
		*raiz = (*raiz)->sig;
		free (pos);
		return 0;
	} else {
		ant = *raiz;
		while (ant != NULL && ant->sig != pos)
			ant = ant->sig;
		if (ant != NULL) {
			ant->sig = pos->sig;
			free (pos);
			return 0;	
		} else
			return -3;
	} 
}


/*
 * Función presentada en clase de teoría. Se debe implementar
 * la última versión presentada en clase y disponible en las diapositivas.
 * Todos los aspectos sobre funcionamiento y valor a devolver
 * están descritos en las diapositivas
 */
int
eliminarNodoComienzo(ListaEnlazadaRef raiz)
{

	tipoNodoRef indice;

	if (!estaVacia(*raiz)) {	
		indice = *raiz;
		*raiz = (*raiz)->sig;
		free(indice);
		return 0;
	} else {
		#ifdef DEBUG
			fprintf(stderr, "Error: la lista esta vacia\n");
		#endif
		return -1;
	}	
}


/*
 * Función presentada en clase de teoría. Se debe implementar
 * la última versión presentada en clase y disponible en las diapositivas.
 * Todos los aspectos sobre funcionamiento y valor a devolver
 * están descritos en las diapositivas
 */
int
eliminarNodoFinal(ListaEnlazadaRef raiz)
{
	tipoNodoRef ant, indice;	
	if (!estaVacia(*raiz)) {
		indice = *raiz;
		ant = NULL;
	
		while (indice->sig != NULL) {
			ant = indice;
			indice = indice->sig;
		}
	
		if (ant == NULL) {
			//Solo existe un nodo;
			*raiz = NULL;
			free (indice);
			return 0;
		} else {
			//Existe mas de un nodo
			ant->sig = NULL;
			free (indice);
			return 0;
		}
	}
	return -1;
}

/*
 * Esta función recibe una lista (por referencia) y un índice entero
 * Debe eliminar el nodo situado en en la posición indicada por el
 * índice entero (implica recorrido contando nodos hasta el valor
 * indicado por indice). Ojo, hay que garantizar que
 * esa posición existe, es decir, que si la lista tiene 5 nodos (los numeramos
 * de 0 a 4), las posiciones válidas son 0, 1, 2, 3, 4.
 * Si la posición no existe, no se puede hacer la eliminación (en nuestro ejemplo,
 * la posición -1 ya no existe, y la posición 5 tampoco).
 * Se puede hacer uso de alguna de las funciones implementadas, aunque 
 * implique ineficiencias (recorridos adicionales de la lista enlazada). 
 * Opcional escribir una versión eficiente que no implique repetir recorridos.
 */
int
eliminarNodoPosicionIndice(ListaEnlazadaRef raiz, int indice)
{

	tipoNodoRef ind, ant;


	if (!estaVacia(*raiz) && indice >= 0) {
		ind = *raiz;
		ant = NULL;

		while (ind->sig != NULL && ind->info.id != indice) {
			ant = ind;
			ind = ind->sig;
		}

		if (ant == NULL) {
			//Primera posición
			*raiz = (*raiz)->sig;
			free (ind);
			return 0;
		} else {
			//Existe mas de un nodo
			ant->sig = ind->sig;
			free (ind);
			return 0;
		}
	}
	return -1;

}



/*
 * Función presentada en clase de teoría. Se debe implementar
 * la última versión presentada en clase y disponible en las diapositivas.
 * Todos los aspectos sobre funcionamiento y valor a devolver
 * están descritos en las diapositivas
 */
int
liberarListaEnlazada(ListaEnlazadaRef raiz)
{
		
	tipoNodoRef aborrar;
	aborrar = *raiz;

	while (*raiz != NULL) {
		*raiz = (*raiz)->sig;
		free (aborrar);
		aborrar = *raiz;	
	}
	return 0;
}

int mostrarNumeroNodos(ListaEnlazadaRef raiz)
{
	tipoNodoRef indice;
	int cuantos = 0;
   	indice = *raiz;
   	while (indice != NULL) {
      	cuantos++;
      	indice = indice-> sig;
   	}
	return cuantos;

}
/********************************************************************
 *                                                                  *
 * Las cuatro funciones que vienen a continuación dependen de cómo  *
 * está definido el tipo de dato tipoInfo. No es lo mismo mostrar   *
 * por pantalla el contenido de los datos almacenados en una lista  *
 * enlazada si éste es de tipo entero, o si se trata de un registro.*
 * Adicionalmente, y para que el alumno no pierda el tiempo         *
 * en implementar funciones que aportan poco desde un punto de vista*
 * docente, se proporcionan ya implementadas las funciones          *
 * mostrarListaEnlazada() y crearListaValoresAleatorios(). Las otras*
 * dos son sencillas y se dejan al alumno su implementación.        *
 *                                                                  *
 *******************************************************************/

/*
 * Esta función recibe una lista (por referencia) y un índice entero
 * Debe devolver la información almacenada en el nodo situado en la 
 * posición indicada por el índice entero (implica recorrido contando nodos 
 * hasta el valor indicado por indice). Ojo, hay que garantizar que
 * esa posición existe, es decir, que si la lista tiene 5 nodos (los numeramos
 * de 0 a 4), las posiciones válidas son 0, 1, 2, 3, 4.
 * Si la posición no existe, no se puede devolver nada (en nuestro ejemplo,
 * la posición -1 ya no existe, y la posición 5 tampoco).
 * IMPORTANTE: tipoInfo es, en este caso particular, un entero. Aunque
 * la dependencia es pequeña, si existe. ¿Cómo se devuelve el código de error
 * en caso de no existir la posición indicada por el parámetro indice?. Si 
 * tipoInfo es un int es sencillo, si es un registro también, pero ya es
 * diferente implementación.
 */
/*
tipoInfo
devolverInfoPosicionIndice(ListaEnlazada raiz, int indice)
{

	tipoNodoRef ind;
	int counter, flag;

	if (indice < 0 || indice >= mostrarNumeroNodos(&raiz) || estaVacia(raiz))
		return -1;

	ind = raiz;
	counter = 0;
	flag = mostrarNumeroNodos(&raiz);

	while (counter < flag) {
		if (counter == indice)
			return ind->info;
		counter++;
		ind = ind->sig;
	}

}
*/
/*
 * Función presentada en clase de teoría. Se debe implementar
 * la versión presentada en clase y disponible en las diapositivas.
 * Todos los aspectos sobre funcionamiento y valor a devolver
 * están descritos en las diapositivas
 */
/*
int
insertarOrdenada(ListaEnlazadaRef raiz, tipoInfoRef info)
{

	tipoNodoRef indice, ant;
	tipoNodoRef nuevo;
		
	if ((nuevo = creaNodo(info)) == NULL)
		return -1;

    // Si lista vacia el ultimo nodo es el primero
	indice = *raiz;
	ant = NULL;

    if (indice == NULL)
    	*raiz = nuevo;
    else {
*/		/* Recorremos lista hasta un nodo con contenido
		mayor al nodo a insertar hasta llegar al final */
/*
      while (indice->info < nuevo->info && indice->sig != NULL) {
           	ant = indice;
	        indice = indice->sig;
		}
	
		if (indice->sig == NULL && indice->info < nuevo->info) {
			// Insertamos al final de la lista
			nuevo->sig = NULL;
			indice->sig = nuevo;
		} else {
			if (ant == NULL) {
				// Insertamos al principio de la lista
				nuevo->sig = *raiz;
				*raiz = nuevo;
			} else {
				// Insertamos intermedio de la lista
				nuevo->sig = indice;
				ant->sig = nuevo;
			}
		}
	}
}
*/

int
mostrarListaEnlazada(ListaEnlazada raiz)
{
	tipoNodoRef aImprimir;
	int res = 0,i = 0;
	
	printf("\n\n");
	printf("%-14s%-10s\n","  Posición", "Valor");
	printf("%-14s%-10s\n","  ==========", "======");


	aImprimir = raiz;
	while (aImprimir != NULL) {
		printf("\t%3d)\t%7d\n",i++, aImprimir->info.id);
		aImprimir = aImprimir->sig;
	}
	return res;
}


/*
int
crearListaValoresAleatorios(ListaEnlazadaRef raiz, int numNodos)
{
	int i;
	tipoInfo temp;
	
	if (estaVacia(*raiz)) {
		srand(time(NULL));
		for (i = 0; i < numNodos; i++) {
            temp = rand()%10000;
            insertarDespuesDeNodo(raiz, NULL, &temp);
            //insertarAntesDeNodo(raiz, NULL, &temp);
            //insertarNodoFinal(raiz, &temp);
			//insertarNodoComienzo(raiz, &temp);
		}
		return 0;
	}
	return -1;
}
*/


