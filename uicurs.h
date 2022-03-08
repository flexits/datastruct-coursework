#ifndef UICURS_H_INCLUDED
#define UICURS_H_INCLUDED

#include "curses.h"
#include "datastruct.h"

//     CP-866 characters
#define     CHR_BOX_SINGLE_VR   195
#define     CHR_BOX_SINGLE_VL   180
#define     CHR_WHTSPACE        32
#define     CHR_SQUARE_BLK      254
#define     CHR_LIGHT_SHADE     176
#define     CHR_FULL_BLOCK      219
//     keycodes
#define     KBD_ESC             27
#define     KBD_ENTER           13
#define     KBD_SPACE           32
//     UI elements
#define     CURSOR_INVISIBLE    0
#define     CURSOR_REGULAR      1
#define     WND_HEADER          "DbView"    //��������� ���� �ணࠬ��

typedef enum WndResult {ERROR=-1, EXIT=0, NOPE, DOSEARCH, SHOWLR, SHOWDB} wndresult_t;  //���� ������ �㭪権, ��������� ������ ����䥩�; ᮢ���⨬� � ERR/OK, ��।���묨 PDCurses
                                                                                //�����頥��� 䫠� �訡�� ERROR, ��ଠ�쭮�� �����襭�� EXIT, ��� �ਧ��� �ॡ㥬��� ᫥���饣� ����⢨�

int UIInit(void);                                                               //���樠������ ����䥩� � PDCurses; �����頥� OK/ERR
wndresult_t UIDisplayDbrecords(const struct List *lst, const char *dbname);     //�⮡ࠦ���� ᯨ᪠, ᮤ�ঠ饣� ����� ��, � ���� � ����ᮩ �ப��⪨
wndresult_t UIDisplayStrings(int argc, char *argv[]);                           //�⮡ࠦ���� ���ᨢ� � ������� ���-��� ��ப (!�� �஢������ ��室 �� �࠭��� ����!)
wndresult_t UISearch(char *str1, const int len1, char *str2, const int len2);   //���� ����� ���ଠ樨 ��� ���᪠ (!�஡���� � ��ਫ��楩!)
wndresult_t UIProgressbarDisplay(int progress, char *title);                    //�뢮� ������ - �������� �ண��� (�������� ���祭�� �� 0 �� 100 � ���������)
int UIProgressbarDestroy(void);                                                 ///��᫥ �ᯮ�짮����� �������� �ண��� ��易⥫쭮 �����樠����஢���!
int UIDestroy(void);                                                            ///��᫥ �����襭�� ࠡ��� � PDCurses ��易⥫쭮 �����樠����஢���!
                                                                                //��楤��� �����樠����樨 �ᥣ�� �������� OK
#endif // UICURS_H_INCLUDED
