/*
       downloadlist.c
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/

#define _GNU_SOURCE

#ifdef HAVE_STRING_H
#include <string.h>
#else  /* not HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#endif /* not HAVE_STRING_H */

#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include "../config.h"
#include "rtypedef.h"
#include "downloadlist.h"
#include "fnet.h"
#include "debug.h"

TTdownload *
RgetdownloadListNr(TTdownload *RdownloadList, int Rnr)
{
  while (Rnr-- > 0)
  {
    RdownloadList = RdownloadList->next;
    if (RdownloadList == NULL)
      kill(0, SIGTERM);
  }
  return(RdownloadList);
}

TTdownload *
RgetdownloadListLast(TTdownload *RdownloadList)
{
  if (RdownloadList == NULL)
    return(NULL);
  while (RdownloadList->next != NULL)
    RdownloadList = RdownloadList->next;
  return(RdownloadList);
}

TTdownload *
RgetdownloadListFirst(TTdownload *RdownloadList)
{
  if (RdownloadList == NULL)
    return(NULL);
  while (RdownloadList->previous)
    RdownloadList = RdownloadList->previous;
  return(RdownloadList);
}

int
RgetdownloadListCount(TTdownload *RdownloadList)
{
  int Rc = 0;

  if (RdownloadList == NULL) 
    return(0);
  while (RdownloadList)
  {
     RdownloadList = RdownloadList->next;
     Rc++;
  }
  return(Rc);
}

void 
RfreeAllTTdownload(TTdownload **RdownloadList)
{
  *RdownloadList = RgetdownloadListFirst(*RdownloadList);

  while (*RdownloadList)
    RfreeOneTTdownloadAndGoToFirst(RdownloadList);
}

void
RfreeOneTTdownloadAndGoToFirst(TTdownload **RdownloadList)
{
  TTdownload *RdownloadListTmp = NULL;

  if (*RdownloadList)
  {
    if ((*RdownloadList)->previous)
    {
      (*RdownloadList)->previous->next = (*RdownloadList)->next;
      RdownloadListTmp = (*RdownloadList)->previous;
    }
    if ((*RdownloadList)->next)
    {
      (*RdownloadList)->next->previous = (*RdownloadList)->previous;
      if (RdownloadListTmp == NULL)
        RdownloadListTmp= (*RdownloadList)->next;
    }
 
    Rfree((*RdownloadList)->username);
    Rfree((*RdownloadList)->password);
    Rfree((*RdownloadList)->groupnameR);
    Rfree((*RdownloadList)->hostname);
    Rfree((*RdownloadList)->pathRLbase);
    Rfree((*RdownloadList)->pathR);
    Rfree((*RdownloadList)->pathL);
    Rfree((*RdownloadList)->fullfilename);
    Rfree((*RdownloadList)->filename);
    Rfree((*RdownloadList)->sizeDisplayR);
    Rfree((*RdownloadList)->sizeDisplayDownR);
    Rfree((*RdownloadList)->percent);
    Rfree((*RdownloadList));

    *RdownloadList = RgetdownloadListFirst(RdownloadListTmp);
  }
}

void
RaddOneTTdownloadToLast(TTdownload **RdownloadList,
                        char *username, char *password,
                        char *groupnameR, char *hostname, char *pathRLbase, char *pathR, char *pathL, 
                        char *fullfilename, char *filename, unsigned int sizeR,
                        char *sizeDisplayR, char *sizeDisplayDownR, 
                        char *percent, char active, char upload, 
                        time_t delayRetry, time_t nextRetry)
{
  if (*RdownloadList)
  {
    *RdownloadList = RgetdownloadListLast(*RdownloadList);
    (*RdownloadList)->next = Rmalloc(sizeof(TTdownload));
    (*RdownloadList)->next->previous = *RdownloadList;
    *RdownloadList = (*RdownloadList)->next;
  }
  else
  {
    *RdownloadList = malloc(sizeof(TTdownload));
    (*RdownloadList)->previous = NULL;
  }
  (*RdownloadList)->next = NULL;
  
  (*RdownloadList)->username = Rstrdup(username);
  (*RdownloadList)->password = Rstrdup(password);
  (*RdownloadList)->groupnameR = Rstrdup(groupnameR);
  (*RdownloadList)->hostname = Rstrdup(hostname);
  (*RdownloadList)->pathRLbase = Rstrdup(pathRLbase);
  (*RdownloadList)->pathR = Rstrdup(pathR);
  (*RdownloadList)->pathL = Rstrdup(pathL);
  (*RdownloadList)->fullfilename = Rstrdup(fullfilename);
  (*RdownloadList)->filename = Rstrdup(filename);
  (*RdownloadList)->sizeR = sizeR;
  (*RdownloadList)->sizeDisplayR = Rstrdup(sizeDisplayR);
  (*RdownloadList)->sizeDisplayDownR = Rstrdup(sizeDisplayDownR);
  (*RdownloadList)->percent = Rstrdup(percent);
  (*RdownloadList)->active = active;
  (*RdownloadList)->upload = upload;
  (*RdownloadList)->delayRetry = delayRetry;
  (*RdownloadList)->nextRetry = nextRetry;
}

TTdownload *
RcreateEmptyTTdownload()
{
  TTdownload *RdownloadList = Rmalloc(sizeof(TTdownload));

  RdownloadList->username = NULL;
  RdownloadList->password = NULL;
  RdownloadList->groupnameR = NULL;
  RdownloadList->hostname = NULL;
  RdownloadList->pathRLbase = NULL;
  RdownloadList->pathR = NULL;
  RdownloadList->pathL = NULL;
  RdownloadList->fullfilename = NULL;
  RdownloadList->sizeR = -1;
  RdownloadList->sizeDisplayR = NULL;
  RdownloadList->sizeDisplayDownR = NULL;
  RdownloadList->percent = NULL;
  RdownloadList->active = 0;
  RdownloadList->upload = 0;
  RdownloadList->delayRetry = -1;
  RdownloadList->nextRetry = -1;
  RdownloadList->next = NULL;
  RdownloadList->previous = NULL;

  return(RdownloadList);
}


int 
RgetHumanSizeMalloc(char **Rdst, int RstrMaxLen, unsigned int Rsrc)
{
  int Ri;

  RDBG("downloadlist: RgetHumanSizeMalloc start\n");
  Rfree(*Rdst);
  RDBG("downloadlist: RgetHumanSizeMalloc 1\n");
  *Rdst = Rmalloc(RstrMaxLen);
  Ri = RgetHumanSize(*Rdst, RstrMaxLen, Rsrc);
  return(Ri);
}

/*   Rstr - value in bytes
 *
 *   function return 
 *      Rdst[0..RstrMaxLen-2] = "123.3M"
 *      Rdst[RstrMaxLen-1] = 0;
 *
 *      Rsrc = 200  Rdst 2  = "2"
 *      Rsrc = 200  Rdst 3  = "20"
 *      Rsrc = 200  Rdst 4  = "200"
 *      Rsrc = 200  Rdst 5  = "200M"
 *      Rsrc = 200  Rdst 10 = "     200M"
 **********************************************
 *   prefer RstrMaxLen >= 5
*/
int 
RgetHumanSize(char *Rdst, int RstrMaxLen, unsigned int Rsrc)
{
  int Rjl;
  int RiT;
  int Rs = 0; 
  char Rje = 'B';
  char *RdstTmp = NULL;
  char *RstrTmp = NULL;
  char *Rj = NULL;
  char *Rf = NULL;
  char *Rl = NULL;
  char *RstrT = NULL;
  int Rspc = 0;

  if (RstrMaxLen <= 0)
    return(-1);

  RdstTmp = Rmalloc(RstrMaxLen);
  Rf = Rmalloc(RstrMaxLen);
  Rl = Rmalloc(RstrMaxLen);

  RDBG("getHumanSize: getHumanSize Rsrc == "); RDBGi(Rsrc); RDBG("\n");
  if ((Rsrc/1024) > 0) 
  { 
    Rs = (Rsrc % 1024);  
    Rsrc = Rsrc/1024; 
    Rje = 'K';  
  }
  if ((Rsrc/1024) > 0) 
  { 
    Rs = (Rsrc % 1024);  
    Rsrc = Rsrc/1024; 
    Rje = 'M';  
  }
  if ((Rsrc/1024) > 0) 
  { 
    Rs = (Rsrc % 1024);  
    Rsrc = Rsrc/1024; 
    Rje = 'G';  
  }
  if ((Rsrc/1024) > 0) 
  { 
    Rs = (Rsrc % 1024);  
    Rsrc = Rsrc/1024; 
    Rje = 'T';  
  }
  itoa(Rf, RstrMaxLen, Rsrc);
  if (Rs > 0)
  {
    itoa(Rl, RstrMaxLen, Rs);
    Rj = Rstrdup(Rf);
    Rcharaddtoend(&Rj, '.');
    Rstraddtoend(&Rj, Rl);
    Rcharaddtoend(&Rj, '0');
  }
  else
  {
    if (Rje == 'B')
      Rj = Rstrdup(Rf);
    else
    {
      Rj = Rstrdup(Rf);
      Rstraddtoend(&Rj, ".00");
    }
  }
  Rjl = strlen(Rj);
  RDBG("getHumanSize: RgetHumanSize Rj = "); RDBG(Rj); RDBG("\n");

  memset(RdstTmp, ' ', RstrMaxLen);
  RdstTmp[RstrMaxLen-1] = 0;

  RDBG("getHumanSize: getHumanSize Rf == "); RDBG(Rf); RDBG("\n");
  RDBG("getHumanSize: getHumanSize Rl == "); RDBG(Rl); RDBG("\n");
  RDBG("getHumanSize: getHumanSize Rj == "); RDBG(Rj); RDBG("\n");
  RDBG("getHumanSize: getHumanSize 1 RdstTmp == "); RDBG(RdstTmp); RDBG("\n");

  /*
   * Rjl+1      - string(Rjl)+1(\0)
   * RstrMaxLen - is with \0
   */
  RiT = Rjl+1-RstrMaxLen;
  // if < must add spaces at the begin on the return string
  if (RiT < 0)
  {
    memcpy(RdstTmp+abs(RiT), Rj, Rjl);
    Rspc = 1;
  }
  else
    // if = return identical string
    if (RiT == 0)
      memcpy(RdstTmp, Rj, Rjl);
    else
      // if > return truncate string
      memcpy(RdstTmp, Rj, RstrMaxLen-1);

  RDBG("getHumanSize: getHumanSize 2 RdstTmp == "); RDBG(RdstTmp); RDBG("\n");

  RstrdupN(RstrT, index(RdstTmp, '.'));
  if (RstrT)
     RiT = strlen(RstrT);
  else
     RiT = 0;
  RDBG("getHumanSize: getHumanSize 4\n");
  if (RiT > 0)
  {
    if (RiT == 2)
    {
      RdstTmp[RstrMaxLen-3] = Rje;
      RdstTmp[RstrMaxLen-2] = 0;
      RstrdupN(RstrTmp, Rstrdup(" "));
      Rstraddtoend(&RstrTmp, RdstTmp);
      memcpy(RdstTmp, RstrTmp, strlen(RstrTmp)+1);
    }
    else
      RdstTmp[RstrMaxLen-2] = Rje;
    RDBG("getHumanSize: getHumanSize 5\n");
  }
  else
    if (Rspc)
    {
      RstrTmp = Rstrdup(RdstTmp+1);
      Rcharaddtoend(&RstrTmp, Rje);
      memcpy(RdstTmp, RstrTmp, RstrMaxLen);
      RDBG("getHumanSize: getHumanSize 6\n");
    }
  RDBG("getHumanSize: getHumanSize 7\n");
  memcpy(Rdst, RdstTmp, RstrMaxLen);

  Rfree(Rf);
  Rfree(Rl);
  Rfree(Rj);
  Rfree(RdstTmp);
  return(1);
}


