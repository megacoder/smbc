/*
       ttlist.c
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
#include "ttlist.h"

void
RgoToFirstTTlist(TTlist **Rlist)
{
  if (*Rlist == NULL)
  {
    RDBG("ttlist: RgoToFirstTTlist *Rlist == NULL\n");
    kill(0, SIGTERM);
  }

  while ((*Rlist)->previous)
    *Rlist = (*Rlist)->previous;
}

TTlist *
RreturnFirstTTlist(TTlist *Rlist)
{
  RgoToFirstTTlist(&Rlist);
  return(Rlist);
}

void
RgoToLastTTlist(TTlist **Rlist)
{
  if (*Rlist == NULL)
    kill(0, SIGTERM);

  while ((*Rlist)->next)
    *Rlist = (*Rlist)->next;
}

TTlist *
RreturnLastTTlist(TTlist *Rlist)
{
  RgoToLastTTlist(&Rlist);
  return(Rlist);
}

void
RgoToRecordInTTlist(TTlist **Rlist, int Ri)
{
  RDBG("ttlist: RgoToRecordInTTlist Ri = "); RDBGi(Ri); RDBG("\n");
  if (*Rlist == NULL)
  {
    RDBG("ttlist: RgoToRecordInTTlist *Rlist == NULL\n");
    kill(0, SIGTERM);
  }
  RDBG("ttlist: RgoToRecordInTTlist 2\n");
  RgoToFirstTTlist(Rlist);
  if (Ri > 0)
    while (Ri--)
    {
      *Rlist = (*Rlist)->next;
      if (*Rlist == NULL)
      {
        RDBG("ttlist: RgoToRecordInTTlist *Rlist == NULL\n");
        kill(0, SIGTERM);
      }
    }
  RDBG("ttlist: RgoToRecordInTTlist end\n");
}

void
RgoToRecordInTTlistMaster(TTlist **Rlist, int Ri, TTlist *RlistMaster)
{
  if (RlistMaster == NULL)
  {
    RDBG("ttlist: RgoToRecordInTTlistMaster RlistMaster = NULL\n");
    kill(0, SIGTERM);
  }
  *Rlist = RlistMaster;
  RgoToRecordInTTlist(Rlist, Ri);
}

void
RunselectAllRecords(TTlist *Rlist)
{
  RgoToFirstTTlist(&Rlist);
  while (Rlist)
  {
    Rlist->select = 0;
    Rlist = Rlist->next;
  }
}

void
RselectAllRecords(TTlist *Rlist)
{
  RgoToFirstTTlist(&Rlist);
  while (Rlist)
  {
    Rlist->select = 1;
    Rlist = Rlist->next;
  }
}

void
RinverseSelectAllRecords(TTlist *Rlist)
{
  RgoToFirstTTlist(&Rlist);
  while (Rlist)
  {
    if (Rlist->select == 0)
      Rlist->select = 1;
    else
      Rlist->select = 0;
    Rlist = Rlist->next;
  }
}

unsigned char
RexistAnySelect(TTlist *Rlist)
{
  RgoToFirstTTlist(&Rlist);
  while (Rlist)
  {
    if (Rlist->select)
      return(1);
    Rlist = Rlist->next;
  }
  return(0);
}

void
RrotateTwoRecords(TTlist *Rone, TTlist *Rtwo)
{
  char *col1;
  char *col2;
  char *col3;
  char *col4;
  unsigned char select;
  int size;

  if ((Rone == NULL) || (Rtwo == NULL))
    kill(0, SIGTERM);

  col1 = Rone->col1;
  col2 = Rone->col2;
  col3 = Rone->col3;
  col4 = Rone->col4;
  select = Rone->select;
  size = Rone->size;

  Rone->col1 = Rtwo->col1;
  Rone->col2 = Rtwo->col2;
  Rone->col3 = Rtwo->col3;
  Rone->col4 = Rtwo->col4;
  Rone->select = Rtwo->select;
  Rone->size = Rtwo->size;

  Rtwo->col1 = col1;
  Rtwo->col2 = col2;
  Rtwo->col3 = col3;
  Rtwo->col4 = col4;
  Rtwo->select = select;
  Rtwo->size = size;
}

void
RcreateNextOrNewTTlistAndAddData(TTlist **RlistI,
                                 char *col1I, char *col2I, char *col3I, char *col4I,
                                 int sizeI,
                                 unsigned char selectI)
{
  if (*RlistI)
  {
    (*RlistI)->next = Rmalloc(sizeof(TTlist));
    (*RlistI)->next->previous = *RlistI;
    *RlistI = (*RlistI)->next;
  }
  else
  {
    *RlistI = Rmalloc(sizeof(TTlist));
    (*RlistI)->previous = NULL;
  }
  (*RlistI)->next = NULL;

  (*RlistI)->col1 = Rstrdup(col1I);
  (*RlistI)->col2 = Rstrdup(col2I);
  (*RlistI)->col3 = Rstrdup(col3I);
  (*RlistI)->col4 = Rstrdup(col4I);
  (*RlistI)->size = sizeI;
  (*RlistI)->select = selectI;
}

void
RfreeAllInTTlist(TTlist **RlistI)
{
  TTlist *RlistTt;

  while (*RlistI)
  {
    RlistTt = *RlistI;
    Rfree((*RlistI)->col1);
    Rfree((*RlistI)->col2);
    Rfree((*RlistI)->col3);
    Rfree((*RlistI)->col4);
    *RlistI = (*RlistI)->next;
    Rfree(RlistTt);
  }
  *RlistI = NULL;
}

int
RreturnCountTTlist(TTlist *RlistI)
{
  int Ri = 0;
  while (RlistI)
  {
    Ri++;
    RlistI = RlistI->next;
  }
  return(Ri);
}
