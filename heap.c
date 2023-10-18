#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "heap.h"

typedef struct nodo{
   void* data;
   int priority;
}heapElem;

typedef struct Heap{
  heapElem* heapArray;
  int size;
  int capac;
} Heap;


void* heap_top(Heap* pq) {
  if (pq == NULL || pq->size == 0) {
    printf("El montículo está vacío\n");
    return NULL;
  }
  return pq->heapArray[0].data;
}



void heap_push(Heap* pq, void* data, int priority){
  if(pq == NULL){
    printf("El montículo no es válido\n");
    return;
  }
  if (pq->size == pq->capac){
    pq->capac = pq->capac * 2 + 1;
    pq->heapArray = realloc(pq->heapArray, pq->capac * sizeof(heapElem));
    if (pq->heapArray == NULL){
      printf("Error al reasignar memoria para el arreglo del montículo\n");
      return;
    }
  }
  heapElem newElem;
  newElem.data = data;
  newElem.priority = priority;
  pq->heapArray[pq->size] = newElem;
  pq->size++;
  int index = pq->size - 1;
  int parent = (index - 1) / 2;
  while (index > 0 && pq->heapArray[index].priority > pq->heapArray[parent].priority){
    heapElem temp = pq->heapArray[index];
    pq->heapArray[index] = pq->heapArray[parent];
    pq->heapArray[parent] = temp;
    index = parent;
    parent = (index - 1) / 2;
  }
}


void heap_pop(Heap* pq) {
  if (pq == NULL) {
    printf("El montículo no es válido\n");
    return;
  }
  if (pq->size == 0) {
    printf("El montículo está vacío\n");
    return;
  }

  heapElem temp = pq->heapArray[0];
  pq->heapArray[0] = pq->heapArray[pq->size - 1];
  pq->heapArray[pq->size - 1] = temp;
  pq->size--;
  int index = 0;
  int left = index * 2 + 1;
  int right = index * 2 + 2;
  while (left < pq->size && (pq->heapArray[index].priority < pq->heapArray[left].priority || (right < pq->size && pq->heapArray[index].priority < pq->heapArray[right].priority))) {
    if (right >= pq->size || pq->heapArray[left].priority > pq->heapArray[right].priority) {
      temp = pq->heapArray[index];
      pq->heapArray[index] = pq->heapArray[left];
      pq->heapArray[left] = temp;
      index = left;
      left = index * 2 + 1;
    } else {
      temp = pq->heapArray[index];
      pq->heapArray[index] = pq->heapArray[right];
      pq->heapArray[right] = temp;
      index = right;
      right = index * 2 + 2;
    }
  }
}


Heap* createHeap() {

  Heap* heap = (Heap*) malloc(sizeof(Heap));
  if (heap == NULL) {
    printf("Error al reservar memoria para el Heap\n");
    return NULL;
  }
  heap->capac = 3;
  heap->heapArray = (heapElem*) malloc(heap->capac * sizeof(heapElem));
  if (heap->heapArray == NULL) {
    printf("Error al reservar memoria para el arreglo del Heap\n");
    free(heap); 
    return NULL;
  }
  heap->size = 0;
  return heap;
}

int heap_empty(Heap * pq) {
      if (pq->size == 0) {
        return 1;
      } else {
        return 0;
      }
    }
