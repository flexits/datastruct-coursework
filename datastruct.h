#ifndef DATASTRUCT_H_INCLUDED
#define DATASTRUCT_H_INCLUDED

#define DBFILENAME "BASE3.DAT"  //��� 䠩�� ��
#define DBREC_NAME_LENGTH   32  //ࠧ���� ⥪�⮢�� �����
#define DBREC_DATE_LENGTH   8
#define DBREC_LAYR_LENGTH   22
#define MAX_LINE_NEEDED (DBREC_DATE_LENGTH+DBREC_LAYR_LENGTH+DBREC_NAME_LENGTH+8+1) //���ᨬ��쭠� ����� ��ப�, ᮤ�ঠ饩 ���祭�� ��� �����, ࠧ������ �஡�����, � \0 � ����
/*
 *  �������� ������
 */
struct DbRecord {char name[DBREC_NAME_LENGTH+1]; int sum; char date[DBREC_DATE_LENGTH+1]; char lawyer[DBREC_LAYR_LENGTH+1];};   //������ �� ��, ࠧ��頥��� � �����
struct Vertex {struct DbRecord *data; struct Vertex *left; struct Vertex *right; int bal;};                                     //���設� ��ॢ� (�࠭�� 㪠��⥫� �� ������ �� ��)
struct Node {struct DbRecord *data; struct Node *previous; struct Node *next;};                                                 //㧥� ᯨ᪠ (�࠭�� 㪠��⥫� �� ������ �� ��)
struct List {struct Node *head; struct Node *tail; size_t length;};                                                             //ᯨ᮪: 㪠��⥫� �� �������� � ������ 㧫�, ��饥 ���-�� 㧫��
/*
 *  �㭪樨 ࠡ��� � ����묨
 */
struct Vertex* TreeInsert(struct DbRecord *data, struct Vertex *root);      //�������� ������ �� �� � ���-��ॢ�, �����頥� ���設� ��ॢ� (���� - ���� name �����)
void TreeDestruct(struct Vertex *root);                                     //㤠��� �� ���設� ��ॢ�, ��᢮������ ������� ������
void TreeSearch(const char* name, const char* lawyer, struct Vertex *root, struct List *result); //���� ����ᥩ � ��ॢ�, ��� ������ ���� name ��稭����� � name, ���� lawyer ᮢ������ � lawyer, � ���������� ��뫮� �� ��������� � ᯨ᮪
void ListAppend(struct DbRecord *data, struct List *lst);                   //���������� 㧫� � ����� ᯨ᪠
void ListRemove(struct Node *n, struct List *lst);                          //㤠����� 㧫� �� ᯨ᪠
void ListClear(struct List *lst);                                           //㤠����� ��� 㧫�� ᯨ᪠
enum keyfield {DEPOSITOR, LAWYER};                                          //�롮� ���� ���஢�� ᯨ᪠ - ��� �����稪� ��� ��� �������
void ListSort(struct List *lst, enum keyfield srt);                         //���஢�� ᯨ᪠ ��⮤�� ��אַ�� ᫨ﭨ� �� ��������� ����
void ListDistinct(struct List *lst, enum keyfield srt);                     //㤠����� �㡫�������� ���祩 �� ᯨ᪠ (ᯨ᮪ �㤥� �����஢��)

#endif // DATASTRUCT_H_INCLUDED
