/*
 * EOS - Experimental Operating System
 * Double linked list data structure implementation header
 */

#ifndef _LIST_H
#define _LIST_H

#include <stddef.h>
#include <stdint.h>

typedef struct listnode {
    struct listnode *prev; // 0
    struct listnode *next; // 4
    void *val;             // 8
} listnode_t;

typedef struct list {
    listnode_t *head; // 0
    listnode_t *tail; // 4
    uint32_t size;    // 8
} list_t;

#define foreach(t, list) for (listnode_t *t = list->head; t != NULL; t = t->next)

list_t *list_create();

uint32_t list_size(list_t *list);

listnode_t *list_insert_front(list_t *list, void *val);
listnode_t *list_insert_back(list_t *list, void *val);

void *list_remove_node(list_t *list, listnode_t *node);
void *list_remove_front(list_t *list);
void *list_remove_back(list_t *list);

listnode_t *list_push(list_t *list, void *val);
listnode_t *list_pop(list_t *list);

void list_enqueue(list_t *list, void *val);
listnode_t *list_dequeue(list_t *list);

void *list_peek_front(list_t *list);
void *list_peek_back(list_t *list);

void list_destroy(list_t *list);
void listnode_destroy(listnode_t *node);

int list_contain(list_t *list, void *val);

listnode_t *list_get_node_by_index(list_t *list, int index);
void *list_remove_by_index(list_t *list, int index);

#endif
