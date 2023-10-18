#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"



int enlarge_called=0;

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
  long valorHash = hash(key,map->capacity);

  while(map->buckets[valorHash]!=NULL && map->buckets[valorHash]->key!=NULL){
    if(strcmp(map->buckets[valorHash]->key,key)==0){
      return;
    }
    valorHash=(valorHash+1)%map->capacity;
  }
  Pair* nuevoPar=createPair(key,value);
  map->buckets[valorHash]=nuevoPar;
  map->size++;
  map->current=valorHash;
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
  Pair** old_buckets = map->buckets;
  int old_capacity = map->capacity;
  map->capacity=map->capacity*2;
  map->buckets = (Pair**)malloc(map->capacity*sizeof(Pair*));
  map->size=0;
  for(int i=0;i<map->capacity;i++){
    map->buckets[i]=NULL;
  }
  for(int i=0;i<old_capacity;i++){
    if(old_buckets[i]!=NULL && old_buckets[i]->key!=NULL){
      insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
    }
  }
  free(old_buckets);
}


HashMap * createMap(long capacity) {
  HashMap* nuevoMapa = (HashMap*)malloc(sizeof(HashMap));
  nuevoMapa->buckets=(Pair**)malloc(capacity*sizeof(Pair*));
  for(int i=0;i<capacity;i++){
    nuevoMapa->buckets[i]=NULL;
  }
  nuevoMapa->capacity=capacity;
  nuevoMapa->size=0;
  nuevoMapa->current=-1;
    return nuevoMapa;
}

void eraseMap(HashMap * map,  char * key) {    
  Pair* par = searchMap(map,key);
  if(par!=NULL){
    par->key=NULL;
    map->size--;
  }
}

Pair * searchMap(HashMap * map,  char * key) {   
  long valorHash = hash(key,map->capacity);
  while (map->buckets[valorHash]!=NULL){
    if (strcmp(map->buckets[valorHash]->key,key)==0){
      map->current = valorHash;
      return map->buckets[valorHash];
    }
    valorHash=valorHash+1;

  }

  return NULL;
}

Pair * firstMap(HashMap * map) {
  for(int i=0;i<map->capacity;i++){
    if(map->buckets[i]!=NULL && map->buckets[i]->key!=NULL){
      map->current=i;
      return map->buckets[i];
    }
  }
  return NULL;
}

Pair * nextMap(HashMap * map) {
  for(int i=map->current+1;i<map->capacity;i++){
    if(map->buckets[i]!=NULL && map->buckets[i]->key!=NULL){
      map->current=i;
      return map->buckets[i];
    }
  }
    return NULL;
}
