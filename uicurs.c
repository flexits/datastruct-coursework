#include <stdlib.h>
#include <strings.h>
#include <inttypes.h>
#include "curses.h"
#include "uicurs.h"
#include "datastruct.h"

#define MIN_TERMINAL_WIDTH 72       //min terminal windows width (Win7 default = 80)
#define WND_COLOR_PAIR_NUM 1        //main windows colors
#define HEAD_COLOR_PAIR_NUM 2       //header line colors
#define MENU_COLOR_PAIR_NUM 3       //menu line colors
#define PBAR_COLOR_PAIR_NUM 4       //progress bar colors

typedef struct {WINDOW *win; int X;int Y; int width;int height;} wndprops_t;//ᮧ������ ���� �ਫ������ � ��� ᢮��⢠: �࠭�� ���न���� ������ ���孥�� 㣫�, �ਭ�, ����,
                                                                            //� ������ ����� ࠧ����� ᮤ�ন��� (�᪫�祭� �࠭���, ���� � ��.)

static int UIWndCreate(wndprops_t *wprops);                                 //ᮧ���� ���� �ਫ������ � ��।��� 㪠��⥫� �� ���� � wprops; �����頥� OK/ERR
static wndresult_t process_menukeys(int key);                               //��ࠡ��稪 ����⨩ ������ ����; �����頥� ���, ᮮ⢥�����騩 ��࠭���� �㭪�� ����, ��� VOID � ���� ��砥
static void writeln_centred(WINDOW *win, int lnum, char *ln);               //�뢥�� ��ப� �� 業���
static void writeln_alignr(WINDOW *win, int lnum, int indentr, char *ln);   //�뢥�� ��ப� � ��ࠢ�������� �� �ࠢ��� ��� � ������� ����㯮� �� ���
static void fillln(WINDOW *win, int lnum);                                  //������� ��ப� �ନ���� 梥⮬ 䮭�

int UIInit(void){
    initscr();                                                              //���樠����஢��� PDCurses
    if (start_color() == ERR){                                              //���樠����஢��� 梥�
        endwin();
        return ERR;
    }
    if (COLS<MIN_TERMINAL_WIDTH) resize_term(LINES, MIN_TERMINAL_WIDTH);    //�������� ࠧ��� ���� �ନ����, �⮡ ����⨫��� ᮤ�ন���
    init_pair(WND_COLOR_PAIR_NUM, COLOR_WHITE, COLOR_BLUE);                 //���樠������ 梥⮢�� ���
    init_pair(HEAD_COLOR_PAIR_NUM, COLOR_BLACK, COLOR_GREEN);
    init_pair(MENU_COLOR_PAIR_NUM, COLOR_BLACK, COLOR_WHITE);
    init_pair(PBAR_COLOR_PAIR_NUM, COLOR_YELLOW, COLOR_WHITE);
    return OK;
}

static void writeln_centred(WINDOW *win, int lnum, char *ln){
    int start_index = (COLS - strlen(ln))/2;
    mvwaddstr(win, lnum, start_index, ln);
}

static void writeln_alignr(WINDOW *win, int lnum, int indentr, char *ln){
    int start_index = COLS - strlen(ln) - indentr;
    if (start_index < 0) return; //��ப� �� ���頥��� � ����
    mvwaddstr(win, lnum, start_index, ln);
}

static void fillln(WINDOW *win, int lnum){
    for (int i=0;i<COLS;i++){
        mvwaddch(win, lnum, i, CHR_WHTSPACE);
    }
}

static int UIWndCreate(wndprops_t *wprops){
    if (wprops == NULL) return ERR;
    if (wprops->win != NULL) delwin(wprops->win);                           //㭨�⮦��� �������饥 ����
    WINDOW *win = newwin(0, 0, 0, 0);                                       //ᮧ���� ����� ���� ࠧ��஬ = ���� �ନ����
    if (win == NULL) return ERR;

    wbkgd(win, COLOR_PAIR(WND_COLOR_PAIR_NUM));                             //�ਬ����� � ���� 梥� �� 㬮�砭��
    wattrset(win, COLOR_PAIR(HEAD_COLOR_PAIR_NUM));                         //�ਬ����� 梥� ��ப� ���������
    fillln(win, 0);                                                         //������ ��ப� 梥⮬ 䮭�
    writeln_centred(win, 0, WND_HEADER);                                    //�뢥�� ��ப�
    fillln(win, LINES-1);                                                   //�������筮 ��� ����
    writeln_centred(win, LINES-1, "F2 - �� �����, F3 - ��������, F4 - ����, ESC - �������� �ணࠬ��");
    wstandend(win);                                                         //�⪫���� ��ਡ��� - ������ � 梥⠬ �� 㬮�砭��

    mvwhline(win, 1, 1, ACS_HLINE, COLS-2);                                 //��㥬 ࠬ�� ��� ᮤ�ন���� ����
    mvwhline(win, LINES-3, 1, ACS_HLINE, COLS-2);
    mvwvline(win, 2, 0, ACS_VLINE, LINES-5);
    mvwvline(win, 2, COLS-1, ACS_VLINE, LINES-5);
    mvwaddch(win, 1, 0, ACS_ULCORNER);
    mvwaddch(win, 1, COLS-1, ACS_URCORNER);
    mvwaddch(win, LINES-3, 0, ACS_LLCORNER);
    mvwaddch(win, LINES-3, COLS-1, ACS_LRCORNER);

    wprops->win = win;                                                      //�����頥� 㪠��⥫� �� ᮧ������ ���� � ࠧ���� ����㯭�� ��� �뢮�� ���ଠ樨 ������
    wprops->X = 1;
    wprops->Y = 2;
    wprops->height = LINES - 2*wprops->Y-1;
    wprops->width = COLS - 2*wprops->X;
    return OK;
};

static wndresult_t process_menukeys(int key){
    switch(key){
        case KBD_ESC:   //close the window
            return EXIT;
        case KEY_F(2):  //F2 - �� �����
            return SHOWDB;
        case KEY_F(3):  //F3 - ��������
            return SHOWLR;
        case KEY_F(4):  //F4 - ����
            return DOSEARCH;
        default:        //������ ������ �� �⭮���� � ����
            return NOPE;
    }
}

wndresult_t UIDisplayStrings(int argc, char *argv[]){
    wndprops_t *wprops = calloc(1, sizeof(wndprops_t));
    if (wprops == NULL) return ERROR;
    WINDOW *win = NULL;
    if (UIWndCreate(wprops) == ERR || (win = wprops->win) == NULL){
        free(wprops);
        return ERROR;
    }
    const int LINE_INDENT_H = 8;
    const int LINE_INDENT_V = 4;
    for (int i=0;i<argc;i++){
        mvwaddstr(win, LINE_INDENT_V+i+wprops->Y, LINE_INDENT_H+wprops->X, argv[i]);
        //TODO trim lines to fit the window
        //TODO if i>height show only first lines
        //TODO indents as args
    }
    wrefresh(win);
    noecho();
    keypad(win, TRUE);
    meta(win, TRUE);
    bool finish = FALSE;
    int retcode = EXIT;
    for(;!finish;){
        retcode = process_menukeys(wgetch(win));
        if (retcode!=NOPE) finish = TRUE;
    }
    if (finish){
        delwin(win);
        free(wprops);
        return retcode;
    } else return ERROR;
}

wndresult_t UIDisplayDbrecords(const struct List *lst, const char *dbname){
    if (lst == NULL) return ERROR;
    wndprops_t *wprops = calloc(1, sizeof(wndprops_t));
    if (wprops == NULL) return ERROR;
    WINDOW *win = NULL;
    size_t scr_counter = 0;                                     //⥪��� ������ �ப��⪨
    char str[MAX_LINE_NEEDED];
    int key;
    bool finish = FALSE;
    int retcode = EXIT;
    for(;!finish;){
        if (UIWndCreate(wprops) == ERR || (win = wprops->win) == NULL){
            free(wprops);
            return ERROR;
        }
                                                                //���ᮢ��� ������ �ப��⪨
        const int scroller_top = wprops->Y+1;
        const int scroller_btm = wprops->Y+wprops->height-2;
        const int scroller_col = wprops->X+wprops->width;
        for (int i=scroller_top;i<=scroller_btm;i++){
            mvwaddch(win, i, scroller_col, CHR_LIGHT_SHADE);
        }
        mvwaddch(win, scroller_top-1, scroller_col, ACS_UARROW);
        mvwaddch(win, scroller_btm+1, scroller_col, ACS_DARROW);
                                                                //���ᮢ��� ����㭮� �� ����� �ப��⪨
        int scroll_position = scroller_top;
        if (lst->length > 0) scroll_position += scr_counter * (wprops->height-2) / lst->length;
        mvwaddch(win, scroll_position, scroller_col, CHR_FULL_BLOCK);

        size_t i=0, line = wprops->Y;                          //�뢥�� ������ ᯨ᪠
        for (struct Node *n=lst->head;n!=NULL;n=n->next,i++){
            if (i<scr_counter) continue;
            if (i>=scr_counter+wprops->height) break;
            memset(str, '\0', MAX_LINE_NEEDED);
            snprintf(str,MAX_LINE_NEEDED,"%-*s  %5u  %s  %s", DBREC_NAME_LENGTH, n->data->name, n->data->sum, n->data->date, n->data->lawyer);
            mvwaddstr(win, line++, wprops->X, str);
        }
                                                                //�뢥�� ����稪� ��ப ����� �ࠢ�
        memset(str, '\0', MAX_LINE_NEEDED);
        snprintf(str,MAX_LINE_NEEDED," %"PRIu64"-%"PRIu64" / %"PRIu64"", scr_counter+1, i, lst->length);
        writeln_alignr(win, wprops->Y+wprops->height, 2, str);
        if (dbname != NULL) mvwaddstr(win, wprops->Y+wprops->height+1, wprops->X, dbname);

        wrefresh(win);
        curs_set(CURSOR_INVISIBLE);                             //������ �����
        noecho();
        keypad(win, TRUE);
        meta(win, TRUE);
        int fast_step = wprops->height;                         //�� �⠭���⭮� �஫�� �� ���� ࠧ ᤢ������� ���� ��ப�, �� ����஬ fast_step ��ப
        key = wgetch(win);
        switch(key){
        case KEY_UP:    //normal scroll up
            if (scr_counter > 0) scr_counter--;
            break;
        case CTL_UP:    //fast scroll up
        case KEY_PPAGE:
            if (scr_counter >= fast_step) scr_counter -= fast_step;
            break;
        case KEY_DOWN:  //normal scroll down
            if(scr_counter+1 < lst->length) scr_counter++;
            break;
        case CTL_DOWN:  //fast scroll down
        case KEY_NPAGE:
            if(scr_counter + fast_step < lst->length) scr_counter += fast_step;
            break;
        case CTL_HOME:  //scroll up to the start
            scr_counter = 0;
            break;
        case CTL_END:   //scroll down to the end
            if (lst->length > fast_step) scr_counter = lst->length - fast_step;
            break;
        default:        //process menu
            retcode = process_menukeys(key);
            if (retcode!=NOPE) finish = TRUE;
            break;
        }
    }
    if (finish){
        delwin(win);
        free(wprops);
        return retcode;
    } else return ERROR;
}

wndresult_t UISearch(char *str1, const int len1, char *str2, const int len2){
    if(str1 == NULL || str2 == NULL) return ERROR;
    memset(str1, '\0', len1+1);
    memset(str2, '\0', len2+1);
    wndprops_t *wprops = calloc(1, sizeof(wndprops_t));
    if (wprops == NULL) return ERROR;
    WINDOW *win = NULL;
    const int LINE_INDENT_H = 8;
    const int LINE_INDENT_V = 4;
    const char *inp_title1 = "������ ��� �����稪�";
    const char *inp_title2 = "������ ��� �������";
    const int inp_X = LINE_INDENT_H+wprops->X+24;
    const int inp1_Y = LINE_INDENT_V+wprops->Y+2;
    const int inp2_Y = LINE_INDENT_V+wprops->Y+4;
    int key;
    bool finish = FALSE;
    int retcode = EXIT;
    for(;!finish;){
        if (UIWndCreate(wprops) == ERR || (win = wprops->win) == NULL){
            free(wprops);
            return ERROR;
        }
        curs_set(CURSOR_REGULAR);
        mvwaddstr(win, inp1_Y-2, LINE_INDENT_H+wprops->X, "\t���� � ��:");
        mvwaddstr(win, inp1_Y, LINE_INDENT_H+wprops->X, inp_title1);
        mvwaddstr(win, inp2_Y, LINE_INDENT_H+wprops->X, inp_title2);
        mvwaddstr(win, inp2_Y+2, LINE_INDENT_H+wprops->X, "\tEnter ��� ���⢥ত����");
        wrefresh(win);

        flushinp();
        keypad(win, TRUE);
        meta(win, TRUE);
        echo();
        mvwgetnstr(win, inp1_Y, inp_X, str1, len1);
        mvwgetnstr(win, inp2_Y, inp_X, str2, len2);
        retcode = EXIT;
        finish = TRUE;
        break;
        noecho();
        for (;!finish;){
            key = wgetch(win);
            switch(key){
            case KEY_UP:
                if (getcury(win) == inp2_Y) wmove(win, inp1_Y, inp_X);
                break;
            case KEY_DOWN:
                if (getcury(win) == inp1_Y) wmove(win, inp2_Y, inp_X);
                break;
            default:        //process menu
                retcode = process_menukeys(key);
                if (retcode!=NOPE) finish = TRUE;
            break;
            }
        }
    }
    if (finish){
        delwin(win);
        free(wprops);
        return retcode;
    } else return ERROR;
};

static wndprops_t *wnd_progressbar;

wndresult_t UIProgressbarDisplay(int progress, char *title){
    if (wnd_progressbar == NULL){
        wnd_progressbar = calloc(1, sizeof(wndprops_t));
    }
    if (wnd_progressbar == NULL) return ERROR;
    if (UIWndCreate(wnd_progressbar) == ERR || (wnd_progressbar->win) == NULL){
        free(wnd_progressbar);
        return ERROR;
    }
    if (title != NULL) writeln_centred(wnd_progressbar->win, wnd_progressbar->Y+4, title);
    if (progress < 0) progress = 0;
    else if (progress > 100) progress = 100;
    char percentage[5];
    snprintf(percentage,5,"%u%%", progress);
    writeln_centred(wnd_progressbar->win, wnd_progressbar->Y+5, percentage);
    curs_set(CURSOR_INVISIBLE);
    int pbar_length = progress * (wnd_progressbar->width - 1) / 100;
    int pbar_start = wnd_progressbar->X + 1;
    //int pbar_end = wnd_progressbar->X + wnd_progressbar->width -1;
    int pbar_position = wnd_progressbar->Y+10; //TODO align center
    wattrset(wnd_progressbar->win, COLOR_PAIR(PBAR_COLOR_PAIR_NUM)|WA_BOLD);
    for (int i=pbar_start;i<=pbar_length;i++){
        mvwaddch(wnd_progressbar->win, pbar_position, i, CHR_FULL_BLOCK);
    }
    wstandend(wnd_progressbar->win);
    wrefresh(wnd_progressbar->win);
    flushinp();
    return EXIT;
};

int UIProgressbarDestroy(void){
    if (wnd_progressbar != NULL){
        if (wnd_progressbar->win != NULL) delwin(wnd_progressbar->win);
        free(wnd_progressbar);
    }
    return OK;
};

int UIDestroy(void){
    refresh();
    return endwin(); //always OK
}
