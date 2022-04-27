#include <stdlib.h>
#include <inttypes.h>
#include <strings.h>
#include "datastruct.h"
/*
 *  Вспомогательные функции
 */
static void lst_append(struct Node *n, struct List *lst);                                           //добавление узла в конец списка
static void node_move(struct Node *n, struct List *src, struct List *dst);                          //перемещение узла n из списка src в список dst (!удалит узел, если dst==NULL!)

void ListClear(struct List *lst){
    if (lst==NULL) return;
    struct Node *li = NULL;
    while (lst->tail!=NULL){
        li = (lst->tail)->previous;
        free(lst->tail);
        lst->tail = li;
    }
    lst->head = NULL;
    lst->length = 0;
}

void ListAppend(struct DbRecord *data, struct List *lst){
    if (data==NULL || lst==NULL) return;
    struct Node *n = calloc(1, sizeof(struct Node));
    n->data = data;
    lst_append(n, lst);
}

static void lst_append(struct Node *n, struct List *lst){
    if (n==NULL || lst==NULL) return;
    n->next = NULL;
    if (lst->tail==NULL){           //список пуст, добавляемая вершина - головной узел
        n->previous = NULL;
        lst->head = n;
    } else{                         //добавляем вершину в конец списка
        n->previous = lst->tail;
        lst->tail->next = n;
    }
    lst->tail=n;
    lst->length++;
}

static void node_move(struct Node *n, struct List *src, struct List *dst){
                                    //переместить вершину = удалить n из списка src и добавить в список dst
    if (n==NULL || src==NULL) return;
    if (n->next == NULL){           //n хвост
        src->tail = n->previous;
        if (src->tail != NULL){
            src->tail->next = NULL;
        } else{
            src->head = NULL;
        }
    } else if (n->previous == NULL){//n голова
        n->next->previous = NULL;
        src->head = n->next;
    } else {                        //n в середине
        n->previous->next = n->next;
        n->next->previous = n->previous;
    }
    src->length--;
    if (dst!=NULL){
        lst_append(n,dst);
        dst->length++;
    }
}

void ListRemove(struct Node *n, struct List *lst){
    if (n!=NULL && lst!=NULL) node_move(n, lst, NULL);
}
