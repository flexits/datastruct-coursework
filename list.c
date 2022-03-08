#include <stdlib.h>
#include <inttypes.h>
#include <strings.h>
#include "datastruct.h"
/*
 *  Вспомогательные функции
 */
static void lst_append(struct Node *n, struct List *lst);                                           //добавление узла в конец списка
static void node_move(struct Node *n, struct List *src, struct List *dst);                          //перемещение узла n из списка src в список dst (!удалит узел, если dst==NULL!)
static void merge(struct List *lstA, struct List *lstB, struct List *out, enum keyfield srt);       //прямое слияние списков lstA и lstB в результирующий список out по заданному ключу
static void rebuild_links(struct List *lst);                                                        //восстановление структуры указателей на предыдущие элементы после разбиения списка

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

static void merge(struct List *lstA, struct List *lstB, struct List *out, enum keyfield srt){
    if (lstA==NULL || lstB==NULL || out==NULL) return;
    struct Node *a, *b;
    while(lstA->length!=0 && lstB->length!=0){
        if ((a=lstA->head)==NULL || (b=lstB->head)==NULL){
            return;
        }
        int cmp=0;
        if (srt==LAWYER){
            cmp = strcmp(a->data->lawyer, b->data->lawyer);
        } else if (srt==DEPOSITOR){
            cmp = strcmp(a->data->name, b->data->name);
        }
        if (cmp <= 0){
            node_move(a,lstA,out);
        } else {
            node_move(b,lstB,out);
        }
    }
    while (lstA->length>0){
        if ((a=lstA->head)==NULL){
            return;
        }
        node_move(a,lstA,out);
    }
    while (lstB->length>0){
        if ((b=lstB->head)==NULL){
            return;
        }
        node_move(b,lstB,out);
    }
}

static void rebuild_links(struct List *lst){
    if (lst==NULL || lst->head==NULL) return;
    (lst->head)->previous = NULL;
    lst->length = 1;
    struct Node *prev = lst->head;
    struct Node *next = prev->next;
    for(;next!=NULL;lst->length++){
        next->previous = prev;
        prev = next;
        next = next->next;
    }
    lst->tail = prev;
}

void ListSort(struct List *lst, enum keyfield srt){
    if (lst==NULL || lst->length<=0) return;
    //n - количество элементов сортируемого списка
    size_t n = lst->length;
    //разбиение списка на две части A и B
    struct List A = {NULL, NULL, 0};
    struct List B = {NULL, NULL, 0};
    struct Node *k = lst->head;
    struct Node *p = k->next;
    A.head = k;
    B.head = p;
    for (;p!=NULL;p = p->next){
        k->next = p->next;
        k = p;
    }
    //восстановление указателей на элементы списков
    rebuild_links(&A);
    rebuild_links(&B);
    //сортировка методом прямого слияния
    //p - предполагаемый размер серии
    //la - фактический размер серии в списке A, lb - в списке B
    //m - число элементов в списке
    //c0, c1 - врЕменные очереди
    //i - номер активной очереди
    //size_t la, lb;
    struct List c0 = {NULL, NULL, 0};
    struct List c1 = {NULL, NULL, 0};
    size_t i = 0;
    for (size_t p=1;p<n;p*=2){
        c0.head = c0.tail =NULL;
        c1.head = c1.tail = NULL;
        c0.length = c1.length = 0;
        i = 0;
        size_t m = n;
        while (m>0){
            A.length = (m>=p) ? p : m;
            m -= A.length;
            B.length = (m>=p) ? p : m;
            m -= B.length;
            if (i==0){
                merge(&A, &B, &c0, srt);
            } else if (i==1){
                merge(&A, &B, &c1, srt);
            }
            i = 1 - i;
        }
        A.head = c0.head;
        B.head = c1.head;
    }
    c0.tail->next = NULL;
    lst->head = c0.head;
}

void ListDistinct(struct List *lst, enum keyfield srt){
    if (lst==NULL || lst->length<=1) return;
    ListSort(lst, srt);
    //если ключ очередного элемента отсортированного списка
    //равен ключу предыдущего элемента, очередной элемент удаляется
    for (struct Node *n=lst->head->next;n!=NULL;n=n->next){
        int cmp=1;
        if (srt==LAWYER){
            cmp = strcmp(n->data->lawyer, n->previous->data->lawyer);
        } else if (srt==DEPOSITOR){
            cmp = strcmp(n->data->name, n->previous->data->name);
        }
        if (cmp == 0){
            node_move(n, lst, NULL);
        }
    }
}
