/*
       fnet.c
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.org
    home page: http://smbc.airm.net
*/
// -------------------------------------------------------------------
#define _GNU_SOURCE

#include "../config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else  /* not HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#endif /* not HAVE_STRING_H */

#include <libsmbclient.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include "fnet.h"
#include "ierror.h"
#include "debug.h"

#define MAXLINE 512
//static int smbc_compat_initialized = 0;

#ifdef SAMBA3XX
static SMBCCTX *statcont = NULL;
#endif /* SAMBA3XX */

void 
reverse(char *str)
{
  int c, i, j;
  
  for (i=0, j=strlen(str)-1; i<j; i++, j--) 
  {
    c = str[i];
    str[i] = str[j];
    str[j] = c;
  }
}

void 
itoa(char *str, int strLen, int n)
{
  int sign;
  char *RstrT = NULL;
  char *RstrTT = NULL;
  
  void itoaAddChar(char Rch)
  {
    asprintf(&RstrTT, "%s%c", RstrT?RstrT:"", Rch);
    Rfree(RstrT);
    RstrT = RstrTT;
  }

  if ((sign = n) < 0)
     n=-n;
  do 
  {
    itoaAddChar(n % 10 + '0');
  } while (( n /= 10 ) > 0);
  if (sign < 0)
    itoaAddChar('-');
  reverse(RstrT);
  sign = strlen(RstrT);
  if (sign+1-strLen > 0)
    sign = strLen;
  memcpy(str, RstrT, sign+1);
  str[sign+1] = 0;
}  

void
itoaMalloc(char **str, int strLen, int n)
{
  Rfree(*str);
  *str = Rmalloc(strLen);
  itoa(*str, strLen, n);
}

int 
readline(register int fd, register char *ptr, register int maxlen)
{
  int n, rc;
  char c;
  
  for (n = 1; n < maxlen; n++) 
  {
    if ( (rc = read(fd, &c, 1)) == 1) 
    {
      *ptr++ = c;
      if (c == '\n')
	break;
    } 
    else 
      if (rc == 0) 
      {
        if (n == 1)
          return(0);
        else
	  break;
      } 
      else
        return(-1);
  }

  *ptr = 0;
  return(n);
}

int 
writen(register int fd, register char *ptr, register int nbytes)
{
  int nleft, nwritten;

  nleft = nbytes;
  while (nleft > 0) 
  {
    nwritten = write(fd, ptr, nleft);
    if (nwritten <= 0)
      return(nwritten);
    
    nleft -= nwritten;
    ptr += nwritten;
  }
  return(nbytes - nleft);
}

void 
str_echo(int sockfd)
{
  int n;
  char line[MAXLINE];

  for (;;) 
  {
    n = readline(sockfd, line, MAXLINE);
    if (n == 0)
      return;
    else 
      if (n < 0)
        err_dump("str_echo: readline error");

    if (writen(sockfd, line, n) != n)
      err_dump("str_echo: writen error");
  }
}


int 
readlineii(int sockfd, char *buf, int size, int *linepos) 
{
  int n;
  char c;
    
  while ((n = read(sockfd, &c, 1)) == 1)
  {
    buf[(*linepos)++] = c;

    if ( c == '\n')
    {
      buf[(*linepos)] = 0;
      *linepos = 0;
      return 1;
    }
  }
  if (!n)
    return -1;
  return(0);
}    

void 
refreshw(WINDOW *win, char *buf)
{
  werase(win);
  mvwprintw(win, 1, 1, buf);
}

int 
readlineie(WINDOW *win, char *buf, int size, int *linepos ) 
{
  int klawisz;

  nodelay(win, 1);
  klawisz = mvwgetch(win, 1, (*linepos)+1);

  if ((klawisz >='A' && klawisz <= 'Z') ||
      (klawisz >='a' && klawisz <= 'z') ||
      (klawisz >='0' && klawisz <= '9') ||
       klawisz == 234 || klawisz == 202 ||   // ê,Ê
       klawisz == 243 || klawisz == 211 ||   // ó,Ó
       klawisz == 177 || klawisz == 161 ||   // ±,¡
       klawisz == 182 || klawisz == 166 ||   // ¶,¦
       klawisz == 179 || klawisz == 163 ||   // ³,£
       klawisz == 191 || klawisz == 175 ||   // ¿,¯
       klawisz == 188 || klawisz == 172 ||   // ¼,¬
       klawisz == 230 || klawisz == 198 ||   // æ,Æ
       klawisz == 241 || klawisz == 209 ||   // ñ,Ñ
      (klawisz == ' ' || klawisz == '/' || klawisz == '\\' || klawisz == '#')) 
  {
    buf[(*linepos)++] = (char)klawisz;
    buf[*linepos] = '\0';
    refreshw(win, buf);
    return (0);
  }
  switch(klawisz) 
  {
    case 127: if (*linepos) 
              {
                buf[--(*linepos)] = '\0';
                refreshw(win, buf);
              }
              break;
    case 10: if (*linepos) 
             {
               buf[(*linepos)++] = '\n';
               buf[*linepos] = '\0';
               *linepos = 0;
               werase(win);
               return(1);
             }
             else
               return(0);
  }
  return(0);
}    

int 
readlineWait(WINDOW *win, int wy, int wx, char *buf, int sizebuf, int winSize, int colorPair) 
{
  int klawisz, tmp;
  int startidx = 0;
  int esc, Ri;
  int wxx = (strlen(buf) > winSize?winSize:strlen(buf));

  curs_set(1);
  keypad(win, TRUE);
  refreshTxtInWin(win,wy,wx,buf, winSize, startidx, colorPair, A_NORMAL, 0);
  while ((klawisz = mvwgetch(win, wy, wx+wxx-startidx)) != 0) 
  {
    esc = 1;
    switch(klawisz) 
    {
      case 263: 
        if (strlen(buf) && wxx) 
        {
          if (strlen(buf) == wxx) 
             buf[strlen(buf)-1] = '\0';
          else 
          {
            tmp = strlen(buf);
            for (Ri = 0; Ri < tmp-wxx; Ri++)
              buf[wxx+Ri] = buf[wxx+Ri+1]; 
          }
          if (startidx) 
          { 
            startidx--;
            wxx--;
          } 
          else 
            wxx--;
          refreshTxtInWin(win,wy,wx,buf, winSize, startidx, colorPair, A_NORMAL, 0);
        }
        esc = 0;
        break;
      case KEY_HOME:
        startidx = 0;
        wxx = 0;
        esc = 0;
        refreshTxtInWin(win,wy,wx,buf, winSize, startidx, colorPair, A_NORMAL, 0);
        break;
      case KEY_END:
        startidx = (strlen(buf) < winSize?0:strlen(buf)-winSize+1);
        wxx = strlen(buf);
        esc = 0;
        refreshTxtInWin(win,wy,wx,buf, winSize, startidx, colorPair, A_NORMAL, 0);
        break;
      case KEY_LEFT:
        if ((startidx) && (startidx == wxx)) 
        {
          startidx--; 
          wxx--;
        }
        else 
          if (wxx) 
            wxx--;
        refreshTxtInWin(win,wy,wx,buf, winSize, startidx, colorPair, A_NORMAL, 0);
        esc = 0;
        break;
      case KEY_RIGHT:
        if (wxx < strlen(buf)) 
        {
          if ((startidx+winSize <= strlen(buf)) && (startidx+winSize == wxx+1)) 
          {
            startidx++; 
            if ( wxx+1 < strlen(buf)) 
              wxx++;
          }
          else 
            wxx++;
          refreshTxtInWin(win,wy,wx,buf, winSize, startidx, colorPair, A_NORMAL, 0);
          esc = 0;
        }
        break;
      case 10: 
      case 9: 
        curs_set(0);
        return(1);
    }
    RDBG("fnet.c readlineWait: klawisz = "); RDBGc(klawisz); RDBG(", esc = "); RDBGi(esc); RDBG(","); RDBGi(klawisz); RDBG("\n");
    if ((esc) && ((klawisz >='A' && klawisz <= 'Z') ||
        (klawisz >='a' && klawisz <= 'z') ||
        (klawisz >='0' && klawisz <= '9') ||
        (klawisz == ' ' || klawisz == '/' || klawisz == '-' ||
         klawisz == '\\' || klawisz == '#' || 
         klawisz == 234 || klawisz == 202 ||   // ê,Ê
         klawisz == 243 || klawisz == 211 ||   // ó,Ó
         klawisz == 177 || klawisz == 161 ||   // ±,¡
         klawisz == 182 || klawisz == 166 ||   // ¶,¦
         klawisz == 179 || klawisz == 163 ||   // ³,£
         klawisz == 191 || klawisz == 175 ||   // ¿,¯
         klawisz == 188 || klawisz == 172 ||   // ¼,¬
         klawisz == 230 || klawisz == 198 ||   // æ,Æ
         klawisz == 241 || klawisz == 209 ||   // ñ,Ñ
         klawisz == '.' || klawisz == ',')) && 
        (strlen(buf)+1 < sizebuf)) 
    {
        RDBG("fnet.c readlineWait: ok1\n");
      if (wxx == strlen(buf)) 
      {
        buf[wxx] = (char)klawisz;
        buf[wxx+1] = 0;
      }
      else 
      {
        tmp = strlen(buf);
        for (Ri = 0; Ri < tmp-wxx+1; Ri++)
          buf[tmp+1-Ri] = buf[tmp-Ri];
        buf[wxx] = (char)klawisz;
      }
      wxx++;
      if (startidx+winSize == wxx) startidx++;
      refreshTxtInWin(win,wy,wx,buf, winSize, startidx, colorPair, A_NORMAL, 0);
    }
  }
  return(0);
}    

int 
ssqr(int Rv, int Rpot)
{
  int Ri;
  int Rr = 1;
  for (Ri = 0; Ri < Rpot; Ri++) 
    Rr = Rv*Rr;
  return(Rr);
}

int 
strhextoi(char *str)
{
  int Ri;
  int Rm = strlen(str);
  int Rr = 0;
  char Rc;
  int Rci;

  for (Ri = 0; Ri < Rm; Ri++) 
  {
    Rc = str[strlen(str)-Ri-1];
    switch (Rc) {
      case 'A' :
      case 'a' : Rci = 10; break;
      case 'B' :
      case 'b' : Rci = 11; break;
      case 'C' :
      case 'c' : Rci = 12; break;
      case 'D' : 
      case 'd' : Rci = 13; break;
      case 'E' :
      case 'e' : Rci = 14; break;
      case 'F' : 
      case 'f' : Rci = 15; break;
      default : Rci = Rc-48;
                if ((Rci < 0) | (Rci > 9)) 
                  return (-1);
    }
    Rr+= Rci*ssqr(16,Ri);
  }
  return(Rr);
}

void 
RmvwprintwCenter(WINDOW *win, int i, char *str)
{
  int y, x;

  getmaxyx(win, y, x);
  mvwprintw(win, i, x/2-strlen(str)/2, str);
  wrefresh(win);
}

void 
refreshTxtInWin(WINDOW *win, int y, int x, char *buf, int winSize, int startidx, int colorPair, int attr, int xDelta)
{
  char *str;

  str = strdup(buf);
  if (xDelta > winSize-1)
    xDelta = winSize-1;
  RDBG("fnet refreshTxtInWin xDelta = "); RDBGi(xDelta); RDBG(", winSize = "); RDBGi(winSize); RDBG(",   startidx = "); RDBGi(startidx); RDBG("\n");

  if (strlen(str+startidx) > winSize-1) 
    str[startidx+winSize-1] = 0;
  if (colorPair != -1) 
    wattron(win, COLOR_PAIR(colorPair));
  mvwhline(win, y, x+xDelta, ' ', winSize-xDelta);
  wattron(win, attr);
  mvwprintw(win, y, x+xDelta, str+startidx);
  wattroff(win, attr);
  if (colorPair != -1) 
    wattroff(win, COLOR_PAIR(colorPair));
  Rfree(str);
}

//  0 <= xCursor <= strlen(buf)
// startidx - start index for printing buf

void 
refreshTxtInWinCursor(WINDOW *win, int y, int x, char *buf, int winSize, int startidx, int colorPair, int xCursor, int RpassMode)
{
  char *Rstr;
  int Rprinted = 0;
  int Rstrl = strlen(buf);
  char *Rbuf;
  int Ri = 0;

  if (startidx > Rstrl)
    return;
  if ((xCursor < 0) || (xCursor > Rstrl))
    return;
  if (xCursor < startidx)
    return;

  Rbuf = strdup(buf);

  if (RpassMode) 
  {
    while (Rbuf[Ri])
      Rbuf[Ri++] = '*';
  }

  RDBG("fnet refreshTxtInWinCursor xCursor = "); RDBGi(xCursor); RDBG(", Rstrl = "); RDBGi(Rstrl); 
  RDBG(", winSize = "); RDBGi(winSize); RDBG("\n");

  if (colorPair != -1) 
    wattron(win, COLOR_PAIR(colorPair));
  
  // clear line
  mvwhline(win, y, x, ' ', winSize); 

  // some string before cursor
  Rstr = strdup(Rbuf+startidx);
  Rstr[xCursor-startidx] = 0;
  RDBG("fnet refreshTxtInWinCursor 1: Rstr = \""); RDBG(Rstr); RDBG("\"\n");
  mvwprintw(win, y, x, Rstr);
  Rprinted = strlen(Rstr);
  Rfree(Rstr);
   
  // char in curor position
  wattron(win, A_REVERSE);
  RDBG("fnet refreshTxtInWinCursor Rprinted = "); RDBGi(Rprinted); RDBG("\n");
  if (Rprinted < winSize-1) 
  {
    if (Rprinted < Rstrl) 
    {
      asprintf(&Rstr, "%c", Rbuf[xCursor]);
      RDBG("fnet refreshTxtInWinCursor 21\n");
    }
    else 
    {
      asprintf(&Rstr, " ");
      RDBG("fnet refreshTxtInWinCursor 22\n");
    }
  }
  else 
  {
    asprintf(&Rstr, " ");
    RDBG("fnet refreshTxtInWinCursor 23\n");
  }
  RDBG("fnet refreshTxtInWinCursor 29: Rstr = \""); RDBG(Rstr); RDBG("\"\n");
  mvwprintw(win, y, x+Rprinted, Rstr);
  Rfree(Rstr);
  wattroff(win, A_REVERSE);
  Rprinted++;

  // some string after cursor

  if ((Rprinted < winSize-1) && 
      (Rprinted+startidx < Rstrl)) 
  {
    Rstr = strdup(Rbuf+startidx+Rprinted);
    if (winSize-Rprinted < strlen(Rstr))
       Rstr[winSize-Rprinted] = 0;
    RDBG("fnet refreshTxtInWinCursor 3: Rstr = \""); RDBG(Rstr); RDBG("\"\n");
    mvwprintw(win, y, x+Rprinted, Rstr);
    Rfree(Rstr);
  }

  if (colorPair != -1) 
    wattroff(win, COLOR_PAIR(colorPair));
  Rfree(Rbuf);
}

int 
readlineWaitForOneChar(WINDOW *win, int wy, int wx, char *buf, int sizebuf, int winSize, 
                       int colorPair, int *ctrlp, int *startidx, int *wxx) 
{
  int Rfirst = 1;
  int Rt = 0;

  while (!(Rt = readlineOneCharNoDelay(win, wy, wx, buf, sizebuf, winSize, colorPair, ctrlp, 0, startidx, wxx, &Rfirst)))
  {}
  return(Rt);
}

// if first = 2 readlineOneCharNoDelay is in password mode
int 
readlineOneCharNoDelay(WINDOW *win, int wy, int wx, char *buf, int sizebuf, int winSize, 
                       int colorPair, int *ctrlp, int Rnodelay, int *startidx, int *wxx, 
                       int *first)
{
  int klawisz = 0;
  int esc, Ri;
  int Rbufl;
  int RpassMode = 0;
  int Rit;

  if (!Rnodelay)
    curs_set(1);
  
  keypad(win, TRUE);
  if (((*first) - 2) >= 0) 
  {
    RpassMode = 1;
    (*first) = (*first) -2;
  }
  if (*first) 
  {
    refreshTxtInWinCursor(win,wy,wx,buf, winSize, *startidx, colorPair, *wxx, RpassMode);
    *first = 0;
  }
  if (RpassMode)
    (*first) = 2;

  while ((klawisz = mvwgetch(win, wy, wx+(*wxx)-(*startidx))) != 0) 
  {
    RDBG("fnet: readlineWaitForOneChar klawisz = "); RDBGi(klawisz); RDBG("\n");
    Rbufl = strlen(buf);
    esc = 1;
    switch(klawisz) 
    {
      case 263: // backspace 
      case KEY_DC: // delete
        if ((Rbufl && (*wxx) && (klawisz == 263)) || (Rbufl && (klawisz == KEY_DC))) 
        {
           if (Rbufl == *wxx) 
           {
             buf[Rbufl-1] = '\0';
             Rbufl = strlen(buf);
           }
           else 
           {
             Rit = Rbufl-(*wxx);
             for (Ri = 0; Ri < Rit; Ri++)
               buf[(*wxx)+Ri] = buf[(*wxx)+Ri+1]; 
             Rbufl = strlen(buf);
           }
           if (klawisz == 263) 
           {
             if (*startidx) 
             { 
               (*startidx)--;
               (*wxx)--;
             } 
             else 
               (*wxx)--;
           };
           refreshTxtInWinCursor(win,wy,wx,buf, winSize, *startidx, colorPair, *wxx, RpassMode);
        }
        esc = 0;
        curs_set(0);
        return(0);
      case KEY_HOME:
        *startidx = 0;
        (*wxx) = 0;
        esc = 0;
        refreshTxtInWinCursor(win,wy,wx,buf, winSize, *startidx, colorPair, *wxx, RpassMode);
        break;
      case KEY_END:
        *startidx = (strlen(buf) < winSize?0:strlen(buf)-winSize+1);
        (*wxx) = Rbufl;
        esc = 0;
        refreshTxtInWinCursor(win,wy,wx,buf, winSize, *startidx, colorPair, *wxx, RpassMode);
        break;
      case KEY_LEFT:
        if ((*startidx) && (*startidx == *wxx)) 
        {
          (*startidx)--; 
          (*wxx)--;
        }
        else 
          if (*wxx) 
            (*wxx)--;
        refreshTxtInWinCursor(win,wy,wx,buf, winSize, *startidx, colorPair, *wxx, RpassMode);
        esc = 0;
        break;
      case KEY_RIGHT:
        if ((*wxx) < Rbufl) 
        {
          if (((*startidx)+winSize <= Rbufl) && ((*startidx)+winSize == (*wxx)+1)) 
          {
             (*startidx)++; 
             if ((*wxx)+1 < Rbufl) 
               (*wxx)++;
          }
          else 
            (*wxx)++;
          refreshTxtInWinCursor(win,wy,wx,buf, winSize, *startidx, colorPair, *wxx, RpassMode);
          esc = 0;
        }
        break;
      case 10:          // Enter
        curs_set(0);
        return(2);
      case 16:          // CTRL-p
        if (*ctrlp) 
          (*ctrlp) = 0;
        else 
          (*ctrlp) = 1;
        return(0);
      case 11:          // CTRL-k
        (*ctrlp) = 2;
        return(2);
      case 9:           // TAB
        curs_set(0);
        return(1);
    }
    //RDBG("fnet.c: klawisz = "); RDBGc(klawisz); RDBG("\n");
    if ((esc) && ((klawisz >='A' && klawisz <= 'Z') ||
        (klawisz >='a' && klawisz <= 'z') ||
        (klawisz >='0' && klawisz <= '9') ||
        (klawisz == ' ' || klawisz == '/' || 
         klawisz == '\\' || klawisz == '#' || 
         klawisz == 234 || klawisz == 202 ||   // ê,Ê
         klawisz == 243 || klawisz == 211 ||   // ó,Ó
         klawisz == 177 || klawisz == 161 ||   // ±,¡
         klawisz == 182 || klawisz == 166 ||   // ¶,¦
         klawisz == 179 || klawisz == 163 ||   // ³,£
         klawisz == 191 || klawisz == 175 ||   // ¿,¯
         klawisz == 188 || klawisz == 172 ||   // ¼,¬
         klawisz == 230 || klawisz == 198 ||   // æ,Æ
         klawisz == 241 || klawisz == 209 ||   // ñ,Ñ
         klawisz == '.' || klawisz == ',' ||
         klawisz == '-')) && 
        (Rbufl+1 < sizebuf)) {
      if ((*wxx) == Rbufl) 
      {
        buf[*wxx] = (char)klawisz;
        buf[(*wxx)+1] = 0;
      }
      else 
      {
        for (Ri = 0; Ri < Rbufl-(*wxx)+1; Ri++)
          buf[Rbufl+1-Ri] = buf[Rbufl-Ri];
        buf[(*wxx)] = (char)klawisz;
      }
      (*wxx)++;

      if ((*startidx)+winSize == (*wxx)) 
        (*startidx)++;

      RDBG("fnetc: readlineOneCharNoDelay startidx = "); RDBGi(*startidx); RDBG(", winSize = "); RDBGi(winSize); RDBG(", wxx = "); RDBGi(*wxx); RDBG("\n");

      refreshTxtInWinCursor(win,wy,wx,buf, winSize, *startidx, colorPair, *wxx, RpassMode);
      RDBG("fnet readlineOneCharNoDelay exit 10\n");
      return(0);
    }
    if (Rnodelay)
      return(-1);
  }
  return(0);
}    

void 
RstrToUpper(char *str)
{
  int i;
  int k = strlen(str);

  for (i = 0; i < k; i++)
    str[i] = toupper(str[i]);
}

void 
RstrToUpperCopy(char *Dstr, char *Sstr)
{
  memcpy(Dstr, Sstr, strlen(Sstr)+1);
  RstrToUpper(Dstr);
}

void 
RgetFileFromTwoDirs(char **RfileName, char *RdirBig, char *RdirSmall)
{
  char *Rstr;
  int Rlen, RlB, RlS, Rlo;
  int Rs = 0;

  RlB = Rstrlen(RdirBig);
  RlS = Rstrlen(RdirSmall);
  if ((RdirBig != NULL) &&
      (RdirSmall != NULL) &&
      (RlB > RlS)) 
  {
    Rlen = RlB - RlS + 1;
    Rstr = Rmalloc(Rlen);
    memcpy(Rstr, RdirBig+RlS, Rlen);
    Rstr[Rlen] = 0;

    if (Rstr[0] == '/') 
      Rs = 1;

    Rlo = strlen(Rstr);
    if (Rlo > 1)
    if (Rstr[Rlo-1] == '/')
      RtruncateStr(&Rstr, Rlo-1);

    Rlo = strlen(Rstr)+1;

    RstrdupN(*RfileName, Rstr+Rs);
    Rfree(Rstr);
  }
  else
    *RfileName = NULL;
}

void *
Rmalloc(size_t Isize)
{
  void *Rret;

  Rret = malloc(Isize);
  if (Rret == NULL) 
  {
    RDBG("fnet: Rmalloc malloc failed\n");
    kill(0, SIGTERM);
    return(NULL);
  }
  return(Rret);
}

char *
Rstrcpy(char *dest, int destLen, const char *src, int srcLen)
{
  int CsrcLen = strlen(src);

  if (srcLen > -1)
  {
    if (CsrcLen > srcLen)
      return(NULL);
  }
  if (destLen > -1)
  {
  if (CsrcLen > destLen)
    return(NULL);
  }
  return(strncpy(dest, src, CsrcLen+1));
}

int
Rasprintf(char **strp, const char *fmt)
{
  int Ri;

  RDBG("fnet: Rasprintf start\n");
  if (fmt)
  {
    RDBG("fnet: Rasprintf 1\n");
    Ri = strlen(fmt)+1;
    RDBG("fnet: Rasprintf 2\n");
  }
  else
  {
    RDBG("fnet: Rasprintf 3\n");
    return(-1);
  }

  *strp = Rmalloc(Ri);
  if (*strp == NULL)
  {
    kill(0, SIGTERM);
  }
  memcpy(*strp, fmt, Ri);
  RDBG("fnet: Rasprintf end\n");
  return(Ri);
}

int
Rstraddtoend(char **strp, const char *fmt)
{
  int Ra;
  int Rb;
  char *Ri;

  if (*strp)
    Ra = strlen(*strp);
  else
    Ra = 0;
  if (fmt)
    Rb = strlen(fmt);
  else
    Rb = 0;

  Ri = Rmalloc(Ra+Rb+1);

  if (Ri == NULL)
    kill(0, SIGTERM);
  memcpy(Ri, *strp, Ra);
  memcpy(Ri+Ra, fmt, Rb+1);
  Rfree(*strp);
  *strp = Ri;
  return(strlen(Ri)+1);
}

int
Rcharaddtoend(char **strp, const char fmt)
{
  int Ra;
  char *Ri;

  if (*strp)
    Ra = strlen(*strp);
  else
    Ra = 0;

  Ri = Rmalloc(Ra+2);

  if (Ri == NULL)
    kill(0, SIGTERM);
  memcpy(Ri, *strp, Ra);
  Ri[Ra] = fmt;
  Ri[Ra+1] = 0;
  if (*strp)
    free(*strp);
  *strp = Ri;
  return(strlen(Ri)+1);
}

int
Rintaddtoend(char **strp, int fmt)
{
  char *Ri = NULL;

  asprintf(&Ri, "%i", fmt);

  if (Ri == NULL)
    kill(0, SIGTERM);
  Rstraddtoend(strp, Ri);
  free(Ri);
  return(strlen(Ri)+1);
}

int
Rmvwprintw(WINDOW *win, int y, int x, const char *fmt)
{
  int Ri;
  char *Rc1;
  char *Rc2;
  char Rchanged = 1;
  int Rc2start = 0;

  void
  RmvwprintwAddPercent()
  {
    char *Rt1 = NULL;
    int Rti = 0;

    Rt1 = malloc(strlen(Rc1)+2);
    if (Rt1 == NULL)
      kill(0, SIGTERM);
    Rti = Rc2-Rc1+1;
    memcpy(Rt1, Rc1, Rti);
    Rt1[Rti] = '%';
    memcpy(Rt1+Rti+1, Rc2+1, strlen(Rc2));  // strlen((Rc2+1)+1) == strlen(Rc2);
    Rfree(Rc1);
    Rc1 = Rt1;
    Rc2start = Rti+1;
    Rchanged = 1;
  }

  if (fmt == NULL)
    return(0);

  Rasprintf(&Rc1, fmt);

  while (Rchanged)
  {
    Rchanged = 0;
    if (Rc1 == NULL)
      kill(0, SIGTERM);
    Rc2 = Rc1+Rc2start;
    while (Rc2[0] && !Rchanged)
    {
      if (Rc2[0] == '%')
        RmvwprintwAddPercent();
      Rc2++;
    }
  }
  RDBG("fnet: RmvwprintwAddPercent fmt = |"); RDBG((char *)fmt); RDBG("|\n");
  RDBG("fnet: RmvwprintwAddPercent Rc1 = |"); RDBG(Rc1); RDBG("|\n");
  mvwprintw(win, y, x, Rc1);
  Ri = strlen(Rc1);
  Rfree(Rc1);
  RDBG("fnet: RmvwprintwAddPercent end\n");
  return(Ri);
}

int
RtruncateStr(char **Rstr, int Rpos)
{
  char *Rst = NULL;

  RDBG("fnet: RtruncateStr 1\n");
  if (Rpos > strlen(*Rstr))
    kill(0, SIGTERM);
  RDBG("fnet: RtruncateStr 2\n");
  Rst = Rmalloc(Rpos+1);
  memcpy(Rst, *Rstr, Rpos);
  RDBG("fnet: RtruncateStr 3\n");
  Rst[Rpos] = 0;
  Rfree(*Rstr);
  *Rstr = Rst;
  RDBG("fnet: RtruncateStr 10\n");
  return(strlen(*Rstr));
}

char *
Rstrdup(char *Rstr)
{
  char *Rout;

  if (Rstr == NULL)
    return(NULL);
  Rout = strdup(Rstr);

  if ((Rout == NULL) && (Rstr != NULL))
    kill(0, SIGTERM);
  return(Rout);
}

int
RstrCmp(char *src, char *dst) 
{
  if ((src == NULL) || (dst == NULL))
    kill(0, SIGTERM);
  return(!strcmp(src, dst));
}

ssize_t 
RwriteUnsignedInt(int fd, unsigned int buf)
{
  int Ri;
  int count = sizeof(unsigned int);

  if (fd < 0)
    kill(0, SIGTERM);
  Ri = write(fd, &buf, count);
  if (Ri != count)
    kill(0, SIGTERM);
  return(Ri);
}

ssize_t 
RwriteStr(int fd, const char *buf)
{
  ssize_t Ri;
  ssize_t count = strlen(buf)+1;

  if (fd < 0)
    kill(0, SIGTERM);
  if (buf == NULL)
  {
    count = 1;
    Ri = write(fd, "", count);
  }
  else
    Ri = write(fd, buf, count);
  if (Ri != count)
    kill(0, SIGTERM);
  return(Ri);
}

ssize_t 
RwriteChar(int fd, const char buf)
{
  ssize_t Ri;
  ssize_t count = sizeof(char);

  if (fd < 0)
    kill(0, SIGTERM);
  Ri = write(fd, &buf, count);
  if (Ri != count)
    kill(0, SIGTERM);
  return(Ri);
}

ssize_t 
RwriteTime_t(int fd, time_t buf)
{
  ssize_t Ri;
  ssize_t count = sizeof(time_t);

  if (fd < 0)
    kill(0, SIGTERM);
  Ri = write(fd, &buf, count);
  if (Ri != count)
    kill(0, SIGTERM);
  return(Ri);
}

size_t
RreadStr(int fd, char **buf)
{
  char Rch;

  if (*buf)
    Rfree(*buf);
  while (read(fd, &Rch, 1) == 1)
  {
    Rcharaddtoend(buf, Rch);
    if (Rch == 0)
      return(strlen(*buf));
  }
  Rfree(*buf);
  return(-1);
}

int
RreadUnsignedInt(int fd, unsigned int *buf)
{
  int Ri = sizeof(unsigned int);

  if (read(fd, buf, Ri) == Ri)
    return(Ri);
  return(-1);
}

int
RreadChar(int fd, char *buf)
{
  int Ri = sizeof(char);

  if (read(fd, buf, Ri) == Ri)
    return(Ri);
  return(-1);
}

time_t
RreadTime_t(int fd, time_t *buf)
{
  int Ri = sizeof(time_t);

  if (read(fd, buf, Ri) == Ri)
    return(Ri);
  return(-1);
}

size_t
Rstrlen(const char *s)
{
  if (s == NULL)
    return(0);
  else
    return(strlen(s));
}

void
RchdirUp(char **Rstr)
{
  int RicU;
  char *RstrT = NULL;

  if (Rstr == NULL)
    kill(0, SIGTERM);
  if (*Rstr == NULL)
    kill(0, SIGTERM);

  RicU = strlen(*Rstr);
  if (RicU == 0)
    return;

  RstrT = Rstrdup(*Rstr);
  if (RstrT[RicU-1] == '/')
  {
    RtruncateStr(&RstrT, RicU-1);
  }

  while(strlen(RstrT) > 0)
  {
    RicU = strlen(RstrT);
    if (RstrT[RicU-1] != '/')
      RtruncateStr(&RstrT, RicU-1);
    else
    {
      RstrdupN(*Rstr, RstrT);
      return;
    }
  }
  RstrdupN(*Rstr, RstrT);
}

void
RparsingDir(char **Rstr)
{
  int Ri = 0;
  char *RstrT = NULL;

  if (*Rstr == NULL)
    kill(0, SIGTERM);

  Ri = Rstrlen(*Rstr);

  if (*Rstr[0] == '/')
    RstrdupN(RstrT, *Rstr+1)
  else
    RstrdupN(RstrT, *Rstr);
  
  if (RstrT[Ri] != '/')
    Rcharaddtoend(&RstrT, '/');

  Rfree(*Rstr);
  *Rstr = RstrT;
}

int
RmakeCopyFileFromPIDtoPID(int Rdst, int Rsrc)
{
#define READBF 1024

  char Rdata[READBF];
  int Rread;
  int Rwrite;

  if ((Rdst <= 0) || (Rsrc <= 0))
    kill(0, SIGHUP);
  while ((Rread = read(Rsrc, &Rdata, READBF)))
  {
    Rwrite = write(Rdst, &Rdata, Rread);
    if (Rread != Rwrite)
      kill(0, SIGHUP);
  }
  return(1);
}
