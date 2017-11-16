/*
       bookmarks.c
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
  
    last update: 2004-06-02
*/

#define _GNU_SOURCE

#include "../config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else  /* not HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#endif /* not HAVE_STRING_H */

#include <libintl.h>
#include <libsmbclient.h>
#include <unistd.h>
#include <stdlib.h>
#include "dulist.h"
#include "consts.h"
#include "debug.h"
#include "rtypedef.h"
#include "bookmarks.h"
#include "convert.h"
#include "convertm.h"
#include "fnet.h"

#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

void 
RgotoFirstBookmark(TTbookmark **Rbk)
{
  if (*Rbk == NULL)
    return;
  else 
  {
    while ((*Rbk)->previous != NULL)
      (*Rbk) = (*Rbk)->previous;
  }
}

void 
RgotoLastBookmark(TTbookmark **Rbk)
{
  if (*Rbk == NULL)
    return;
  else 
  {
    while ((*Rbk)->next != NULL)
      (*Rbk) = (*Rbk)->next;
  }
}

int 
RgotoXBookmark(TTbookmark **Rbk, int Rnr)
{
  if (*Rbk == NULL)
    return(0);
  else 
  {
    RgotoFirstBookmark(Rbk);
    while (((*Rbk)->next) && (Rnr-- > 0)) 
       (*Rbk) = (*Rbk)->next;
  }
  if (!Rnr)
    return(1);
  return(0); 
}

void 
RfreeBookmark(TTbookmark *Rbk)
{
  int RbkSize = sizeof(TTbookmark);
  TTbookmark *Rbkt;

  if (Rbk == NULL)
    return; 
  RgotoFirstBookmark(&Rbk);
  while (Rbk != NULL) 
  {
    Rbkt = Rbk;
    Rbk = Rbk->next;
    realloc(Rbkt, RbkSize);
  }
}

int 
RfindInBookmark(TTbookmark **Rbk, int Riselect, char *Riagroup, char *Riahost, char *Riadir, char *Rifile, char *Riusername, char *Ripassword)
{
  TTbookmark *Rbkt = *Rbk;

  RgotoFirstBookmark(&Rbkt);
  while (Rbkt != NULL) 
  {
    if (Rbkt->type == Riselect) 
    {
      if (Riselect == 'R') 
      {
        if ((RstrCmp(Rbkt->username, Riusername)) &&
            (RstrCmp(Rbkt->password, Ripassword)) &&
            (RstrCmp(Rbkt->group, Riagroup)) &&
            (RstrCmp(Rbkt->host, Riahost)) &&
            (RstrCmp(Rbkt->dir, Riadir)) &&
            (RstrCmp(Rbkt->file, Rifile))) 
        {
           *Rbk = Rbkt;
           return(1);
        }
      }
      else 
      {
        if ((RstrCmp(Rbkt->dir, Riadir)) &&
            (RstrCmp(Rbkt->file, Rifile))) 
        {
           *Rbk = Rbkt;
           return(1);
        }
      }
    }
    Rbkt = Rbkt->next;
  }
  return(0);
}

void 
RaddToBookmark(TTbookmark **Rbk, int Riselect, char *Riagroup, char *Riahost, char *Riadir, char *Rifile, char *Riusername, char *Ripassword)
{
  if ((*Rbk) == NULL) 
  {
    RDBG("bookmarks RaddToBookmark add first\n");
    *Rbk = Rmalloc(sizeof(TTbookmark));
    (*Rbk)->previous = NULL;
  }
  else 
  {
    RDBG("bookmarks RaddToBookmark add next\n");
    RgotoLastBookmark(Rbk);
    (*Rbk)->next = Rmalloc(sizeof(TTbookmark));
    (*Rbk)->next->previous = (*Rbk);
    *Rbk = (*Rbk)->next;
  }
  (*Rbk)->next = NULL;
  (*Rbk)->type = Riselect;
  (*Rbk)->del = 'X';
  Rstrcpy((*Rbk)->username, RMAXGHLEN, Riusername, -1);
  Rstrcpy((*Rbk)->password, RMAXGHLEN, Ripassword, -1);
  Rstrcpy((*Rbk)->group, RMAXGHLEN, Riagroup, -1);
  Rstrcpy((*Rbk)->host, RMAXGHLEN, Riahost, -1);
  Rstrcpy((*Rbk)->dir, TTBOOKMARKDIRLEN, Riadir, -1);
  Rstrcpy((*Rbk)->file, TTBOOKMARKFILELEN, Rifile, -1);
}

void 
RopenBookmark(TTbookmark **Rbk, char *RfileName)
{
  int Rfhandle;
  int RbkSize = sizeof(TTbookmark);
  TTbookmark Rbkt;

  RDBG("bookmarks RopenBookmark start\n");
  if (*Rbk != NULL)
    return;
 
  Rfhandle = open(to_sys(RfileName), O_RDONLY, 0644);
  if (Rfhandle > 0) 
  {
    RDBG("bookmarks RopenBookmark Rfhandle > 0\n");
    while (read(Rfhandle, &Rbkt, RbkSize) == RbkSize) 
    {
      RDBG("bookmarks RopenBookmark readed 1\n");
      if (*Rbk) 
      {
        (*Rbk)->next = Rmalloc(RbkSize);
        memcpy((*Rbk)->next, &Rbkt, RbkSize);
        (*Rbk)->next->previous = (*Rbk);
        (*Rbk) = (*Rbk)->next;
      }
      else 
      {
        (*Rbk) = Rmalloc(RbkSize);
        memcpy(*Rbk, &Rbkt, RbkSize);
        (*Rbk)->previous = NULL;
      }
      (*Rbk)->next = NULL;
    }
    close(Rfhandle);
  }
  RDBG("bookmarks RopenBookmark end\n");
}

void 
RsaveBookmark(TTbookmark *Rbk, char *RfileName)
{
  int Rfhandle;
  int RbkSize = sizeof(TTbookmark);

  RDBG("bookmarks RsaveBookmark start\n");
  RgotoFirstBookmark(&Rbk);
  unlink(to_sys(RfileName));
  Rfhandle = open(to_sys(RfileName), O_CREAT | O_NONBLOCK | O_WRONLY | O_TRUNC, 0644);
  if (Rfhandle > 0) 
  {
    RDBG("bookmarks RsaveBookmark Rfhandle > 0\n");
    while (Rbk) 
    {
      if (Rbk->del == 'X') 
      {
        if (write(Rfhandle, Rbk, RbkSize) != RbkSize) 
        {
          printf(_("error saving bookmark list\n"));
          exit(0);
        }
      }
      RDBG("bookmarks RsaveBookmark written 1\n");
      Rbk = Rbk->next;
    }
    close(Rfhandle);
  }
  RDBG("bookmarks RsaveBookmark end\n");
}

unsigned int 
RgetBookmarkCount(TTbookmark *Rbk)
{
  unsigned int Ri = 1;

  RgotoFirstBookmark(&Rbk);
  if (Rbk == NULL)
    return(0);
  while (Rbk->next != NULL) 
  {
    Ri++;
    Rbk = Rbk->next;
  }
  return(Ri);
}

