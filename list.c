#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"


Node * createNode(void * data) {
    Node * new = (Node *)malloc(sizeof(Node));
    assert(new != NULL);
    new->data = data;
    new->prev = NULL;
    new->next = NULL;
    return new;
}

List * createList() {
  List* list = (List*) malloc(sizeof(List));
    if (list == NULL) {
       printf("Error: no se pudo reservar memoria para la lista\n");
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;

    return list;
}

void * firstList(List * list) {
  if (list == NULL || list->head == NULL){
    return NULL;
  }
  list->current = list->head;
  return list->current->data;
}

void * nextList(List * list) {
  if (list == NULL || list->current == NULL || list->current->next == NULL){
    return NULL;
  }
  list->current = list->current->next;
  return list->current->data;
}

void * lastList(List * list) {
  if (list == NULL || list->tail ==NULL){
    return NULL;
  }
  list->current = list->tail;
  return list->current->data;
}

void * prevList(List * list) {
  if (list == NULL || list->current == NULL || list->current->prev == NULL){
    return NULL;
  }
  list->current = list->current->prev;
  return list->current->data;
}

void pushFront(List * list, void * data){  
  if(list == NULL){
    return;
  }
  Node* node = createNode(data);
  if (node == NULL){
    return;
  }
  if (list->head == NULL){
    list->head = node;
    list->tail = node;
    list->current = node;
  }else{
    node->next = list->head;
    list->head->prev = node;
    list->head = node;
  }
}


void pushBack(List * list, void * data) {
  if(list == NULL){
    return;
  }
  Node* node = createNode(data);
  if(node == NULL){
    return;
  }
  if(list->head == NULL){
    list->head = node;
    list->tail = node;
    list->current = node;
  }else{
    node->prev = list->tail;
    list->tail->next = node;
    list->tail = node;
  }

}

void pushCurrent(List * list, void * data) {
  if (list == NULL || list->current == NULL){
    return;
  }
  Node* node = createNode(data);
  if (node == NULL){
    return;
  }
  if (list->current == list->tail){
    node->prev = list->current;
    list->current->next = node;
    list->tail = node;
  }else{
    node->prev = list->current;
    node->next = list->current->next;
    list->current->next->prev = node;
    list->current->next = node;
  }
}

void * popFront(List * list) {
  if(list==NULL || list->head == NULL){
    return NULL;
  }
  Node *temp = list->head;
  void* data = temp->data;
  if(list->head == list->tail){
    free(temp);
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
  }else{
    list->head = list->head->next;
    free(temp);
    list->head->prev = NULL;
  }
  return data;

}

void * popBack(List * list) {
  if(list==NULL || list->tail==NULL){
    return NULL;
  }
  Node* temp = list->tail;
  void* data = temp->data;
  if(list->head == list->tail){
    free(temp);
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
  }else{
    list->tail = list->tail->prev;
    free(temp);
    list->tail->next = NULL;
  }
  return data;

}

void * popCurrent(List * list) {
  if (list==NULL || list->current == NULL){
    return NULL;
  }
  void* data = list->current->data;
  if (list->current == list->head && list->current == list->tail){
    free(list->current);
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
  }else if(list->current == list->head){
    list->current = list->current->next;
    free(list->head);
    list->head = list->current;
    list->head->prev = NULL;
  }else if(list->current == list->tail){
    list->current = list->current->prev;
    free(list->tail);
    list->tail = list->current;
    list->tail->next = NULL;
  }else{
    Node* temp = list->current;
    list->current = list->current->prev;
    list->current->next = temp->next;
    temp->next->prev = list->current;
    free(temp);
  }
  return data;
}

void cleanList(List * list) {
    while (list->head != NULL) {
        popFront(list);
    }
}

int sizeList(List * list) {
    int size = 0;
    Node * node = list->head;
    while (node != NULL) {
        size++;
        node = node->next;
    }
    return size;
}