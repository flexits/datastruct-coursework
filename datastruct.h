#ifndef DATASTRUCT_H_INCLUDED
#define DATASTRUCT_H_INCLUDED

#define DBFILENAME "BASE3.DAT"  //имя файла БД
#define DBREC_NAME_LENGTH   32  //размеры текстовых полей
#define DBREC_DATE_LENGTH   8
#define DBREC_LAYR_LENGTH   22
#define MAX_LINE_NEEDED (DBREC_DATE_LENGTH+DBREC_LAYR_LENGTH+DBREC_NAME_LENGTH+8+1) //максимальная длина строки, содержащей значения всех полей, разделённых пробелами, и \0 в конце
/*
 *  Структуры данных
 */
struct DbRecord {char name[DBREC_NAME_LENGTH+1]; int sum; char date[DBREC_DATE_LENGTH+1]; char lawyer[DBREC_LAYR_LENGTH+1];};   //запись из БД, размещаемая в памяти
struct Vertex {struct DbRecord *data; struct Vertex *left; struct Vertex *right; int bal;};                                     //вершина дерева (хранит указатель на запись из БД)
struct Node {struct DbRecord *data; struct Node *previous; struct Node *next;};                                                 //узел списка (хранит указатель на запись из БД)
struct List {struct Node *head; struct Node *tail; size_t length;};                                                             //список: указатели на заглавный и конечный узлы, общее кол-во узлов
/*
 *  Функции работы с данными
 */
struct Vertex* TreeInsert(struct DbRecord *data, struct Vertex *root);      //добавляет запись из БД в АВЛ-дерево, возвращает вершину дерева (ключ - поле name записи)
void TreeDestruct(struct Vertex *root);                                     //удаляет все вершины дерева, высвобождая занятую память
void TreeSearch(const char* name, const char* lawyer, struct Vertex *root, struct List *result); //поиск записей в дереве, для которых поле name начинается с name, поле lawyer совпадает с lawyer, и добавление ссылок на найденное в список
void ListAppend(struct DbRecord *data, struct List *lst);                   //добавление узла в конец списка
void ListRemove(struct Node *n, struct List *lst);                          //удаление узла из списка
void ListClear(struct List *lst);                                           //удаление всех узлов списка
enum keyfield {DEPOSITOR, LAWYER};                                          //выбор поля сортировки списка - имя вкладчика или имя адвоката
void ListSort(struct List *lst, enum keyfield srt);                         //сортировка списка методом прямого слияния по заданному полю
void ListDistinct(struct List *lst, enum keyfield srt);                     //удаление дублирующихся ключей из списка (список будет отсортирован)

#endif // DATASTRUCT_H_INCLUDED
