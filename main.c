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
 *  ���ଠ�� �� �� ����㦠���� � ������ �������� ��᫥����⥫�� ���뢠���� ����ᥩ; ��� ������ ������ �뤥����� ����࠭�⢮ � ��� (������� struct DbRecord).
 *  ����� � ����㦥���� ���ଠ樨 �����⢫���� ���।�⢮� ᯨ᪠ 㪠��⥫�� �� �������� � �����.
 *  �������� � ���஢�� � ��稥 ����⢨� �ந᧮����� ⮫쪮 ��� 㪠��⥫ﬨ; � ����㦥��� �����, � �� ࠧ��饭�� � ��� ������� ���������.
 *  ��᪮��� �ॡ���� �����⢫��� ����� �� ࠧ�� ���砬 (�����稪 � �������), �ᯮ��㥬 ��� ࠧ��� ᯨ᪠ 㪠��⥫��.
 */

/*
 *  �ᯮ����⥫�� �㭪樨
 */
static int string_trim(char *str);          //㤠���� �஡��� � ���� ��ப�; �����頥� ERR, �᫨ ����砫쭠� ��ப� �����
static int string_trimlstchr(char *str);    //㤠���� ���� ᨬ��� � ���� ��ப�; �����頥� ERR, �᫨ ����砫쭠� ��ப� �����
static int ReadDbFile(const char* fname, struct List *dbcontents);   //����� ᮤ�ন��� �� �� 䠩�� � ᯨ᮪; �����頥� OK/ERR
static wndresult_t PrintLawyers(const struct List *dbcontents);      //�뢮� ��� ������⮢ �� ᯨ᪠, �࠭�饣� ᮤ�ন��� ��
static wndresult_t SearchName(struct Vertex *treeroot);              //�믮����� ���� ����� � ��ॢ� �� �����

int main()
{
    //����㧨�� ᮤ�ন��� 䠩�� � ���
    struct List *dbcontents = calloc(1, sizeof(struct List));
    if (ReadDbFile(DBFILENAME, dbcontents)==ERR){
        UIDestroy();
        return ERR;
    }

    //���樠������ ���짮��⥫�᪮�� ����䥩�
    if (UIInit() == ERR) return ERR;

    //����ந�� �� ����㦥���� ������ ��ॢ� ���᪠, �⮡ࠦ�� �������� �ண���
    struct Vertex *treeroot = NULL;
    size_t counter = 0;
    for (struct Node *n=dbcontents->head; n!=NULL; n=n->next, counter++){
        UIProgressbarDisplay(counter*100/dbcontents->length, "����஥��� ��� ��ॢ�");
        treeroot = TreeInsert(n->data,treeroot);
    }
    UIProgressbarDestroy();

    //�⮡ࠦ���� ���� ���짮��⥫�᪮�� ����䥩� � 横�� �� �����襭�� �ணࠬ��
    wndresult_t retcode = UIDisplayDbrecords(dbcontents, DBFILENAME);
    bool finish = FALSE;
    for(;!finish;){
        switch(retcode){
        case ERROR:
        case EXIT:
            finish = TRUE;
            break;
        case DOSEARCH:      //�������� ���� �����
            retcode = SearchName(treeroot);
            break;
        case SHOWDB:        //�뢥�� ᮤ�ন��� ��
            retcode = UIDisplayDbrecords(dbcontents, DBFILENAME);
            break;
        case SHOWLR:        //�뢥�� ᯨ᮪ ������⮢
            retcode = PrintLawyers(dbcontents);
            break;
        default:
            break;
        }
    }
    //�� �����襭�� �ணࠬ�� �����樠�����㥬 ����䥩� � �᢮������� ������
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
    if (len==0) return ERR;   //����� ��ப�
    for (--len;len>0;len--){                    //�ய��⨢ �������騩 '\0', �������� �ࠢ� ������,
        if (isspace(str[len])) str[len] = '\0'; //�� "�����" ᨬ���� �������� �� '\0',
        else break;                             //�� ��ࢮ�� �����⮣�
    }                                           //�᫨ � 横�� ��諨 �� ��砫� ��ப� � ᨬ��� � ��砫� ���⮩ - ��ப� �����
    if (len==0 && (str[len]=='\0' || isspace(str[len]))) return ERR;
    else return OK;
}

static int string_trimlstchr(char *str){
    if (str == NULL) return ERR;
    size_t len=strlen(str);
    if (len==0) return ERR;   //����� ��ப�
    str[len-1] = '\0';
    return OK;
}

static int ReadDbFile(const char* fname, struct List *dbcontents){
    FILE *fptr = fopen(fname, "r");
    if (fptr == NULL) return ERR;
    ListClear(dbcontents);
    char name[DBREC_NAME_LENGTH+1];
    while(!feof(fptr)){                         //�����筮 ���뢠�� 䠩�, �ய�᪠� ��ப�, ��� ��砫� (�.�. ��� �����稪�) - ���⮥
        memset(name, '\0', DBREC_NAME_LENGTH+1);
        fgets(name, DBREC_NAME_LENGTH+1, fptr);
        if (string_trim(name)==ERR) continue;
        struct DbRecord *record = calloc(1, sizeof(struct DbRecord));
        if (record == NULL){                    //��� ������ ��ப� (�.�. �����) �뤥�塞 � ����� ����� �������� � �����뢠�� � ��� �����
            fclose(fptr);
            return ERR;
        }
        strcpy(record->name, name);
        fread(&record->sum, sizeof(uint16_t), 1, fptr);
        fgets(record->date, DBREC_DATE_LENGTH+1, fptr);
        fgets(record->lawyer, DBREC_LAYR_LENGTH+1, fptr);
        string_trim(record->lawyer);
        ListAppend(record, dbcontents);         //��뫪� �� ������ �������� ������塞 � ᯨ᮪
    }
    fclose(fptr);
    return OK;
}

static wndresult_t PrintLawyers(const struct List *dbcontents){
    struct List *lawyers = calloc(1, sizeof(struct List));
    for (struct Node *n=dbcontents->head;n!=NULL;n=n->next){
        ListAppend(n->data,lawyers);            //ᮧ��� ���� ᯨ᮪ ��뫮� �� ��������
    }
    ListDistinct(lawyers, LAWYER);              //㤠�塞 �� ᯨ᪠ �����, ��� ��� ���� �㡫������
    char *strgs[4+lawyers->length];             //���ᨢ ��ப ��� �࠭���� ���� ���⮢
    char *str = calloc(MAX_LINE_NEEDED, sizeof(char));
    snprintf(str,MAX_LINE_NEEDED,"\t�� ᮤ�ন� %"PRIu64" ������⮢:",lawyers->length);
    strgs[0] = str;                             //��ࢮ� ��ப�� ����襬 ����� ���ଠ��, ᫥���騥 - �����
    struct Node *l=lawyers->head;
    for (size_t i = 0; i < lawyers->length; l=l->next) strgs[++i] = l->data->lawyer;
    wndresult_t retcode = UIDisplayStrings(1+lawyers->length, strgs);   //�뢥��� ��ப� �� �࠭ � ���� ��࠭��� ���짮��⥫�� ����⢨�
    ListClear(lawyers);                         //��। �����襭��� ���⨬ ������
    free(lawyers);
    free(str);
    return retcode;
}

static wndresult_t SearchName(struct Vertex *treeroot){
    UIDestroy();                                //PDCurses �� ����� �� �ᯮ������ ��-�� ᫮����⥩ � ��ਫ��楩
    system("cls");
    //���� ������
    char dpsitr[DBREC_NAME_LENGTH+1];
    printf("\n������ ��� �����稪� ��� ���᪠: ");
    fgets(dpsitr, DBREC_NAME_LENGTH, stdin);
    string_trimlstchr(dpsitr);
    char lawyer[DBREC_LAYR_LENGTH+1];
    printf("\n������ ��� ������� ���\n������ Enter ��� ���᪠ �� �����: ");
    fgets(lawyer, DBREC_LAYR_LENGTH, stdin);
    string_trimlstchr(lawyer);
    //����: ᮧ��� ᯨ᮪ ��� �࠭���� 㪠��⥫�� �� ��������, 㤮���⢮���騥 �᫮���
    struct List *contents = calloc(1, sizeof(struct List));
    if (contents==NULL) return ERROR;
    TreeSearch(dpsitr, lawyer, treeroot, contents);     //�饬 � ��ॢ�
    char *title = calloc(MAX_LINE_NEEDED, sizeof(char));
    if (contents->length==0) snprintf(title, MAX_LINE_NEEDED, "�� ������ %s | %s ����ᥩ �� �������!", dpsitr, lawyer);
    else snprintf(title, MAX_LINE_NEEDED, "���� �� ������ %s | %s", dpsitr, lawyer);
    UIInit();                                   //���樠�����㥬 ����䥩� � �⮡ࠦ��� �������� �����
    wndresult_t retcode = UIDisplayDbrecords(contents, title);
    ListClear(contents);
    free(contents);
    return retcode;
}
