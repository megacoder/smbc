/*
       dulist.c
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
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
#include <dirent.h>
#include <libsmbclient.h>
#include <unistd.h>
#include <stdlib.h>
#include "fnet.h"
#include "dulist.h"
#include "debug.h"
#include "rtypedef.h"
#include "convertm.h"
#include "downloadlist.h"

#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

int 
RstrCmpIncludeFirst(const char *srcBig, const char *dstSmall)
{
  int RsrcL, RsrcD, Ri;

  RsrcL = strlen(srcBig);
  RsrcD = strlen(dstSmall);
  if (RsrcL < RsrcD) 
    return 0;
  for (Ri =0; Ri<RsrcD; Ri++)
  { 
    if (srcBig[Ri] != dstSmall[Ri]) 
      return 0;
  } 
  return 1;
}

void 
RlistAllDownloadLists()
{
  char *Rdir = NULL; 
  DIR *Rdhandle;
  struct dirent *Rsdirent;
  int Rcount = 0;

  Rasprintf(&Rdir, getenv("HOME"));
  Rstraddtoend(&Rdir, "/.smbc/");

  Rdhandle = opendir(to_sys(Rdir));
  if (Rdhandle != NULL) 
  { 
    while ((Rsdirent = readdir(Rdhandle)) > 0) 
    {
      if (strlen(Rsdirent->d_name) >= 6) 
        if (RstrCmpIncludeFirst(Rsdirent->d_name, "dulist")) 
        {
          printf("%s : %s\n", Rsdirent->d_name+6, Rsdirent->d_name); 
          Rcount++;
        }       
    }
    closedir(Rdhandle);
  }
  Rfree(Rdir);
  printf(_("Founded %i lists\n"), Rcount);
}

void 
RflushAllDownloadLists()
{
  char *Rdir = NULL; 
  char *RdufileName = NULL; 
  DIR *Rdhandle;
  struct dirent *Rsdirent;
  char RfindF; 

  asprintf(&Rdir, "%s/.smbc/", getenv("HOME"));
  RfindF = 1;
  while (RfindF) {
    Rdhandle = opendir(to_sys(Rdir));
    if (Rdhandle != NULL) 
    { 
      RfindF = 0;
      while ((Rsdirent = readdir(Rdhandle)) > 0) 
      {
        if (strlen(Rsdirent->d_name) >= 6) 
          if (RstrCmpIncludeFirst(Rsdirent->d_name, "dulist")) 
          {
            asprintf(&RdufileName, "%s/.smbc/%s", getenv("HOME"),Rsdirent->d_name);
            printf(_("%s flushed\n"), Rsdirent->d_name); 
            unlink(to_sys(RdufileName));
            Rfree(RdufileName);
            RfindF = 1;
          }       
      }
    }
    else    
      RfindF = 0;
    closedir(Rdhandle);
  }
  Rfree(Rdir);
  printf(_("Flush done.\n"));
}

void 
RdisplayVersion()
{
  printf("Samba Commander %s\n", VERSION);

#ifdef HAVE_DEBUG
  printf("With debug mode.\n");
#endif /* HAVE_DEBUG */

#ifdef HAVE_SET_CONTEXT
  printf("With smbc_set_context.\n");
#endif /* HAVE_SET_CONTEXT */

#ifdef HAVE_SMBCCTX
  printf("With smbc_new_context.\n");
#endif /* HAVE_SMBCCTX */
}

void 
RShowAllItemsFromDUlist(int RnrDulist)
{
  char *RdufileName = NULL;
  int RdlsSize = sizeof(TTdownload);
  int Rfhandle;
  TTdownload *Rdls;
  char *RstrT;
  int Ri;

  asprintf(&RdufileName, "%s/.smbc/dulist%i", getenv("HOME"), RnrDulist);
  Rfhandle = open(to_sys(RdufileName), O_NONBLOCK | O_RDONLY, 0755);
  if (Rfhandle > 0) {
    printf("---------------------------------------------------------\n");
    printf(" 1 | 2 | 3 | 4 | 5 | 6 | 7\n\n");
    printf(_("1 - local path\n"));
    printf(_("2 - workgroup\n"));
    printf(_("3 - size\n"));
    printf(_("4 - percent download/upload\n"));
    printf(_("5 - 'X' - active, 'W' - wait, ' ' don't download/upload\n"));
    printf(_("6 - 'D' - download, 'U' - upload\n"));
    printf(_("7 - remote path\n"));
    printf("---------------------------------------------------------\n");
    Ri = 0;
    while (Ri != -1)
    {
      Rdls = RcreateEmptyTTdownload();
      
      Ri = RreadStr(Rfhandle, &(Rdls->username));
      if (Ri != -1) Ri = RreadStr(Rfhandle, &(Rdls->password));
      if (Ri != -1) Ri = RreadStr(Rfhandle, &(Rdls->groupnameR));
      if (Ri != -1) Ri = RreadStr(Rfhandle, &(Rdls->pathRLbase));
      if (Ri != -1) Ri = RreadStr(Rfhandle, &(Rdls->pathR));
      if (Ri != -1) Ri = RreadStr(Rfhandle, &(Rdls->pathL));
      if (Ri != -1) Ri = RreadStr(Rfhandle, &(Rdls->fullfilename));
      if (Ri != -1) Ri = RreadStr(Rfhandle, &(Rdls->filename));
      if (Ri != -1) Ri = RreadUnsignedInt(Rfhandle, &(Rdls->sizeR));
      if (Ri != -1) Ri = RreadStr(Rfhandle, &(Rdls->sizeDisplayR));
      if (Ri != -1) Ri = RreadStr(Rfhandle, &(Rdls->sizeDisplayDownR));
      if (Ri != -1) Ri = RreadStr(Rfhandle, &(Rdls->percent));
      if (Ri != -1) Ri = RreadChar(Rfhandle, &(Rdls->active));
      if (Ri != -1) Ri = RreadChar(Rfhandle, &(Rdls->upload));
      if (Ri != -1) Ri = RreadTime_t(Rfhandle, &(Rdls->delayRetry));
      if (Ri != -1) Ri = RreadTime_t(Rfhandle, &(Rdls->nextRetry));

      if (Ri != -1)
      {
        Rstraddtoend(&RstrT, Rdls->pathL);
        Rstraddtoend(&RstrT, " | ");
        Rstraddtoend(&RstrT, Rdls->groupnameR);
        Rstraddtoend(&RstrT, " | ");
        Rstraddtoend(&RstrT, Rdls->sizeDisplayR);
        Rstraddtoend(&RstrT, " | ");
        Rstraddtoend(&RstrT, Rdls->percent);
        Rstraddtoend(&RstrT, " | ");
        Rcharaddtoend(&RstrT, Rdls->active);
        Rstraddtoend(&RstrT, " | ");
        Rcharaddtoend(&RstrT, Rdls->upload);
        Rstraddtoend(&RstrT, " | ");
        Rstraddtoend(&RstrT, Rdls->pathR);
        Rstraddtoend(&RstrT, "/");
        Rstraddtoend(&RstrT, Rdls->filename);
        Rstraddtoend(&RstrT, "\n");
        printf(to_scr(RstrT));
        Rfree(RstrT);
        RfreeOneTTdownloadAndGoToFirst(&Rdls);
      }
    }
    close(Rfhandle);
    realloc(Rdls, RdlsSize);
  }
  Rfree(RdufileName);
}

int 
RsaveMessage(char *Rstr, char *RfileName)
{
  char *Rfile;
  int Ri = 0;
  int Rf, Rfi;
  char Rfinding = 1;
  char Rc[1];

  if ((Rstr == NULL) || (RfileName == NULL))
    return(0);

  umask(0000);
  mkdir(RMESSAGEDIR, 0777);
  if ((Rfi = open(to_sys(RfileName), O_RDONLY)) > 0) 
  {
    while (Rfinding) 
    {
      Rasprintf(&Rfile, RMESSAGEDIR);
      Rcharaddtoend(&Rfile, '/');
      Rstraddtoend(&Rfile, RMESSAGEFILE);
      Rintaddtoend(&Rfile, Ri);
      if ((Rf = open(to_sys(Rfile), O_CREAT | O_EXCL | O_WRONLY, 0666)) > 0) 
      {
        Rfinding = 0;
        write(Rf, Rstr, strlen(Rstr));
        write(Rf, "\n", 1);
        while (read(Rfi, Rc, 1) > 0) 
          write(Rf, Rc, 1);
        close(Rf);
        close(Rfi);
      }
      else 
        Ri++;
      Rfree(Rfile);
    }
  }
  return(1);
}

void 
RreturnHumanDelayTime(char **RtimeText, time_t Rtime)
{
  #define RtimeCharTLen 20
  char RtimeCharT[RtimeCharTLen];

  RtimeCharT[0] = 0;
  if (Rtime/3600 > 0) 
  {
    itoa(RtimeCharT, RtimeCharTLen, Rtime/3600);
    memcpy(RtimeCharT+strlen(RtimeCharT), "h", 2);
    Rtime = Rtime % 3600; 
  }
  if (Rtime/60 > 0) 
  {
    itoa(RtimeCharT+strlen(RtimeCharT), RtimeCharTLen-strlen(RtimeCharT), Rtime/60);
    memcpy(RtimeCharT+strlen(RtimeCharT), "m", 2);
    Rtime = Rtime % 60;
  }
  if (Rtime > 0) 
  {
    itoa(RtimeCharT+strlen(RtimeCharT), RtimeCharTLen-strlen(RtimeCharT), Rtime); 
    memcpy(RtimeCharT+strlen(RtimeCharT), "s", 2);
  }
  asprintf(RtimeText, "%s", strlen(RtimeCharT)?RtimeCharT:"now"); 
}

