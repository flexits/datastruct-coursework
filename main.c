#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>
#include <inttypes.h>
#include <ctype.h>
#include <conio.h>
#include <stdbool.h>
#include "curses.h"
#include "datastruct.h"
#include "uicurs.h"

/*
 *  Информация из БД загружается в память компьютера последовательным считыванием записей; под каждую запись выделяется пространство в ОЗУ (структура struct DbRecord).
 *  Доступ к загруженной информации осуществляется посредством списка указателей на структуры в памяти.
 *  Индексация и сортировка и прочие действия происзодятся только над указателями; и загруженные данные, и их размещение в пямти остаются неизменны.
 *  Поскольку требуется осуществлять доступ по разным ключам (вкладчик и адвокат), используем два разных списка указателей.
 */

/*
 *  Вспомогательные функции
 */
static int string_trim(char *str);          //удалить пробелы в конце строки; возвращает ERR, если изначальная строка пустая
static int string_trimlstchr(char *str);    //удалить один символ в конце строки; возвращает ERR, если изначальная строка пустая
static int ReadDbFile(const char* fname, struct List *dbcontents);   //считать содержимое БД из файла в список; возвращает OK/ERR
static wndresult_t PrintLawyers(const struct List *dbcontents);      //вывод имён адвокатов из списка, хранящего содержимое БД
static wndresult_t SearchName(struct Vertex *treeroot);              //выполнить поиск записи в дереве по имени

int main()
{
    //загрузить содержимое файла в ОЗУ
    struct List *dbcontents = calloc(1, sizeof(struct List));
    if (ReadDbFile(DBFILENAME, dbcontents)==ERR){
        UIDestroy();
        return ERR;
    }

    //инициализация пользовательского интерфейса
    if (UIInit() == ERR) return ERR;

    //построить из загруженных данных дерево поиска, отображая индикатор прогресса
    struct Vertex *treeroot = NULL;
    size_t counter = 0;
    for (struct Node *n=dbcontents->head; n!=NULL; n=n->next, counter++){
        UIProgressbarDisplay(counter*100/dbcontents->length, "Построение АВЛ дерева");
        treeroot = TreeInsert(n->data,treeroot);
    }
    UIProgressbarDestroy();

    //отображение окон пользовательского интерфейса в цикле до завершения программы
    wndresult_t retcode = UIDisplayDbrecords(dbcontents, DBFILENAME);
    bool finish = FALSE;
    for(;!finish;){
        switch(retcode){
        case ERROR:
        case EXIT:
            finish = TRUE;
            break;
        case DOSEARCH:      //запустить поиск записи
            retcode = SearchName(treeroot);
            break;
        case SHOWDB:        //вывести содержимое БД
            retcode = UIDisplayDbrecords(dbcontents, DBFILENAME);
            break;
        case SHOWLR:        //вывести список адвокатов
            retcode = PrintLawyers(dbcontents);
            break;
        }
    }
    //по завершению программы деинициализируем интерфейс и освобождаем память
    UIDestroy();
    TreeDestruct(treeroot);
    ListClear(dbcontents);
    free(dbcontents);
    if (retcode == ERROR) return ERR;
    else return OK;
}

static int string_trim(char *str){
    if (str == NULL) return ERR;
    size_t len=strlen(str);
    if (len==0) return ERR;   //пустая строка
    for (--len;len>0;len--){                    //пропустив завершающий '\0', двигаясь справа налево,
        if (isspace(str[len])) str[len] = '\0'; //все "пустые" символы заменить на '\0',
        else break;                             //до первого непустого
    }                                           //если в цикле дошли до начала строки и символ в начале пустой - строка пустая
    if (len==0 && (str[len]=='\0' || isspace(str[len]))) return ERR;
    else return OK;
}

static int string_trimlstchr(char *str){
    if (str == NULL) return ERR;
    size_t len=strlen(str);
    if (len==0) return ERR;   //пустая строка
    str[len-1] = '\0';
    return OK;
}

static int ReadDbFile(const char* fname, struct List *dbcontents){
    FILE *fptr = fopen(fname, "r");
    if (fptr == NULL) return ERR;
    ListClear(dbcontents);
    char name[DBREC_NAME_LENGTH+1];
    while(!feof(fptr)){                         //построчно считываем файл, пропуская строки, где начало (т.е. имя вкладчика) - пустое
        memset(name, '\0', DBREC_NAME_LENGTH+1);
        fgets(name, DBREC_NAME_LENGTH+1, fptr);
        if (string_trim(name)==ERR) continue;
        struct DbRecord *record = calloc(1, sizeof(struct DbRecord));
        if (record == NULL){                    //для каждой строки (т.е. записи) выделяем в памяти новую структуру и записываем в неё данные
            fclose(fptr);
            return ERR;
        }
        strcpy(record->name, name);
        fread(&record->sum, sizeof(uint16_t), 1, fptr);
        fgets(record->date, DBREC_DATE_LENGTH+1, fptr);
        fgets(record->lawyer, DBREC_LAYR_LENGTH+1, fptr);
        string_trim(record->lawyer);
        ListAppend(record, dbcontents);         //ссылку на каждую структуру добавляем в список
    }
    fclose(fptr);
    return OK;
}

static wndresult_t PrintLawyers(const struct List *dbcontents){
    struct List *lawyers = calloc(1, sizeof(struct List));
    for (struct Node *n=dbcontents->head;n!=NULL;n=n->next){
        ListAppend(n->data,lawyers);            //создаём новый список ссылок на структуры
    }
    ListDistinct(lawyers, LAWYER);              //удаляем из списка записи, где имя юриста дублируется
    char *strgs[4+lawyers->length];             //массив строк для хранения имен юристов
    char *str = calloc(MAX_LINE_NEEDED, sizeof(char));
    snprintf(str,MAX_LINE_NEEDED,"\tБД содержит %"PRIu64" адвокатов:",lawyers->length);
    strgs[0] = str;                             //первой строкой запишем общую информацию, следующие - имена
    struct Node *l=lawyers->head;
    for (size_t i = 0; i < lawyers->length; l=l->next) strgs[++i] = l->data->lawyer;
    wndresult_t retcode = UIDisplayStrings(1+lawyers->length, strgs);   //выведем строки на экран и вернём выбранное пользователем действие
    ListClear(lawyers);                         //перед завершением очистим память
    free(lawyers);
    free(str);
    return retcode;
}

static wndresult_t SearchName(struct Vertex *treeroot){
    UIDestroy();                                //PDCurses при вводе не используется из-за сложностей с кириллицей
    system("cls");
    //ввод данных
    char dpsitr[DBREC_NAME_LENGTH+1];
    printf("\nВведите имя вкладчика для поиска: ");
    fgets(dpsitr, DBREC_NAME_LENGTH, stdin);
    string_trimlstchr(dpsitr);
    char lawyer[DBREC_LAYR_LENGTH+1];
    printf("\nВведите имя адвоката или\nнажмите Enter для поиска любой записи: ");
    fgets(lawyer, DBREC_LAYR_LENGTH, stdin);
    bool empty_lawr = string_trimlstchr(lawyer)==ERR;
    //поиск: создаём список для хранения указателей на структуры, удовлетворяющие условиям
    struct List *contents = calloc(1, sizeof(struct List));
    if (contents==NULL) return ERROR;
    TreeSearch(dpsitr, treeroot, contents);     //ищем в дереве
    if (!empty_lawr){                           //если задано имя юриста - дополнительный фильтр результатов по юристу
        for (struct Node *n=contents->head;n!=NULL;n=n->next){
            int cmpl = strncmp(lawyer, n->data->lawyer, strlen(lawyer));
            if (cmpl!=0) ListRemove(n, contents);
        }
    }
    ListSort(contents, DEPOSITOR);              //сортировка результатов поиска по имени вкладчика
    char *title = calloc(MAX_LINE_NEEDED, sizeof(char));
    if (contents->length==0) snprintf(title, MAX_LINE_NEEDED, "По запросу %s | %s записей не найдено!", dpsitr, lawyer);
    else snprintf(title, MAX_LINE_NEEDED, "Поиск по запросу %s | %s", dpsitr, lawyer);
    UIInit();                                   //инициализируем интерфейс и отображаем найденные записи
    wndresult_t retcode = UIDisplayDbrecords(contents, title);
    ListClear(contents);
    free(contents);
    return retcode;
}
