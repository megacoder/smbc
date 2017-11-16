/*
       fnet.h
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.org
    home page: http://smbc.airm.net
*/
// -------------------------------------------------------------------
#define Rfree(ptr)    { free(ptr); ptr = NULL; } 
#define RstrdupN(a, b) { free(a); a = NULL; a = Rstrdup(b); }
#define Rdelwin(ptr)  { delwin(ptr); ptr = NULL; } 
#define RdelwinE(ptr)  { if (ptr) werase(ptr); delwin(ptr); ptr = NULL; } 
#include <curses.h>
#include <time.h>

#ifdef HAVE_STRING_H
#include <string.h>
#else  /* not HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#endif /* not HAVE_STRING_H */

int readline(register int fd, register char *ptr, register int maxlen);
int writen(register int fd, register char *ptr, register int nbytes);
void str_echo(int sockfd);
int readlineii( int sockfd, char *buf, int size, int *linepos );
int readlineie(WINDOW *win, char *buf, int size, int *linepos );
void refreshw(WINDOW *win, char *buf);
void reverse(char *str);
void itoaMalloc(char **str, int strLen, int n);
void itoa(char *str, int strLen, int n);
void refreshTxtInWin(WINDOW *win, int y, int x, char *buf, int winSize, int startidx, int colorPair, int attr, int xDelta);
int readlineWait(WINDOW *win, int wy, int wx, char *buf, int sizebuf, int winSize, int colorPair);
int ssqr(int Rv, int Rpot);
int strhextoi(char *str);
void RmvwprintwCenter(WINDOW *win, int i, char *str);
int readlineOneCharNoDelay(WINDOW *win, int wy, int wx, char *buf, int sizebuf, int winSize, int colorPair, int *ctrlp, int Rnodelay, int *startidx, int *wxx, int *first);
int readlineWaitForOneChar(WINDOW *win, int wy, int wx, char *buf, int sizebuf, int winSize, int colorPair, int *ctrlp, int *startidx, int *wxx);
void RstrToUpper(char *str);
void RstrToUpperCopy(char *Dstr, char *Sstr);
void RgetFileFromTwoDirs(char **RfileName, char *RdirBig, char *RdirSmall);
void *Rmalloc(size_t size);
char *Rstrcpy(char *dest, int destLen, const char *src, int srcLen);

/*
   Ignore all chars % in fmt variable
*/
int Rasprintf(char **strp, const char *fmt);

/*
   add string, char or integer fmt to the end of strp
*/
int Rstraddtoend(char **strp, const char *fmt);
int Rcharaddtoend(char **strp, const char fmt);
int Rintaddtoend(char **strp, int fmt);

/*
   Rmvwprintw ignore all chars % in fmt string.
*/
int Rmvwprintw(WINDOW *win, int y, int x, const char *fmt);

int RtruncateStr(char **Rstr, int Rpos);
char *Rstrdup(char *Rstr);
int RstrCmp(char *src, char *dst);

ssize_t RwriteUnsignedInt(int fd, unsigned int buf);
ssize_t RwriteStr(int fd, const char *buf);
ssize_t RwriteChar(int fd, const char buf);
ssize_t RwriteTime_t(int fd, time_t buf);

size_t RreadStr(int fd, char **buf);
int RreadUnsignedInt(int fd, unsigned int *buf);
int RreadChar(int fd, char *buf);
time_t RreadTime_t(int fd, time_t *buf);

size_t Rstrlen(const char *s);
void RchdirUp(char **Rstr);

/*
   Function return point to dir
   /abc/def/ -> abc/def/
   /abc/def  -> abc/def/
    abc/def  -> abc/def/
*/
void RparsingDir(char **Rstr);

int RmakeCopyFileFromPIDtoPID(int Rdst, int Rsrc);
