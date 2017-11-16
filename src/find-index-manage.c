/*
       find-index-manage.c
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/
#define _GNU_SOURCE
#define READBF 1024

#include "../config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else  /* not HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#endif /* not HAVE_STRING_H */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <libsmbclient.h>
#include <stdlib.h>
#include <math.h>
#include "find-index-manage.h"
#include "debug.h"
#include "convert.h"
#include "convertm.h"
#include "fnet.h"

TTfind *TfindRecord = NULL;

int 
RmakeCopyFileWithPID(char *Rdst, char *Rsrc)
{
  int Rfdi, Rfdo;
  char Rdata[READBF];
  int Rread;

  RDBG("find-index-manage: RmakeCopyFile start\n");
  Rfdi = open(to_sys(Rsrc), O_RDONLY);
  if (Rfdi <= 0) 
  {
    RDBG("find-index-manage: can't open ");
    RDBG(Rsrc);
    RDBG(" file\n");
    return(0);
  }
  Rfdo = open(to_sys(Rdst), O_CREAT | O_NONBLOCK | O_WRONLY | O_TRUNC, 0600);
  if (Rfdo <= 0) 
  {
    if (Rfdi > 0)
      close(Rfdi);
    RDBG("find-index-manage: can't open ");
    RDBG(Rdst);
    RDBG(" file\n");
    return(0);
  }
  while ((Rread = read(Rfdi, &Rdata, READBF))) 
    write(Rfdo, &Rdata, Rread);
  close(Rfdi);
  close(Rfdo);
  return(1);
}

int 
RindexFileUnlink(char *RpathToFile)
{
  int Ri;

  Ri = unlink(to_sys(RpathToFile));
  return(1);
}

int 
RindexFileOpen(char *RpathToFile, int *RfileDesc)
{
  (*RfileDesc) = open(to_sys(RpathToFile), O_RDONLY);
  if ((*RfileDesc) > 0) {
    RDBG("find-index-manage: RindexFileInit exit ok\n");
    return(1);
  }
  RDBG("find-index-manage: RindexFileInit exit fail\n");
  return(0);
}

void 
RindexFileClose(int *RfileDesc)
{
  if ((*RfileDesc) > 0)
    close(*RfileDesc);
  (*RfileDesc) = 0;
}

int 
RindexFileCreate(char *RpathToFile, int *RfileDesc)
{
  (*RfileDesc) = open(to_sys(RpathToFile), O_CREAT | O_NONBLOCK | O_RDWR | O_TRUNC, 0600);
  if ((*RfileDesc) > 0)
    return(1);
  return(0);
}

int 
RindexFileReadOneRecord(int RfileDesc, TTfind **TfindRecord)
{
  char Rc;
  char *Rstr = NULL;
  char *IIgroup = NULL;
  char *IIhost = NULL;
  char *IIpath = NULL;
  char *IIfile = NULL;

  *TfindRecord = NULL;
  while (read(RfileDesc, &Rc, 1) > 0) 
  {
    if (Rc)
    {
      Rcharaddtoend(&Rstr, Rc);
    }
    else 
    {
      if (Rstr == NULL)
        Rstr = Rstrdup("");
      if (IIgroup == NULL) 
      {
        IIgroup = Rstrdup(Rstr);
        Rfree(Rstr);
      }
      else 
      {
        if (IIhost == NULL) 
        {
          IIhost = Rstrdup(Rstr);
          Rfree(Rstr);
        }
        else 
        {
          if (IIpath == NULL) 
          {
            IIpath = Rstrdup(Rstr);
            Rfree(Rstr);
          }
          else 
          {
            if (IIfile == NULL)
            {
              IIfile = Rstrdup(Rstr);
              Rfree(Rstr);
            }
            else
            {
            *TfindRecord = Rmalloc(sizeof(TTfind));
            (*TfindRecord)->group = Rstrdup(IIgroup);
            (*TfindRecord)->host = Rstrdup(IIhost);
            (*TfindRecord)->dir = Rstrdup(IIpath);
            (*TfindRecord)->file = Rstrdup(IIfile);
            (*TfindRecord)->access_denied = Rstrdup(Rstr);
            RDBG("smbc: find-index-manage (*TfindRecord)->group = "); RDBG((*TfindRecord)->group); RDBG("\n");
            RDBG("smbc: find-index-manage (*TfindRecord)->host = "); RDBG((*TfindRecord)->host); RDBG("\n");
            RDBG("smbc: find-index-manage (*TfindRecord)->dir = "); RDBG((*TfindRecord)->dir); RDBG("\n");
            RDBG("smbc: find-index-manage (*TfindRecord)->file = "); RDBG((*TfindRecord)->file); RDBG("\n");
            RDBG("smbc: find-index-manage (*TfindRecord)->access_denied = "); RDBG((*TfindRecord)->access_denied); RDBG("\n");

            Rfree(IIgroup);
            Rfree(IIhost);
            Rfree(IIpath);
            Rfree(IIfile);
            Rfree(Rstr);
            //RDBG("find-index-manage: RindexFileReadOneRecord end 1\n");
            return(1);
            }
          }
        }
      }
    }
  }
  Rfree(IIgroup);
  Rfree(IIhost);
  Rfree(IIpath);
  Rfree(IIfile);
  Rfree(Rstr);
  //RDBG("find-index-manage: RindexFileReadOneRecord end 0\n");
  return(0);
}

int 
RindexFileWriteOneRecord(int RfileDesc, TTfind *TfindRecord)
{
  int Ri;

  if (RfileDesc > 0) 
  {
    RDBG("smbc: RindexFileWriteOneRecord TfindRecord->group = "); RDBG(TfindRecord->group); RDBG("\n");
    RDBG("smbc: RindexFileWriteOneRecord TfindRecord->host = "); RDBG(TfindRecord->host); RDBG("\n");
    RDBG("smbc: RindexFileWriteOneRecord TfindRecord->dir = "); RDBG(TfindRecord->dir); RDBG("\n");
    RDBG("smbc: RindexFileWriteOneRecord TfindRecord->file = "); RDBG(TfindRecord->file); RDBG("\n");
    RDBG("smbc: RindexFileWriteOneRecord TfindRecord->access_denied = "); RDBG(TfindRecord->access_denied); RDBG("\n");
    
    Ri = strlen(TfindRecord->group)+1; 
    if (write(RfileDesc, TfindRecord->group, Ri) != Ri)
      return(0);
    Ri = strlen(TfindRecord->host)+1; 
    if (write(RfileDesc, TfindRecord->host, Ri) != Ri)
      return(0);
    Ri = strlen(TfindRecord->dir)+1; 
    if (write(RfileDesc, TfindRecord->dir, Ri) != Ri)
      return(0);
    Ri = strlen(TfindRecord->file)+1; 
    if (write(RfileDesc, TfindRecord->file, Ri) != Ri)
      return(0);
    Ri = strlen(TfindRecord->access_denied)+1; 
    if (write(RfileDesc, TfindRecord->access_denied, Ri) != Ri)
      return(0);
  }
  return(1);
}

int 
RindexFileSetSeek(int RfileDesc, long int Rnr)
{
  int Rt = 1;
  int Rcont = 1;
  TTfind *TfindRecord = NULL;

  RDBG("find-index-manage: RindexFileSetSeek start, try set ");
  RDBGi(Rnr); RDBG("\n");
  // read counter
  RindexFileReadCounter(RfileDesc);

  while (Rcont && (Rt < Rnr)) 
  {
    if (!RindexFileReadOneRecord(RfileDesc, &TfindRecord))
      Rcont = 0;
    Rt++;
    RindexFreeOneTTfind(&TfindRecord);
  }
  if (Rt == Rnr) 
    return(1);
  return(0);
}

int 
RindexFileExistAnyRecords(int RfileDesc)
{
  TTfind *TfindRecord = NULL;
  
  // read counter
  RindexFileReadCounter(RfileDesc);

  if (RindexFileReadOneRecord(RfileDesc, &TfindRecord)) 
  {
    RindexFreeOneTTfind(&TfindRecord);
    RDBG("smbc: RindexFileExistAnyRecords exit 1\n");
    return(1);
  }
  RDBG("smbc: RindexFileExistAnyRecords exit 0\n");
  return(0);
}

int 
RindexFileGetCount(int RfileDesc)
{
  int Rt = 0;
  TTfind *TfindRecord = NULL;
  
  // read counter
  RindexFileReadCounter(RfileDesc);

  while (RindexFileReadOneRecord(RfileDesc, &TfindRecord)) 
  {
    Rt++;
    RindexFreeOneTTfind(&TfindRecord);
  }
  RDBG("find-index-manage: RindexFileSetSeek return ");
  RDBGi(Rt);
  RDBG(" records\n");
  return(Rt);
}

int 
RindexFileTruncate(int RfileDesc, long int Rnr)
{
  off_t Rseek;

  if (!RindexFileSetSeek(RfileDesc, Rnr)) 
    return(0);
  Rseek = lseek(RfileDesc, 0, SEEK_CUR);
  if (ftruncate(RfileDesc, Rseek) == 0) 
    return(1);
  return(0);
}

int
RindexFileReadCounter(int RfileDesc)
{
  int Rreturn;

  lseek(RfileDesc, 0, SEEK_SET);
  RreadUnsignedInt(RfileDesc, &Rreturn);
  return(Rreturn);
}
//--------------------------------------------------------------------------------

int 
RcreateNewRecordInIndexTable(TTindexTable **Ridx, int Rnr)
{
  RDBG("find-index-manage: RcreateNewRecordInIndexTable start, try mark "); 
  RDBGi(Rnr); RDBG(" record\n");
  if (*Ridx) 
  {
    (*Ridx)->next = Rmalloc(sizeof(TTindexTable));
    (*Ridx)->next->previous = (*Ridx);
    (*Ridx) = (*Ridx)->next;
  }
  else 
  {
    (*Ridx) = Rmalloc(sizeof(TTindexTable));
    (*Ridx)->previous = NULL;
  }
  (*Ridx)->next = NULL;
  (*Ridx)->number = Rnr;
  RDBG("find-index-manage: RcreateNewRecordInIndexTable finished\n");
  return(1);
}

int 
RfreeAllRecordFromIndexTable(TTindexTable **Ridx)
{
  TTindexTable *RidxTmp;

  if (!RgotoFirstIndexTable(Ridx))
    return(1);
  while (*Ridx) 
  {
    RidxTmp = (*Ridx);
    (*Ridx) = (*Ridx)->next;
    Rfree(RidxTmp);
  }
  return(1);
}

int 
RgotoFirstIndexTable(TTindexTable **Ridx)
{
  if (!(*Ridx))
    return(0);
  while ((*Ridx)->previous)
    (*Ridx) = (*Ridx)->previous;
  return(1);
}

int 
RgotoNIndextTable(TTindexTable **Ridx, int Rnr)
{
  RDBG("find-index-manage: RgotoNIndextTable start\n");
  if (Rnr < 1) 
  {
    RDBG("find-index-manage: RgotoNIndextTable return failed : Rnr < 1\n");
    return(0);
  }
  if (!RgotoFirstIndexTable(Ridx)) 
  {
    RDBG("find-index-manage: RgotoNIndextTable return failed : !RgotoFirstIndexTable(Ridx)\n");
    return(0);
  }
  if (RgetCountRecordIndexTable(*Ridx) < Rnr) 
  {
    RDBG("find-index-manage: RgotoNIndextTable return failed : RgetCountRecordIndexTable(*Ridx) < Rnr\n");
    return(0);
  }
  while (--Rnr) 
  {
    (*Ridx) = (*Ridx)->next;
  }
  RDBG("find-index-manage: RgotoNIndextTable return ok\n");
  return(1);
}

int 
RgetCountRecordIndexTable(TTindexTable *Ridx)
{
  int Rnr = 0;

  if (RgotoFirstIndexTable(&Ridx)) 
  {
    while (Ridx) 
    {
      Ridx = Ridx->next;
      Rnr++;
    }
  }
  return(Rnr);
}

int 
RgotoNextIndexTable(TTindexTable **Ridx)
{
  if (!(*Ridx))
    return(0);
  if (!(*Ridx)->next)
    return(0);
  (*Ridx) = (*Ridx)->next;
  return(1);
}

//--------------------------------------------------------------------------------

int 
RloadIndexFile(int Rfd, TTfind **RidxNet, int count, WINDOW *Rwin)
{
  TTfind *Rfi = NULL;
  int RyIF = 0;
  int RxIF = 0;
  int RcountI = count;
  char *RstrIF = NULL;

  if (!RindexFileSetSeek(Rfd, 1)) 
  {
    return(0);
  }

  if (Rwin)
    getmaxyx(Rwin, RyIF, RxIF);
  while (RindexFileReadOneRecord(Rfd, &Rfi)) 
  {
    if (*RidxNet) 
    {
      (*RidxNet)->next = Rmalloc(sizeof(TTfind));
      (*RidxNet)->next->previous = *RidxNet;
      *RidxNet = (*RidxNet)->next;
    }
    else 
    {
      *RidxNet = Rmalloc(sizeof(TTfind));
      (*RidxNet)->previous = NULL;
    }
    if (Rwin)
    {
      RcountI--;
      RstrdupN(RstrIF, "");
      Rintaddtoend(&RstrIF, trunc(((double)(count-RcountI)/(double)count)*100));
      Rstraddtoend(&RstrIF, "%");
      mvwhline(Rwin, 4, 1, 32, RxIF-2);
      Rmvwprintw(Rwin, 4, RxIF/2-Rstrlen(RstrIF)/2, RstrIF);
      wrefresh(Rwin);
    }
    (*RidxNet)->group = Rstrdup(Rfi->group);
    (*RidxNet)->host = Rstrdup(Rfi->host);
    (*RidxNet)->dir = Rstrdup(Rfi->dir);
    (*RidxNet)->file = Rstrdup(Rfi->file);
    (*RidxNet)->access_denied = Rstrdup(Rfi->access_denied);

    (*RidxNet)->next = NULL;
    RindexFreeOneTTfind(&Rfi);
  }
  Rfree(RstrIF);
  return(1);
}

void 
RfreeIndexFile(TTfind **RidxNet)
{
  TTfind *Rtf;

  RDBG("find-index-manage: RfreeIndexFile start\n");
  if (RindexNetSetSeek(RidxNet, 1)) 
  {
    RDBG("find-index-manage: RfreeIndexFile 1\n");
    while (*RidxNet) 
    {
      Rtf = (*RidxNet);
      (*RidxNet) = (*RidxNet)->next;
      Rfree(Rtf->group);
      Rfree(Rtf->host);
      Rfree(Rtf->dir);
      Rfree(Rtf->file);
      RDBG("find-index-manage: RfreeIndexFile Rtf->access_denied = |"); RDBG(Rtf->access_denied); RDBG("|\n");
      Rfree(Rtf->access_denied);
      Rfree(Rtf);
    }
  }
}

int 
RindexNetSetSeek(TTfind **RidxNet, int Rnr)
{
  RDBG("find-index-manage: RindexNetSetSeek start\n");
  if (!(*RidxNet)) 
  {
    RDBG("find-index-manage: RindexNetSetSeek return failed 1\n");
    return(0);
  }
  while ((*RidxNet)->previous) 
    (*RidxNet) = (*RidxNet)->previous;
  while (--Rnr > 0) 
  {
    if ((*RidxNet)->next)
      (*RidxNet) = (*RidxNet)->next;
    else 
    {
      RDBG("find-index-manage: RindexNetSetSeek return failed 2\n");
      return(0);
    }
  }
  RDBG("find-index-manage: RindexNetSetSeek return ok\n");
  return(1);
}

int 
RindexNetReadOneRecord(TTfind **RidxNet, TTfind **RidxNetDst)
{
  RDBG("find-index-manage: RindexNetReadOneRecord start\n");
  if (!(*RidxNet)) 
  {
    RDBG("find-index-manage: RindexNetReadOneRecord return failed\n");
    return(0);
  }
  *RidxNetDst = Rmalloc(sizeof(TTfind));
  (*RidxNetDst)->group = Rstrdup((*RidxNet)->group);
  (*RidxNetDst)->host = Rstrdup((*RidxNet)->host);
  (*RidxNetDst)->dir = Rstrdup((*RidxNet)->dir);
  (*RidxNetDst)->file = Rstrdup((*RidxNet)->file);
  (*RidxNetDst)->access_denied = Rstrdup((*RidxNet)->access_denied);
  (*RidxNetDst)->next = NULL;
  (*RidxNetDst)->previous = NULL;

  (*RidxNet) = (*RidxNet)->next;
  RDBG("find-index-manage: RindexNetReadOneRecord return ok\n");
  return(1);
}

int 
RindexNetGoToNext(TTfind **RidxNet)
{
  if (!(*RidxNet)) 
  {
    return(0);
  }
  (*RidxNet) = (*RidxNet)->next;
  return(1);
}

void 
RindexFreeOneTTfind(TTfind **RidxNet)
{
  RDBG("find-index-manage: RindexFreeOneTTfind start\n");
  if (*RidxNet)
  {
    if ((*RidxNet)->group) Rfree((*RidxNet)->group);
    if ((*RidxNet)->host)  Rfree((*RidxNet)->host);
    if ((*RidxNet)->dir)   Rfree((*RidxNet)->dir);
    if ((*RidxNet)->file)  Rfree((*RidxNet)->file);
    RDBG("find-index-manage: (*RidxNet)->access_denied = |"); RDBG((*RidxNet)->access_denied); RDBG("|"); 
    RDBGi(Rstrlen((*RidxNet)->access_denied)); RDBG("|\n");

    if ((*RidxNet)->access_denied) Rfree((*RidxNet)->access_denied);
    RDBG("find-index-manage: RindexFreeOneTTfind 1\n");
    Rfree(*RidxNet);
    RDBG("find-index-manage: RindexFreeOneTTfind 2\n");
  }
  RDBG("find-index-manage: RindexFreeOneTTfind end\n");
}

//------------------------------------------------

int 
RAddHostToHostsTable(TThostsTable **RhostsTable, char *Rstr)
{
  TThostsTable *RhostsTableTym;

  RDBG("find-index-manage: RAddHostToHostsTable start, try add ");
  RDBG(Rstr); RDBG("\n");
  if (!(*RhostsTable)) 
  {
    (*RhostsTable) = Rmalloc(sizeof(TThostsTable));
    RhostsTableTym = *RhostsTable;
  }
  else {
    RhostsTableTym = *RhostsTable;
    while (RhostsTableTym->next) 
      RhostsTableTym = RhostsTableTym->next;
    RhostsTableTym->next = Rmalloc(sizeof(TThostsTable));
    RhostsTableTym = RhostsTableTym->next;
  }
  RhostsTableTym->next = NULL;
  RhostsTableTym->name = Rmalloc(strlen(Rstr)+1);
  memcpy(RhostsTableTym->name, Rstr, strlen(Rstr)+1);
  RDBG("find-index-manage: RAddHostToHostsTable return\n");
  return(1);
}

int 
RFreeHostsTable(TThostsTable **RhostsTable)
{
   TThostsTable *RhostsTableTym;

   while (*RhostsTable) 
   {
     RhostsTableTym = (*RhostsTable);
     (*RhostsTable) = (*RhostsTable)->next;
     Rfree(RhostsTableTym->name);
     Rfree(RhostsTableTym);
   }
   return(1);
}

int 
RFindHostInToHostsTable(TThostsTable *RhostsTable, char *Rstr)
{
  while (RhostsTable) 
  {
    if (RstrCmp(RhostsTable->name, Rstr)) 
      return(1);
    else 
      RhostsTable = RhostsTable->next;
  }
  return(0);
}

