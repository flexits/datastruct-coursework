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
#define     WND_HEADER          "DbView"    //заголовок окна программы

typedef enum WndResult {ERROR=-1, EXIT=0, NOPE, DOSEARCH, SHOWLR, SHOWDB} wndresult_t;  //коды возврата функций, оперирующих окнами интерфейса; совместимы с ERR/OK, определёнными PDCurses
                                                                                //возвращается флаг ошибки ERROR, нормального завершения EXIT, или признак требуемого следующего действия

int UIInit(void);                                                               //инициализация интерфейса и PDCurses; возвращает OK/ERR
wndresult_t UIDisplayDbrecords(const struct List *lst, const char *dbname);     //отображение списка, содержащего записи БД, в окне с полосой прокрутки
wndresult_t UIDisplayStrings(int argc, char *argv[]);                           //отображение массива с заданным кол-вом строк (!не проверяется выход за границы окна!)
wndresult_t UISearch(char *str1, const int len1, char *str2, const int len2);   //окно ввода информации для поиска (!проблема с кириллицей!)
wndresult_t UIProgressbarDisplay(int progress, char *title);                    //вывод полосы - индикатора прогресса (задаётся значение от 0 до 100 и заголовок)
int UIProgressbarDestroy(void);                                                 ///после использования индикатор прогресса обязательно деинициализировать!
int UIDestroy(void);                                                            ///после завершения работы с PDCurses обязательно деинициализировать!
                                                                                //процедуры деинициализации всегда возвращают OK
#endif // UICURS_H_INCLUDED
