#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include "datastruct.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

/*
 *  Вспомогательные функции - повороты и вставка вершины.
 *  Каждая ф-ция возвращает ссылку на актульную вершину (под)дерева.
 */
static struct Vertex* rotateLL(struct Vertex *v);
static struct Vertex* rotateLR(struct Vertex *v);
static struct Vertex* rotateRR(struct Vertex *v);
static struct Vertex* rotateRL(struct Vertex *v);
static struct Vertex* insert(struct Vertex *v, struct Vertex *root, bool *grown); //добавляемая вершина, вершина дерева, флаг роста (1 - дерево выросло, 0 - нет)

static struct Vertex* rotateLL(struct Vertex *v){
    struct Vertex *q = v->left;
    q->bal = 0;
    v->bal = 0;
    v->left = q->right;
    q->right = v;
    return q;
}

static struct Vertex* rotateLR(struct Vertex *v){
    struct Vertex *q = v->left;
    struct Vertex *r = q->right;
    if (r->bal < 0){
        v->bal = 1;
    }
    else{
        v->bal = 0;
    }
    if (r->bal > 0){
        q->bal = -1;
    }
    else{
        q->bal = 0;
    }
    r->bal = 0;
    v->left = r->right;
    q->right = r->left;
    r->left = q;
    r->right = v;
    return r;
}

static struct Vertex* rotateRR(struct Vertex *v){
    struct Vertex *q = v->right;
    q->bal = 0;
    v->bal = 0;
    v->right = q->left;
    q->left = v;
    return q;
}

static struct Vertex* rotateRL(struct Vertex *v){
    struct Vertex *q = v->right;
    struct Vertex *r = q->left;
    if (r->bal > 0){
        v->bal = -1;
    }
    else{
        v->bal = 0;
    }
    if (r->bal < 0){
        q->bal = 1;
    }
    else{
        q->bal = 0;
    }
    r->bal = 0;
    v->right = r->left;
    q->left = r->right;
    r->left = v;
    r->right = q;
    return r;
}

static struct Vertex* insert(struct Vertex *v, struct Vertex *root, bool *grown)
{
	if (root==NULL){
        *grown = true;
        return v;
	}
	int cmp = strcmp(v->data->name, root->data->name);
	if (cmp<0){
        root->left = insert(v, root->left, grown);
        if (*grown){
            if (root->bal > 0){
                root->bal = 0;
                *grown = false;
            }
            else if (root->bal == 0){
                root->bal = -1;
            }
            else if (root->bal < 0){
                if (root->left->bal < 0){
                    root = rotateLL(root);
                }
                else{
                    root = rotateLR(root);
                }
                *grown = false;
            }
        }
	} else if (cmp>=0){
	    root->right = insert(v, root->right, grown);
        if (*grown){
            if (root->bal < 0){
                root->bal = 0;
                *grown = false;
            }
            else if (root->bal == 0){
                root->bal = 1;
            }
            else if (root->bal > 0){
                if (root->right->bal > 0){
                    root = rotateRR(root);
                }
                else{
                    root = rotateRL(root);
                }
                *grown = false;
            }
        }
	}
	return root;
}

struct Vertex* TreeInsert(struct DbRecord *data, struct Vertex *root)
{
    if (data==NULL) return root;                        //нечего добавлять
    struct Vertex *v = calloc(1,sizeof(struct Vertex));
    if (v==NULL) return NULL;                           //ошибка выделения памяти
    v->data = data;                                     //присваиваем ссылку на данные
    v->left = NULL;
    v->right = NULL;
    v->bal = 0;
	bool grown = false;
	return insert(v, root, &grown);
}

void TreeDestruct(struct Vertex *root){                 //рекурсивно удаляем все вершины дерева
    if (root==NULL) return;
    TreeDestruct(root->left);
    TreeDestruct(root->right);
    free(root);
}

void TreeSearch(const char* key, struct Vertex *root, struct List *result){
    if (root==NULL) return;
    struct Vertex *current = root;
    for (;current!=NULL;){
        int cmp = strncmp(key, current->data->name, strlen(key)); //0 для пустых строк
        if (cmp>0){                                     //начав с корня, движемся в левое или правое поддерево...
            current = current->right;
        } else if (cmp<0){
            current = current->left;
        } else {
            ListAppend(current->data, result);          //...до нахождения совпадения, но не останавливаемся, т.к. частичное совпадение не обязательно единственное
            TreeSearch(key, current->right, result);
            TreeSearch(key, current->left, result);
            return;
        }
    }
}
