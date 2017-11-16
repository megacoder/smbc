/*
       Samba Commander 
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/
// -------------------------------------------------------------------
// if you have other GUEST accunt change this line
#define RGUEST "guest"
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

#include <sys/ioctl.h>
#include <libintl.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <libsmbclient.h>
#include <popt.h>
#include <unistd.h>
#include <stdlib.h>
#include "fnet.h"
#include "smb-pack.h"
#include "find-index-manage.h"
#include "debug.h"
#include "dulist.h"
#include "winpopup.h"
#include "bookmarks.h"
#include "convertm.h"
#include "ttlist.h"
#include "static_str.h"
#include "windows.h"
#include "downloadlist.h"
#include "smb-ext.h"

#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

#define RAstatusBar(a) RstatusBar(a, RwinStatus, RmainWinX, 0)
#define RAstatusBar2(a, b) RstatusBar(a, RwinStatus, RmainWinX, b)
#define RAstatusBarStr(a, b) RstatusBarStr(a, RwinStatus, RmainWinX, b)

#define RMAXGROUPS 1000
#define RMAXHOSTS 1000
#define RMAXLIST 1000
#define TLISTCOLLEN 200
#define RTFILE 0
#define RTDIR 1
#define RCalarm 1
// 5 - chars + 1 - \0
#define TfilesSizeShowLen 6

#define INFO_I_CANT_ENTER_TO 0
#define INFO_MINI_HELP 1
#define INFO_DO_YOU_WANT_DELETE 2
#define INFO_DO_YOU_WANT_QUIT_FROM_SMBC 3
#define INFO_SHARE_X_IS_EMPTY 19
#define INFO_GROUP_X_IS_EMPTY 20
#define INFO_HOST_X_IS_EMPTY 21
#define INFO_I_CANT_ENTER_TO_X_ACCESS_DENIED 22
#define INFO_DO_YOU_WANT_COPY_X_TO_Y 23
#define INFO_DOWNLOAD_LIST_IS_EMPTY 24
#define INFO_FILES_FROM_X_TO_Y_ARE_DIFFERENT 25
#define INFO_WARNING_CANT_X_Y_CONNECTION_BROKEN 26
#define INFO_PERMISSION_DENIED 27
#define INFO_INDEX_IS_EMPTY 28
#define INFO_PROBABLY_X_IS_FULL 29
#define INFO_NEW_MESSAGE_X 31
#define INFO_X 32
#define INFO_DIR_X_IS_EMPTY 33

#define WITH_STATUS_BAR 1
#define WITHOUT_STATUS_BAR 0

typedef enum {
  DO_REDRAW_MAIN_WINDOW = 0,
  DONT_REDRAW_MAIN_WINDOW = 1
} REDRAW_MAIN_WINDOW;

typedef enum {
  PANEL_LEFT  = 0,
  PANEL_RIGHT = 1
} PANEL_TYPE;

typedef enum {
  LEVEL_WORKGROUPS = 0,
  LEVEL_HOSTS      = 1,
  LEVEL_SHARES     = 2,
  LEVEL_FILES      = 3,
  LEVEL_LOCAL      = 10
} LEVEL_TYPE;

PANEL_TYPE Rselected = PANEL_RIGHT;
LEVEL_TYPE RremoteLevel = LEVEL_WORKGROUPS;

int Rdx;
int RmainWinX = 0;
int RmainWinY = 0;
int RmainWinXlistL = 0;
int RmainWinXlistR = 0;
int RmainWinYlist = 0;

WINDOW *RwinMain = NULL;
WINDOW *RwinLeft = NULL;
WINDOW *RwinRight = NULL;
WINDOW *RwinHead = NULL;
WINDOW *RwinStatus = NULL;
WINDOW *RwinLeftList = NULL;
WINDOW *RwinRightList = NULL;
WINDOW *RwinLeftHead = NULL;
WINDOW *RwinRightHead = NULL;
WINDOW *RwinLeftTop = NULL;
WINDOW *RwinRightTop = NULL;
WINDOW *RwinLeftCol1 = NULL;
WINDOW *RwinLeftCol2 = NULL;
WINDOW *RwinLeftCol3 = NULL;
WINDOW *RwinRightCol1 = NULL;
WINDOW *RwinRightCol2 = NULL;
WINDOW *RwinRightCol3 = NULL;
WINDOW *RwinLeftHeadCol1 = NULL;
WINDOW *RwinLeftHeadCol2 = NULL;
WINDOW *RwinLeftHeadCol3 = NULL;
WINDOW *RwinRightHeadCol1 = NULL;
WINDOW *RwinRightHeadCol2 = NULL;
WINDOW *RwinRightHeadCol3 = NULL;
WINDOW *RactiveWindow = NULL;


char *Rahost = NULL;
char *Radir = NULL;
char *RadirL = NULL;

char *RadirTmp = NULL;
char *RadirLtmp = NULL;
char *RadirOld = NULL;
char *RfileNamePos;
char *Rsmbclient = NULL;

int RgrpAct;
int RhostAct;
int RshareAct;
int RfilesAct;

int RxHalf;
int RleftPos = 0;
int RrightPos = 0;
int RleftFirst = 0;
int RrightFirst = 0;
int RdownloadHandleRD = -1;
int RdownloadHandleWR = -1;

#ifdef HAVE_SMBCCTX
SMBCCTX *Rstatcont = NULL;
#endif /* HAVE_SMBCCTX */

int Rtimeout = 3000;
time_t RavrTime = 0;
unsigned int RavrSize = 0;
unsigned int RavrSizeDisplay = 0;
long int RtimeR = 0;
char *RfileNameForFindList = NULL;
char *RfileNameForFindListPrim = NULL;
char *RfileNameMessage = NULL;
char *RfileNameMessagePrim = NULL;
int RindexTime = 300;
int RchildId = 0;
int RchildDUId = 0;
int RlockFile = 0;
int RlockFileFlag = 0;
int RstatusBarStatus = 1;
time_t RlastTimeFileMessage = 1;
int RaskRetry = 1;
int RaskRetryDiskFull = 1;
int RrunFirst = 1;
char *RsmbcLockFile = NULL;
int RcancelFinding = 0;
int RdoResize = 0;
int RdoResizeFind = 0;
int RdoResizeDownload = 0;
int RdoResizeBookmark = 0;
char RdontDownload = 0;
int Raskquit = 1;
int RdefaultDelayRetry = 60;
bool RactuallyNothingDownloadUpload = 0;
int Rerror = 0;
int RdontResizeWindow = 0;

char RcharSetFromPoFile[20];
char RcharSetOutput[20];
char RcharSetInternal[20];
char RcharSetFromToSystem[20];

int RsortReverseR = 0;  
int RgroupFilesDirectoriesR = 1;
int RsortMethodR = 1;   // 1 - name
                        // 2 - ext
                        // 3 - size

int RsortReverseL = 0;  
int RgroupFilesDirectoriesL = 1;
int RsortMethodL = 1;   // 1 - name
                        // 2 - ext
                        // 3 - size

int RretryReadGroups = 5;
int RretryReadHosts = 5;
int RretryReadShares = 5;

TTgroups *Rgroups = NULL;
TThosts *Rhosts = NULL;
TTshares *Rshares = NULL;
TTfiles *RfilesR = NULL;
TTfiles *RfilesL =NULL;
TTlist *RlistLeft = NULL;
TTlist *RlistRight = NULL;
TTdownload *RdownloadList = NULL;
TTdownload *RdownloadListAct = NULL;
TTcolor Rcolor[3];
TText *Rext = NULL; 
TTposition *RpositionLeft = NULL;
TTposition *RpositionRight = NULL;
int RlistLeftPos;
int RlistRightPos;

TTlist *RlistTmainLeft = NULL;
TTlist *RlistTmainRight = NULL;

void RwinRedraw();
void RwriteMenuTxt(int Rfirst, int Rwhat, PANEL_TYPE Rcol, int Rselect, TTlist *RlistI);
void RopenSaveDownloadList(int Rnr);
void RcreateFindListIndex(int Isave, char *Igroup, char *Ihost, char *Ipath, char *Idir, WINDOW *RwinI, int RfileFd);
void RdoWindowResize();
void RwinRedrawWithData();
void RcompareTwoIndexFile(int RfdNew, int RfdOld, int RfdMessage);
int RtryDownloadUpload();
void RtryLockFileAndCreateIndexFile();
void RsigAlarm();
void RsigTerm();
void RsigSEGV();
char *RgetSmallPath(char *Rhost);

//-------------------------------------------------------------

void 
RpressEnter(WINDOW *Rwin)
{
  keypad(Rwin, TRUE);  
  while (wgetch(Rwin) != 10) {};
}

int 
RdetectScreenToSmall()
{
  int Rys, Rxs;
  WINDOW *RwinMsg = NULL;
  
  if ((RmainWinX < 80) || (RmainWinY < 25)) 
  {
     Rys = 4; Rxs = 30;
     RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
     RmvwprintwCenter(RwinMsg, 1, Rm1);
     RmvwprintwCenter(RwinMsg, 2, Rm2);
     box(RwinMsg,0,0);
     wrefresh(RwinMsg);
     RpressEnter(RwinMsg);
     Rdelwin(RwinMsg);
     return(1);
  }
  return(0);
}

int 
RshowMessage(int RnrMsg, char *Rmsg, char *Rmsg2, TTdownload *RTd, REDRAW_MAIN_WINDOW Rredraw_MainWin)
{
  int Rii, Rys, Rxs;
  WINDOW *RwinMsg = NULL;
  WINDOW *RwinMsg2 = NULL;
  char RmsgTmp[1000];
  int Rt1, Rt2, Rkey;
  char Rmsg1[20];
  char *RmsgT1 = NULL;
  
  int Rxm, Rym;
  char *Rstmp = NULL;
  char *Rms = NULL;
  int Rtt, Rtt2;
  int Rt = 2;
  char *RtimeChar = NULL;
  char *RtimeCharT = NULL;

  RDBG("smbc : RshowMessage start\n");

  RdontResizeWindow = 1;

/*
 * Rpaint 
 */
 void Rpaint(int Ri, int Rnr, int Ry) 
 {
   int Rall;

   if (Rnr == 0) 
   { 
     Rall = Rstrlen(Rm136)+Rstrlen(Rm137);
     switch (Ri) 
     {
       case 1 : Rii = (Rxs-Rall)/2;                 
                  wattron(RwinMsg, COLOR_PAIR(1)); Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm136); wattroff(RwinMsg, COLOR_PAIR(1)); 
                Rii = (Rxs-Rall)/2+Rstrlen(Rm136)+1; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm137);
                break;
       case 0 : Rii = (Rxs-Rall)/2;                 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm136);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm136)+1; 
                  wattron(RwinMsg, COLOR_PAIR(1)); Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm137); wattroff(RwinMsg, COLOR_PAIR(1)); 
                break;
     }
   } 
   else 
   if (Rnr == 1) 
   {
     Rt = 3;
     Rall = Rstrlen(Rm25)+Rstrlen(Rm26)+Rstrlen(Rm27)+Rstrlen(Rm137)+3;
     RreturnHumanDelayTime(&RtimeCharT, RTd->delayRetry);
     RstrdupN(RtimeChar, Rm30);
     Rstraddtoend(&RtimeChar, RtimeCharT);
     Rfree(RtimeCharT);
     switch (Ri) 
     {
       case 5 : Rii = (Rxs-Rall)/2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm25);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+1; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm26);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm27); 
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+Rstrlen(Rm27)+3; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm137); 
                Rii = (Rxs)/2-Rstrlen(Rm28)/2; 
                  Rmvwprintw(RwinMsg, Ry+1, 1+Rii, Rm28); 
                mvwhline(RwinMsg, Ry+2, 1, ' ', Rxs-2);
                Rii = (Rxs)/2-Rstrlen(RtimeChar)/2; 
                  wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Ry+2, 1+Rii, RtimeChar); 
                  wattroff(RwinMsg, COLOR_PAIR(1));
                break;
       case 4 : Rii = (Rxs-Rall)/2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm25);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+1; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm26);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm27); 
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+Rstrlen(Rm27)+3; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm137); 
                Rii = (Rxs)/2-Rstrlen(Rm28)/2; 
                  wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Ry+1, 1+Rii, Rm28); 
                  wattroff(RwinMsg, COLOR_PAIR(1)); 
                Rii = (Rxs)/2-Rstrlen(RtimeChar)/2; 
                  Rmvwprintw(RwinMsg, Ry+2, 1+Rii, RtimeChar);
                break;
       case 3 : Rii = (Rxs-Rall)/2; 
                  wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm25); 
                  wattroff(RwinMsg, COLOR_PAIR(1));
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+1; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm26);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm27);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+Rstrlen(Rm27)+3; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm137);
                Rii = (Rxs)/2-Rstrlen(Rm28)/2; 
                  Rmvwprintw(RwinMsg, Ry+1, 1+Rii, Rm28);
                Rii = (Rxs)/2-Rstrlen(RtimeChar)/2; 
                  Rmvwprintw(RwinMsg, Ry+2, 1+Rii, RtimeChar);
                break;
       case 2 : Rii = (Rxs-Rall)/2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm25);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+1; 
                  wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm26); 
                  wattroff(RwinMsg, COLOR_PAIR(1)); 
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm27);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+Rstrlen(Rm27)+3; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm137);
                Rii = (Rxs)/2-Rstrlen(Rm28)/2; 
                  Rmvwprintw(RwinMsg, Ry+1, 1+Rii, Rm28);
                Rii = (Rxs)/2-Rstrlen(RtimeChar)/2; 
                  Rmvwprintw(RwinMsg, Ry+2, 1+Rii, RtimeChar);
                break;
       case 1 : Rii = (Rxs-Rall)/2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm25);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+1; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm26);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+2; 
                  wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm27); 
                  wattroff(RwinMsg, COLOR_PAIR(1)); 
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+Rstrlen(Rm27)+3; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm137);
                Rii = (Rxs)/2-Rstrlen(Rm28)/2; 
                  Rmvwprintw(RwinMsg, Ry+1, 1+Rii, Rm28);
                Rii = (Rxs)/2-Rstrlen(RtimeChar)/2; 
                  Rmvwprintw(RwinMsg, Ry+2, 1+Rii, RtimeChar);
                break;
       case 0 : Rii = (Rxs-Rall)/2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm25);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+1; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm26);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm27); 
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+Rstrlen(Rm27)+3; 
                  wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm137); 
                  wattroff(RwinMsg, COLOR_PAIR(1)); 
                Rii = (Rxs)/2-Rstrlen(Rm28)/2; 
                  Rmvwprintw(RwinMsg, Ry+1, 1+Rii, Rm28);
                Rii = (Rxs)/2-Rstrlen(RtimeChar)/2; 
                  Rmvwprintw(RwinMsg, Ry+2, 1+Rii, RtimeChar);
                break;
     }
     Rfree(RtimeChar);
   } 
   else
   if (Rnr == 2) 
   {
     Rt = 2;
     Rall = Rstrlen(Rm25)+Rstrlen(Rm26)+Rstrlen(Rm29)+Rstrlen(Rm137);
     switch (Ri) 
     {
       case 3 : Rii = (Rxs-Rall)/2; 
                  wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm25); 
                  wattroff(RwinMsg, COLOR_PAIR(1)); 
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+1; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm26);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+2;  
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm29);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+Rstrlen(Rm29)+3; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm137);
                break;
       case 2 : Rii = (Rxs-Rall)/2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm25);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+1; 
                  wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm26); 
                  wattroff(RwinMsg, COLOR_PAIR(1)); 
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm29);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+Rstrlen(Rm29)+3; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm137);
                break;
       case 1 : Rii = (Rxs-Rall)/2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm25);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+1; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm26);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+2; 
                  wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm29); 
                  wattroff(RwinMsg, COLOR_PAIR(1)); 
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+Rstrlen(Rm29)+3; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm137);
                break;
       case 0 : Rii = (Rxs-Rall)/2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm25);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+1; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm26);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+2; 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm29); 
                Rii = (Rxs-Rall)/2+Rstrlen(Rm25)+Rstrlen(Rm26)+Rstrlen(Rm29)+3; 
                  wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Ry, 1+Rii, Rm137); 
                  wattroff(RwinMsg, COLOR_PAIR(1)); 
                break;
     }
   } 
  }

/*
 * END Rpaint 
 */
  switch(RnrMsg) 
  {
    case INFO_I_CANT_ENTER_TO: 
            /*
                I can't enter to
                Rmsg
            */
            Rys = 8; Rxs = 40;
            RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
            box(RwinMsg,0,0);
            Rii = Rxs/2-Rstrlen(Rm41)/2; 
              Rmvwprintw(RwinMsg, 2, 1+Rii, Rm41);
            Rii = Rxs/2-Rstrlen(Rmsg)/2; 
              Rmvwprintw(RwinMsg, 3, 1+Rii, Rmsg);
            Rii = Rxs/2-Rstrlen(Rm136)/2; 
              wattron(RwinMsg, COLOR_PAIR(1)); 
              Rmvwprintw(RwinMsg, 5, 1+Rii, Rm136); 
              wattroff(RwinMsg, COLOR_PAIR(1)); 
            wrefresh(RwinMsg);
            RpressEnter(RwinMsg);
            Rdelwin(RwinMsg);
            break;
    case INFO_MINI_HELP: 
            Rys = 25; Rxs = 60;
            Rtt = 1; Rtt2 = 2;
            RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
            RmvwprintwCenter(RwinMsg, Rtt++, "Samba Commander");
            RmvwprintwCenter(RwinMsg, Rtt++, "http://smbc.airm.net");
            Rtt+=1;
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm90);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm91);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm92);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm93);
            Rtt+=1;
            Rtt2 = 7;
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm94);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm95);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm96);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm97);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm98);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm99);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm100);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm101);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm102);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm103);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm104);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm105);
            Rtt+=1;
            Rtt2 = 2;
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm106);
            Rmvwprintw(RwinMsg, Rtt++, Rtt2, Rm107);
            box(RwinMsg,0,0);
            wrefresh(RwinMsg);
            keypad(RwinMsg, TRUE); 
            nodelay(RwinMsg,1);
            RactiveWindow = RwinMsg;
            RDBG("smbc : RshowMessage RactiveWindow = RwinMsg  1\n");
            while (wgetch(RwinMsg) != 10) {
              RtryDownloadUpload();
            }
            Rdelwin(RwinMsg);
            break;
    case INFO_DO_YOU_WANT_DELETE: 
            /*
               Do you want delete
               RmsgT1
            */
            Rys = 10; Rxs = 60;
            RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
            box(RwinMsg, 0, 0);
            Rii = Rxs/2-Rstrlen(Rm42)/2; 
               Rmvwprintw(RwinMsg, 2, 1+Rii, Rm42);
            RmsgT1 = to_scr(Rmsg);
            
            Rii = Rstrlen(RmsgT1);
            if (Rii > Rxs-4)
              RmsgT1[Rxs-4] = 0;

            Rii = Rxs/2-Rstrlen(RmsgT1)/2; 
              Rmvwprintw(RwinMsg, 3, Rii, RmsgT1);
            Rt1 = 0; 
              Rpaint(Rt1,0,7);
            nodelay(RwinMsg, 1);
            RactiveWindow = RwinMsg;
            RDBG("smbc : RshowMessage RactiveWindow = RwinMsg  2\n");
            while ((Rkey = mvwgetch(RwinMsg, 1,1))) {
               RtryDownloadUpload();
               if (Rkey == 'i' || Rkey == 9 || Rkey == 67 || Rkey == 68) 
               { 
                  Rt1 = !Rt1; 
                  Rpaint(Rt1,0,7); 
               }
               if (Rkey == 10 || Rkey == 'x') 
               {
                 wrefresh(RwinMsg);
                 Rdelwin(RwinMsg);
                 RwinRedrawWithData();
                 RdontResizeWindow = 0;
                 if (Rt1) 
                   return(1);
                 else 
                   return(0);
               }
            }
    case INFO_DO_YOU_WANT_QUIT_FROM_SMBC:
            /*
                Do you want quit from smbc?
            */
            Rys = 8; Rxs = 60;
            RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
            box(RwinMsg,0,0);
            Rii = Rxs/2-Rstrlen(Rm51)/2; Rmvwprintw(RwinMsg, 2, 1+Rii, Rm51);
            Rt1 = 1; Rpaint(Rt1,0,5);
            nodelay(RwinMsg, 1);
            RactiveWindow = RwinMsg;
            while ((Rkey = mvwgetch(RwinMsg, 1,1))) 
            {
               RtryDownloadUpload();
               if (Rkey == 'i' || Rkey == 9 || Rkey == 67 || Rkey == 68) 
               { 
                 Rt1 = !Rt1; 
                 Rpaint(Rt1,0,5); 
               }
               if (Rkey == 10 || Rkey == 'x') 
               {
                 wrefresh(RwinMsg);
                 Rdelwin(RwinMsg);
                 RwinRedrawWithData();
                 RdontResizeWindow = 0;
                 if (Rt1) 
                   return(1);
                 else 
                   return(0);
               }
            }
    case INFO_I_CANT_ENTER_TO_X_ACCESS_DENIED: /*
                 I can't enter to
                 RmsgT1
                 Access denied.
             */
             Rys = 9; Rxs = 60;
             RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
             box(RwinMsg, 0, 0);
             Rii = Rxs/2-Rstrlen(Rm41)/2; 
               Rmvwprintw(RwinMsg, 2, 1+Rii, Rm41);
             RmsgT1 = to_scr(Rmsg);
             Rii = Rxs/2-Rstrlen(RmsgT1)/2; 
               Rmvwprintw(RwinMsg, 3, 1+Rii, RmsgT1);
             Rii = Rxs/2-Rstrlen(Rm45)/2; 
               Rmvwprintw(RwinMsg, 4, 1+Rii, Rm45);
             Rii = Rxs/2-Rstrlen(Rm136)/2; 
               wattron(RwinMsg, COLOR_PAIR(1)); 
               Rmvwprintw(RwinMsg, 6, 1+Rii, Rm136); 
               wattroff(RwinMsg, COLOR_PAIR(1)); 
             wrefresh(RwinMsg);
             RpressEnter(RwinMsg);
             Rdelwin(RwinMsg);
             break;
    case INFO_DO_YOU_WANT_COPY_X_TO_Y: 
             /*
                Do you want copy
                Rmsg
                to
                Rmsg2
             */
             Rys = 10; Rxs = 60;
             RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
             box(RwinMsg,0,0);
             Rii = Rxs/2-Rstrlen(Rm46)/2; 
               Rmvwprintw(RwinMsg, 2, Rii, Rm46);
  
             RmsgT1 = to_scr(Rmsg);
             Rii = Rstrlen(RmsgT1);
             if (Rii > Rxs-4)
               RmsgT1[Rxs-4] = 0;
  
             Rii = Rxs/2-Rstrlen(RmsgT1)/2; 
               Rmvwprintw(RwinMsg, 3, Rii, RmsgT1);
             Rii = Rxs/2-Rstrlen(Rm113)/2; 
               Rmvwprintw(RwinMsg, 4, Rii, Rm113);
  
             RmsgT1 = to_scr(Rmsg2);
             Rii = Rstrlen(RmsgT1);
             if (Rii > Rxs-4)
               RmsgT1[Rxs-4] = 0;
  
             Rii = Rxs/2-Rstrlen(RmsgT1)/2; 
               Rmvwprintw(RwinMsg, 5, Rii, RmsgT1);
  
             Rt1 = 1; Rpaint(Rt1,0,7);
             while ((Rkey = mvwgetch(RwinMsg, 1,1))) 
             {
                if (Rkey == 'i' || Rkey == 9 || Rkey == 67 || Rkey == 68) 
                { 
                  Rt1 = !Rt1; 
                  Rpaint(Rt1,0,7); 
                }
                if (Rkey == 10 || Rkey == 'x') 
                {
                  wrefresh(RwinMsg);
                  Rdelwin(RwinMsg);
                  RwinRedrawWithData();
                  RdontResizeWindow = 0;
                  if (Rt1) 
                    return(1);
                  else 
                    return(0);
                }
             }
             break;
    case INFO_DOWNLOAD_LIST_IS_EMPTY: 
             /*
                Download list is empty.
             */
             Rys = 7; Rxs = 60;
             RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
             box(RwinMsg,0,0);
             Rii = Rxs/2-Rstrlen(Rm40)/2; 
               Rmvwprintw(RwinMsg, 2, 1+Rii, Rm40);
             Rii = Rxs/2-Rstrlen(Rm136)/2;
               wattron(RwinMsg, COLOR_PAIR(1)); 
               Rmvwprintw(RwinMsg, 4, 1+Rii, Rm136);
               wattroff(RwinMsg, COLOR_PAIR(1)); 
             wrefresh(RwinMsg);
             RpressEnter(RwinMsg);
             Rdelwin(RwinMsg);
             break;
    case INFO_FILES_FROM_X_TO_Y_ARE_DIFFERENT: 
             /*
                Warning
                Files
                RTd->filename
                from
                RTd->pathR
                to
                RTd->pathL
                are different or connection broken.
                Click on Wait to download letter.
             */
             Rys = 17; Rxs = 0; 
             getmaxyx(RwinMain, Rym, Rxm); 
             Rt = Rstrlen(RTd->filename)+6; 
             if (Rt > Rxm) 
               Rxs = Rxm; 
             else 
               Rxs = Rt;
             Rt = Rstrlen(RTd->pathR)+6; 
             if (Rt > Rxs) 
             {
               if (Rt > Rxm) 
                 Rxs = Rxm;
               else 
                 Rxs = Rt;
             }
             Rt = Rstrlen(RTd->pathL)+6; 
             if (Rt > Rxs) 
             {
               if (Rt > Rxm) 
                 Rxs = Rxm;
               else 
                 Rxs = Rt;
             }
             if (Rxs < 50) Rxs = 50;
             RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
             keypad(RwinMsg, TRUE); 
             box(RwinMsg,0,0);
             Rii = Rxs/2-Rstrlen(Rm116)/2; 
               Rmvwprintw(RwinMsg, 2, Rii, Rm116);
             Rii = Rxs/2-Rstrlen(Rm111)/2; 
               Rmvwprintw(RwinMsg, 4, Rii, Rm111);
             Rii = Rxs/2-Rstrlen(RTd->filename)/2; 
               Rmvwprintw(RwinMsg, 5, Rii, RTd->filename);
             Rii = Rxs/2-Rstrlen(Rm88)/2; 
               Rmvwprintw(RwinMsg, 6, Rii, Rm88);
             Rii = Rxs/2-Rstrlen(RTd->pathR)/2; 
               Rmvwprintw(RwinMsg, 7, Rii, RTd->pathR);
             Rii = Rxs/2-Rstrlen(Rm113)/2; 
               Rmvwprintw(RwinMsg, 8, Rii, Rm113);
             Rii = Rxs/2-Rstrlen(RTd->pathL)/2; 
               Rmvwprintw(RwinMsg, 9, Rii, RTd->pathL);
             Rii = Rxs/2-Rstrlen(Rm114)/2; 
               Rmvwprintw(RwinMsg, 10, Rii, Rm114);
             Rii = Rxs/2-Rstrlen(Rm115)/2; 
               Rmvwprintw(RwinMsg, 11, Rii, Rm115);
             Rt1 = 0; Rpaint(Rt1,1,Rys-4);
             wrefresh(RwinMsg);
             while ((Rkey = mvwgetch(RwinMsg, 1,1))) 
             {
               if (Rkey == 260) 
               {
                 if (Rt1 < 5) 
                   Rt1++; 
                 else 
                   Rt1 = 0;
                 Rpaint(Rt1,1,Rys-4); 
               } 
               if (Rkey == 'i' || Rkey == 261 || Rkey == 9) 
               { 
                 if (Rt1 > 0) 
                   Rt1--;
                 else 
                   (Rt1 = 5);
                 Rpaint(Rt1,1,Rys-4); 
               }
               if (Rt1 == 5) 
               {
                 if ((Rkey == ',') || (Rkey == 258)) 
                 {
                   if (RTd->delayRetry > 0) 
                     RTd->delayRetry--;
                   Rpaint(Rt1,1,Rys-4);
                 }
                 if ((Rkey == '.') || (Rkey == 259)) 
                 {
                   RTd->delayRetry++;
                   Rpaint(Rt1,1,Rys-4);
                 }
                 if ((Rkey == '<') || (Rkey == 66)) 
                 {
                   if (RTd->delayRetry > 60)
                     RTd->delayRetry = RTd->delayRetry-60;
                   else
                     RTd->delayRetry=0;
                   Rpaint(Rt1,1,Rys-4);
                 }
                 if ((Rkey == '>') || (Rkey == 65)) 
                 {
                   RTd->delayRetry = RTd->delayRetry+60;
                   Rpaint(Rt1,1,Rys-4);
                 }
               }
               if (Rkey == 10 || Rkey == 'x') 
               {
                 wrefresh(RwinMsg);
                 Rdelwin(RwinMsg);
                 RwinRedrawWithData();
                 RdontResizeWindow = 0;
                 return(Rt1);
               }
             }
             break;
    case INFO_WARNING_CANT_X_Y_CONNECTION_BROKEN: 
             /*
                 Warning
                 Can't Rmsg RTd->filename 
                 Connection broken.
             */
             Rys = 14; Rxs = 0; 
             getmaxyx(RwinMain, Rym, Rxm); 
             Rt = Rstrlen(to_scr(RTd->filename))+6; 
             if (Rt > Rxm) 
               Rxs = Rxm; 
             else 
               Rxs = Rt;
             Rt = Rstrlen(to_scr(RTd->pathR))+6; 
             if (Rt > Rxs) 
             {
               if (Rt > Rxm) 
                 Rxs = Rxm;
               else 
                 Rxs = Rt;
             }
             Rt = Rstrlen(to_scr(RTd->pathL))+6; 
             if (Rt > Rxs) 
             {
               if (Rt > Rxm) 
                 Rxs = Rxm;
               else 
                 Rxs = Rt;
             }
             if (Rxs < 50) 
               Rxs = 50;
             
             RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
             box(RwinMsg,0,0);
             Rii = Rxs/2-Rstrlen(Rm116)/2; 
               Rmvwprintw(RwinMsg, 2, Rii, Rm116);
             RstrdupN(Rstmp, Rm117); 
             Rstraddtoend(&Rstmp, Rmsg); 
               Rii = Rxs/2-Rstrlen(Rstmp)/2; 
               Rmvwprintw(RwinMsg, 4, Rii, Rstmp); 
               Rfree(Rstmp);

             Rms = to_scr(RTd->filename);
             if (Rstrlen(Rms)+4 > Rxs)
               Rms[Rxs-4] = 0;
             Rii = Rxs/2-Rstrlen(Rms)/2; 
               Rmvwprintw(RwinMsg, 5, Rii, Rms);

             Rii = Rxs/2-Rstrlen(Rm118)/2; 
               Rmvwprintw(RwinMsg, 6, Rii, Rm118);

             Rt1 = 0; Rpaint(Rt1,1,Rys-5);
             wrefresh(RwinMsg);
             keypad(RwinMsg, TRUE);
             while ((Rkey = mvwgetch(RwinMsg, 1,1))) 
             {
               if (Rkey == 260) 
               { 
                 if (Rt1 < 5) 
                   Rt1++; 
                 else 
                   Rt1 = 0;
                 Rpaint(Rt1,1,Rys-5); 
               } 
               if (Rkey == 261 || Rkey == 'i' || Rkey == 9) 
               {
                 if (Rt1 > 0) 
                   Rt1--;
                 else 
                   (Rt1 = 5);
                 Rpaint(Rt1,1,Rys-5); 
               }
               if (Rt1 == 5) 
               {
                 if ((Rkey == ',') || (Rkey == 258)) 
                 {
                   if (RTd->delayRetry > 0) 
                     RTd->delayRetry--;
                   Rpaint(Rt1,1,Rys-5);
                 }
                 if ((Rkey == '.') || (Rkey == 259)) 
                 {
                   RTd->delayRetry++;
                   Rpaint(Rt1,1,Rys-5);
                 }
                 if ((Rkey == '<') || (Rkey == 66)) 
                 {
                   if (RTd->delayRetry > 60)
                     RTd->delayRetry = RTd->delayRetry-60;
                   else
                     RTd->delayRetry=0;
                   Rpaint(Rt1,1,Rys-5);
                 }
                 if ((Rkey == '>') || (Rkey == 65)) 
                 {
                   RTd->delayRetry = RTd->delayRetry+60;
                   Rpaint(Rt1,1,Rys-5);
                 }
               }
               if (Rkey == 10 || Rkey == 'x') 
               {
                 RTd->nextRetry = time(NULL)+RTd->delayRetry;
                 wrefresh(RwinMsg);
                 Rdelwin(RwinMsg);
                 RwinRedrawWithData();
                 RdontResizeWindow = 0;
                 return(Rt1);
               }
             }
             break;
    case INFO_PERMISSION_DENIED: /*
                 Permission denied to
                 RmsgT1
             */
             Rys = 9; Rxs = 40;
             RmsgT1 = to_scr(Rmsg);
             if (Rstrlen(RmsgT1)+2 > Rxs) 
               Rxs = Rstrlen(RmsgT1)+4;
             RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
             box(RwinMsg, 0, 0);
             Rii = Rxs/2-Rstrlen(Rm55)/2; 
               Rmvwprintw(RwinMsg, 2, Rii, Rm55);
             Rii = Rxs/2-Rstrlen(RmsgT1)/2; 
               Rmvwprintw(RwinMsg, 3, Rii, RmsgT1);
             Rii = Rxs/2-Rstrlen(Rm136)/2; 
               wattron(RwinMsg, COLOR_PAIR(1)); 
               Rmvwprintw(RwinMsg, 6, Rii, Rm136); 
               wattroff(RwinMsg, COLOR_PAIR(1)); 
             wrefresh(RwinMsg);
             RpressEnter(RwinMsg);
             Rdelwin(RwinMsg);
             break;
    case INFO_INDEX_IS_EMPTY: 
             /*
                 Index for find file now is empty, try letter.
             */
             Rys = 7; Rxs = 62;
             RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
             box(RwinMsg,0,0);
             Rii = Rxs/2-Rstrlen(Rm48)/2; 
               Rmvwprintw(RwinMsg, 2, 1+Rii, Rm48);
             Rii = Rxs/2-Rstrlen(Rm136)/2;
               wattron(RwinMsg, COLOR_PAIR(1)); 
               Rmvwprintw(RwinMsg, 4, 1+Rii, Rm136);
               wattroff(RwinMsg, COLOR_PAIR(1)); 
             wrefresh(RwinMsg);
             nodelay(RwinMsg, 1);
             RactiveWindow = RwinMsg;
             RDBG("smbc : RshowMessage RactiveWindow = RwinMsg  4\n");
             while (wgetch(RwinMsg) != 10) 
             {
               RtryDownloadUpload();
             };
             Rdelwin(RwinMsg);
             break;
    case INFO_PROBABLY_X_IS_FULL: 
             /*
                 Probably RmsgT1 disk is full or quota exceeded.
             */
             Rys = 9; Rxs = 60;
             RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
             keypad(RwinMsg, TRUE);
             box(RwinMsg, 0, 0);
             RmsgT1 = to_scr(Rmsg);

             RstrdupN(Rstmp, Rm32); 
             Rstraddtoend(&Rstmp, RmsgT1); 
             Rstraddtoend(&Rstmp, Rm33); 

             Rii = Rxs/2-Rstrlen(Rstmp)/2; 
             Rmvwprintw(RwinMsg, 2, Rii, Rstmp); 
             Rfree(Rstmp);

             Rt1 = 3; 
               Rpaint(Rt1,1,Rys-4);
             wrefresh(RwinMsg);
             while ((Rkey = mvwgetch(RwinMsg, 1,1))) 
             {
               if (Rkey == 260) { 
                 if (Rt1 < 4) Rt1++; 
                 else Rt1 = 0;
                 Rpaint(Rt1,1,Rys-4); 
               } 
               if (Rkey == 261 || Rkey == 'i' || Rkey == 9) 
               {
                 if (Rt1 > 0) Rt1--;
                 else (Rt1 = 4);
                 Rpaint(Rt1,1,Rys-4); 
               }
               if (Rkey == 10 || Rkey == 'x') 
               {
                 wrefresh(RwinMsg);
                 Rdelwin(RwinMsg);
                 RwinRedrawWithData();
                 RdontResizeWindow = 0;
                 return(Rt1);
               }
             }
             break;
    case INFO_NEW_MESSAGE_X: 
             /*
                 New message Rmsg
             */
             Rys = 21; Rxs = 72;
             RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
             RwinMsg2 = newwin(Rys-4, Rxs-2, RmainWinY/2-Rys/2+1, RmainWinX/2-Rxs/2+1);
             keypad(RwinMsg, TRUE);
             box(RwinMsg,0,0);
             Rii = Rxs/2-Rstrlen(Rm50)/2; 
               Rmvwprintw(RwinMsg2, 1, Rii, Rm50);
             Rt2 = Rstrlen(Rmsg);
             Rms = Rmsg;
             Rym = 4; Rxm = 2;
             for (Rt1 = 0; Rt1 < Rt2; Rt1++) 
             {
               mvwprintw(RwinMsg2, Rym, Rxm++, "%c", Rms[Rt1]);
               if (Rxm == Rxs-2) 
               {
                 Rxm = 2;
                 Rym++;
               }
             }

             Rt2 = Rstrlen(Rmsg2);
             Rms = Rmsg2;
             Rym = Rym+3; Rxm = 2;
             for (Rt1 = 0; Rt1 < Rt2; Rt1++) 
             {
               mvwprintw(RwinMsg2, Rym, Rxm++, "%c", Rms[Rt1]);
               if (Rxm == Rxs-2) 
               {
                 Rxm = 2;
                 Rym++;
               }
               if (Rym == Rys-5)
                 break;
             }

             wattron(RwinMsg, COLOR_PAIR(1)); 
               Rmvwprintw(RwinMsg, Rys-3, Rxs/2-3, Rm136); 
               wattroff(RwinMsg, COLOR_PAIR(1)); 
             wrefresh(RwinMsg);
             wrefresh(RwinMsg2);
             RpressEnter(RwinMsg);
             Rdelwin(RwinMsg2);
             Rdelwin(RwinMsg);
             break;
    case INFO_X: 
             /*
                 Rmsg
                 
             */
             Rys = 7; Rxs = 62;
             RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
             box(RwinMsg, 0, 0);
             RmsgT1 = to_scr(Rmsg);
             Rii = Rxs/2-Rstrlen(RmsgT1)/2; 
               Rmvwprintw(RwinMsg, 2, 1+Rii, RmsgT1);
             Rii = Rxs/2-Rstrlen(Rm136)/2;
             wattron(RwinMsg, COLOR_PAIR(1)); 
             Rmvwprintw(RwinMsg, 4, 1+Rii, Rm136);
             wattroff(RwinMsg, COLOR_PAIR(1)); 
             wrefresh(RwinMsg);
             RactiveWindow = RwinMsg;
             RDBG("smbc : RshowMessage RactiveWindow = RwinMsg  4\n");
             nodelay(RwinMsg, 1);
             while (wgetch(RwinMsg) != 10) 
             {
               RtryDownloadUpload();
             };
             Rdelwin(RwinMsg);
             break;
    case INFO_GROUP_X_IS_EMPTY: // Group RmsgT1 is empty.
    case INFO_HOST_X_IS_EMPTY: // Host RmsgT1 is empty.
    case INFO_SHARE_X_IS_EMPTY: // Share RmsgT1 is empty.
    case INFO_DIR_X_IS_EMPTY: // Dir RmsgT1 is empty.
             Rys = 8; Rxs = 60;
             RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
             box(RwinMsg,0,0);
             switch(RnrMsg)
             {
               case INFO_GROUP_X_IS_EMPTY:
                    memcpy(Rmsg1, Rm43, Rstrlen(Rm43)+1);
                    break;
               case INFO_HOST_X_IS_EMPTY:
                    memcpy(Rmsg1, Rm44, Rstrlen(Rm44)+1);
                    break;
               case 19: memcpy(Rmsg1, Rm24, Rstrlen(Rm24)+1);
                        break;
               case INFO_DIR_X_IS_EMPTY:
                    memcpy(Rmsg1, Rm39, Rstrlen(Rm39)+1);
                    break;
             }
             Rt2 = Rstrlen(Rmsg1); Rt1 = Rt2;
             memcpy(RmsgTmp, Rmsg1, Rt2);
             RmsgT1 = to_scr(Rmsg);
             Rt2 = Rstrlen(RmsgT1);
             memcpy(RmsgTmp+Rt1, RmsgT1, Rt2); Rt1 += Rt2;
             memcpy(RmsgTmp+Rt1, Rm23, Rstrlen(Rm23)+1);
             Rii = Rxs/2-Rstrlen(RmsgTmp)/2;
               Rmvwprintw(RwinMsg, 2, 1+Rii, RmsgTmp);
             Rii = Rxs/2-Rstrlen(Rm136)/2;
               wattron(RwinMsg, COLOR_PAIR(1)); 
               Rmvwprintw(RwinMsg, 5, 1+Rii, Rm136);
               wattroff(RwinMsg, COLOR_PAIR(1)); 
             wrefresh(RwinMsg);
             RpressEnter(RwinMsg);
             Rdelwin(RwinMsg);
             break;
  }
  if (Rredraw_MainWin == DO_REDRAW_MAIN_WINDOW)
    RwinRedrawWithData();
  RdontResizeWindow = 0;
  return(0);
}

void 
RwinRedrawWithData()
{
  RwinRedraw();
  RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
  RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
}

int 
RmanageList()
{
  WINDOW *RwinList = NULL;
  WINDOW *RwinCol1 = NULL;
  WINDOW *RwinCol2 = NULL;
  WINDOW *RwinCol3 = NULL;
  WINDOW *RwinCol4 = NULL;
  WINDOW *RwinCol5 = NULL;
  WINDOW *RwinCol6 = NULL;
  WINDOW *RwinCol7 = NULL;
  int Rkey, Rsel, RmaxY, Rx6, Ri, RlFirst, RlSelect;
  TTdownload *RdownloadListTmp, *RdownloadListTmp2;
  int Rcut = 0;
  sighandler_t RpreviousAction;

  void 
  RpaintList(int Rfirst, int Rselect)
  {
    int Rlast, Ri;
    int Rk = -1;
    char *RtimeRemaing = NULL;
    char *RtimeDelay = NULL;
    char *RstrTmp1 = NULL;

    RdownloadListTmp = RgetdownloadListNr(RdownloadList, Rfirst); 

    if ((Rfirst == 0) & (RgetdownloadListCount(RdownloadList) <= RmaxY)) 
      Rlast = RgetdownloadListCount(RdownloadList);
    else Rlast = Rfirst + RmaxY;

    for (Ri = Rfirst; Ri < Rlast; Ri++) 
    {
      if (!RdownloadListTmp) {
        RDBG("smbc: RmanageList RpaintList - error expected next record\n"); 
        RsigTerm();
      }
      mvwhline(RwinCol1, ++Rk, 0, 32, 1);
      mvwhline(RwinCol2, Rk, 0, 32, 1);
      mvwhline(RwinCol3, Rk, 0, 32, 5);
      mvwhline(RwinCol4, Rk, 0, 32, 5);
      mvwhline(RwinCol5, Rk, 0, 32, 3);
      mvwhline(RwinCol6, Rk, 0, 32, Rx6);
      mvwhline(RwinCol7, Rk, 0, 32, RmainWinX-21-Rx6);
      if (Ri == Rselect) 
      {
          wattron(RwinCol1, COLOR_PAIR(1)); 
          wattron(RwinCol2, COLOR_PAIR(1)); 
          wattron(RwinCol3, COLOR_PAIR(1)); 
          wattron(RwinCol4, COLOR_PAIR(1)); 
          wattron(RwinCol5, COLOR_PAIR(1)); 
          wattron(RwinCol6, COLOR_PAIR(1)); 
          wattron(RwinCol7, COLOR_PAIR(1)); 
          mvwhline(RwinCol1, Rk, 0, 32, 1);
          mvwhline(RwinCol2, Rk, 0, 32, 1);
          mvwhline(RwinCol3, Rk, 0, 32, 5);
          mvwhline(RwinCol4, Rk, 0, 32, 5);
          mvwhline(RwinCol5, Rk, 0, 32, 3);
          mvwhline(RwinCol6, Rk, 0, 32, Rx6);
          mvwhline(RwinCol7, Rk, 0, 32, RmainWinX-21-Rx6);
      }
      mvwprintw(RwinCol1, Rk, 0, "%c", RdownloadListTmp->active); 
      mvwprintw(RwinCol2, Rk, 0, "%c", RdownloadListTmp->upload); 
      Rmvwprintw(RwinCol3, Rk, 0, to_scr(RdownloadListTmp->sizeDisplayR)); 
      Rmvwprintw(RwinCol4, Rk, 0, to_scr(RdownloadListTmp->sizeDisplayDownR)); 
      mvwprintw(RwinCol5, Rk, 0, "%s%%", to_scr(RdownloadListTmp->percent)); 
      Rmvwprintw(RwinCol6, Rk, 0, to_scr(RdownloadListTmp->filename));
      RreturnHumanDelayTime(&RtimeRemaing, RdownloadListTmp->nextRetry-time(NULL));
      RreturnHumanDelayTime(&RtimeDelay, RdownloadListTmp->delayRetry);

      RstrdupN(RstrTmp1, to_scr(RtimeRemaing));
      Rstraddtoend(&RstrTmp1, " | ");
      Rstraddtoend(&RstrTmp1, to_scr(RtimeDelay));
      Rstraddtoend(&RstrTmp1, " | ");
      if (RdownloadListTmp->upload == 'D') 
      {
        Rstraddtoend(&RstrTmp1, "//");
        Rstraddtoend(&RstrTmp1, to_scr(RdownloadListTmp->hostname));
        Rstraddtoend(&RstrTmp1, "/");
        Rstraddtoend(&RstrTmp1, to_scr(RdownloadListTmp->pathR));
        Rstraddtoend(&RstrTmp1, " | ");
        Rstraddtoend(&RstrTmp1, to_scr(RdownloadListTmp->pathL));
        Rstraddtoend(&RstrTmp1, " | ");
        Rstraddtoend(&RstrTmp1, to_scr(RdownloadListTmp->groupnameR));
      }
      else 
      {
        Rstraddtoend(&RstrTmp1, to_scr(RdownloadListTmp->pathL));
        Rstraddtoend(&RstrTmp1, " | ");
        Rstraddtoend(&RstrTmp1, "//");
        Rstraddtoend(&RstrTmp1, to_scr(RdownloadListTmp->hostname));
        Rstraddtoend(&RstrTmp1, "/");
        Rstraddtoend(&RstrTmp1, to_scr(RdownloadListTmp->pathR));
        Rstraddtoend(&RstrTmp1, " | ");
        Rstraddtoend(&RstrTmp1, to_scr(RdownloadListTmp->groupnameR));

      }
      Rmvwprintw(RwinCol7, Rk, 0, RstrTmp1);
      Rfree(RstrTmp1);
      Rfree(RtimeRemaing);
      Rfree(RtimeDelay);

      if (Ri == Rselect) 
      {
          wattroff(RwinCol1, COLOR_PAIR(1)); 
          wattroff(RwinCol2, COLOR_PAIR(1)); 
          wattroff(RwinCol3, COLOR_PAIR(1)); 
          wattroff(RwinCol4, COLOR_PAIR(1)); 
          wattroff(RwinCol5, COLOR_PAIR(1)); 
          wattroff(RwinCol6, COLOR_PAIR(1)); 
          wattroff(RwinCol7, COLOR_PAIR(1)); 
      }
      RdownloadListTmp = RdownloadListTmp->next;
    }
    for (Ri = Rlast+1; Ri < Rfirst+RmaxY+1; Ri++) 
    {
      mvwhline(RwinCol1, ++Rk, 0, 32, 1);
      mvwhline(RwinCol2, Rk, 0, 32, 1);
      mvwhline(RwinCol3, Rk, 0, 32, 5);
      mvwhline(RwinCol4, Rk, 0, 32, 5);
      mvwhline(RwinCol5, Rk, 0, 32, 3);
      mvwhline(RwinCol6, Rk, 0, 32, Rx6);
      mvwhline(RwinCol7, Rk, 0, 32, RmainWinX-21-Rx6);
    }
    wrefresh(RwinCol1);
    wrefresh(RwinCol2);
    wrefresh(RwinCol3);
    wrefresh(RwinCol4);
    wrefresh(RwinCol5);
    wrefresh(RwinCol6);
    wrefresh(RwinCol7);
    RDBG("smbc : RmanageList 10.1\n");
  }

  void 
  RmanageListCreateWindow()
  {
    int Rii = RgetdownloadListCount(RdownloadList);
    Rx6 = 0;
    RdownloadListTmp = RdownloadList;
    for (Ri = 0; Ri < Rii; Ri++) 
    {
      if (Rstrlen(RdownloadListTmp->filename) > Rx6)
        Rx6 = Rstrlen(RdownloadListTmp->filename);
      RdownloadListTmp = RdownloadListTmp->next;
    }
    getmaxyx(RwinMain, RmaxY, Rsel);
    if (Rx6 > Rsel-18) 
      Rx6 = Rsel-18-10;
    if (Rcut)
      RAstatusBar(STATUS_DEFAULT_FOR_DULIST_MOVE);
    else
      RAstatusBar(STATUS_DEFAULT_FOR_DULIST);
    RwinList = newwin(RmainWinY-2, RmainWinX, 1, 0);
    RwinCol1 = newwin(RmainWinY-6, 1, 4, 1);
    RwinCol2 = newwin(RmainWinY-6, 1, 4, 3);
    RwinCol3 = newwin(RmainWinY-6, 5, 4, 5);
    RwinCol4 = newwin(RmainWinY-6, 5, 4, 11);
    RwinCol5 = newwin(RmainWinY-6, 3, 4, 17);
    RwinCol6 = newwin(RmainWinY-6, Rx6, 4, 21);
    RwinCol7 = newwin(RmainWinY-6, RmainWinX-Rx6-23, 4, 22+Rx6);
    getmaxyx(RwinCol1, RmaxY, Rsel);
    Rsel = 0;
    Rmvwprintw(RwinList, 1,1, Rm52);
    Rmvwprintw(RwinList, 1,1+21+Rx6, Rm53);
    box(RwinList,0,0);
    mvwvline(RwinList, 1, 2, 0, RmainWinY-4);
    mvwvline(RwinList, 1, 4, 0, RmainWinY-4);
    mvwvline(RwinList, 1, 10, 0, RmainWinY-4);
    mvwvline(RwinList, 1, 16, 0, RmainWinY-4);
    mvwvline(RwinList, 1, 20, 0, RmainWinY-4);
    mvwvline(RwinList, 1, 20+Rx6+1, 0, RmainWinY-4);
    mvwhline(RwinList, 2, 1, 0, RmainWinX-2);
    wrefresh(RwinList);
    keypad(RwinList, TRUE); 
    nodelay(RwinList, 0);
    RactiveWindow = RwinList;
    RDBG("smbc : RmanageList RactiveWindow = RwinList  2\n");
  }

  void 
  RdoWindowResizeDownloadList()
  {
    if (RdoResizeDownload) 
    {
      RdoResizeDownload = 0;
      RdoWindowResize();
      RmanageListCreateWindow();
      RpaintList(RlFirst,RlSelect);
    }
  }

  void 
  RsigAlarmManageWindow() 
  {
    RDBG("RmanageList : RsigAlarmManageWindow alarm\n");
    RdoWindowResizeDownloadList();
    RpaintList(RlFirst,RlSelect);
    alarm(RCalarm); 
  }

  // -------------------------------------------------------------
  // --- begin a procedure
  // -------------------------------------------------------------

  alarm(0); 

  RmanageListCreateWindow();
  RlFirst = 0; RlSelect = 0;
  RpaintList(RlFirst, RlSelect);

  RpreviousAction = signal(SIGALRM, RsigAlarmManageWindow);

  alarm(RCalarm); 
  while ((Rkey = mvwgetch(RwinList, 1,1))) 
  {
    alarm(0); 
    if ((Rkey == ',') || (Rkey == 260)) 
    {
      RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect);
      if (RdownloadListTmp->delayRetry > 0)
        RdownloadListTmp->delayRetry--;
    }
    if ((Rkey == '.') || (Rkey == 261)) 
    {
      RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect);
      RdownloadListTmp->delayRetry++;
    }
    if ((Rkey == '<') || (Rkey == 68)) 
    {
      RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect);
      if (RdownloadListTmp->delayRetry > 60)
        RdownloadListTmp->delayRetry = RdownloadListTmp->delayRetry-60;
      else
        RdownloadListTmp->delayRetry = 0;
    }
    if ((Rkey == '>') || (Rkey == 67)) 
    {
      RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect);
      RdownloadListTmp->delayRetry = RdownloadListTmp->delayRetry+60;
    }

    if (Rkey == ';') 
    {
      RdownloadListTmp = RgetdownloadListFirst(RdownloadList);
      while (RdownloadListTmp) 
      {
        if (RdownloadListTmp->delayRetry > 0)
          RdownloadListTmp->delayRetry--;
        RdownloadListTmp = RdownloadListTmp->next;
      }
    }
    if (Rkey == '\'') 
    {
      RdownloadListTmp = RgetdownloadListFirst(RdownloadList);
      while (RdownloadListTmp) 
      {
        RdownloadListTmp->delayRetry++;
        RdownloadListTmp = RdownloadListTmp->next;
      }
    }
    if (Rkey == ':') 
    {
      RdownloadListTmp = RgetdownloadListFirst(RdownloadList);
      while (RdownloadListTmp) 
      {
        if (RdownloadListTmp->delayRetry > 60)
          RdownloadListTmp->delayRetry = RdownloadListTmp->delayRetry-60;
        else
          RdownloadListTmp->delayRetry = 0;
        RdownloadListTmp = RdownloadListTmp->next;
      }
    }
    if (Rkey == '\"')  
    {
      RdownloadListTmp = RgetdownloadListFirst(RdownloadList);
      while (RdownloadListTmp) {
        RdownloadListTmp->delayRetry = RdownloadListTmp->delayRetry+60;
        RdownloadListTmp = RdownloadListTmp->next;
      }
    }
    if (Rkey == '|') 
    {
      RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect);
      RdownloadListTmp2 = RgetdownloadListFirst(RdownloadList);
      while (RdownloadListTmp2) 
      {
        RdownloadListTmp2->delayRetry = RdownloadListTmp->delayRetry;
        RdownloadListTmp2 = RdownloadListTmp2->next;
      }
    }
    if (Rkey == 'n') 
    {
      RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect);
      RdownloadListTmp->nextRetry = time(NULL);
    }
    if (Rkey == 'N') 
    {
      RdownloadListTmp = RgetdownloadListFirst(RdownloadList);
      while (RdownloadListTmp) 
      {
        RdownloadListTmp->nextRetry = time(NULL);
        RdownloadListTmp = RdownloadListTmp->next;
      }
    }

    if (Rkey == 32) 
    {
       RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect);
       if(RdownloadListTmp->active == 'X') RdownloadListTmp->active = ' ';
       else RdownloadListTmp->active = 'X';
       Rkey = KEY_DOWN;
    }
    if (Rkey == 'd' || Rkey == KEY_DC) 
    {
       RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect);
       RdownloadListTmp->active = 'D';
       Rkey = KEY_DOWN;
    }
    if (Rkey == 4) 
    {             // CTRL-d
       RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect+1);
       RdownloadListTmp2 = RgetdownloadListFirst(RdownloadList);
       while (RdownloadListTmp2 != NULL) 
       {
         if (RstrCmp(RdownloadListTmp2->pathR, RdownloadListTmp->pathR) == 1)  
            RdownloadListTmp2->active = 'D';
         RdownloadListTmp2 = RdownloadListTmp2->next;
       }
       RpaintList(RlFirst,RlSelect);
    }
    if (Rkey == 6) 
    {             // CTRL-f
       RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect+1);
       RdownloadListTmp2 = RgetdownloadListFirst(RdownloadList);
       while (RdownloadListTmp2 != NULL) 
       {
         RdownloadListTmp2->active = 'D';
         RdownloadListTmp2 = RdownloadListTmp2->next;
       }
       RpaintList(RlFirst,RlSelect);
    }
    if (Rkey == 'u') 
    {
       RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect);
       RdownloadListTmp->active = 'X';
       Rkey = KEY_DOWN;
    }
    if (Rkey == KEY_DOWN || Rkey == 'o') 
    {
       RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect);
       if ((Rcut) && (RdownloadListTmp->next)) 
       {
         RdownloadListTmp2 = RdownloadListTmp;
         if (RdownloadListTmp->previous)
           RdownloadListTmp->previous->next = RdownloadListTmp->next;
         else
           RdownloadList = RdownloadListTmp->next;
         RdownloadListTmp->next->previous = RdownloadListTmp->previous;
 
         RdownloadListTmp = RdownloadListTmp->next;
 
         RdownloadListTmp2->previous = RdownloadListTmp;
         RdownloadListTmp2->next = RdownloadListTmp->next;
         
         if (RdownloadListTmp->next)
           RdownloadListTmp->next->previous = RdownloadListTmp2;
         RdownloadListTmp->next = RdownloadListTmp2;
       }
      if ((RlSelect+1 == RlFirst+RmaxY) & (RlFirst+RmaxY < RgetdownloadListCount(RdownloadList))) 
      {
        RlFirst++; 
        RlSelect++;
      }
      else 
        if (RlSelect < RgetdownloadListCount(RdownloadList)-1) 
          RlSelect++;
      RpaintList(RlFirst,RlSelect);
    }
    if (Rkey == KEY_UP || Rkey == 'p') 
    {
       RdownloadListTmp = RgetdownloadListNr(RdownloadList, RlSelect);
       if ((Rcut) && (RdownloadListTmp->previous)) 
       {
         RdownloadListTmp2 = RdownloadListTmp;
         if (RdownloadListTmp->next)
           RdownloadListTmp->next->previous = RdownloadListTmp->previous;
         RdownloadListTmp->previous->next = RdownloadListTmp->next;
 
         RdownloadListTmp = RdownloadListTmp->previous;
 
         RdownloadListTmp2->next = RdownloadListTmp;
         RdownloadListTmp2->previous = RdownloadListTmp->previous;
         
         if (RdownloadListTmp->previous)
           RdownloadListTmp->previous->next = RdownloadListTmp2;
         else
           RdownloadList = RdownloadListTmp2;
         RdownloadListTmp->previous = RdownloadListTmp2;
       }
      if (RlSelect > 0) 
      {
         if ((RlFirst > 0) && (RlSelect == RlFirst)) { RlFirst--; RlSelect--; } 
         else RlSelect--;
         RpaintList(RlFirst,RlSelect);
      }
    }
    if (Rkey == KEY_NPAGE || Rkey == 'k') 
    {
      if (RlFirst+RmaxY <= RgetdownloadListCount(RdownloadList)) 
      {
        if (RlSelect+1 == RlFirst+RmaxY) 
        {
           if (RlSelect+RmaxY < RgetdownloadListCount(RdownloadList)) 
           {
              RlFirst = RlSelect;
              RlSelect = RlFirst+RmaxY-1;
            }
            else 
            {
              RlFirst = RgetdownloadListCount(RdownloadList)-RmaxY;
              RlSelect = RgetdownloadListCount(RdownloadList)-1;
            }
        }
        else 
        {
           if (RlSelect+RmaxY <= RgetdownloadListCount(RdownloadList)) 
             RlSelect = RlFirst+RmaxY-1;
           else
             RlSelect = RgetdownloadListCount(RdownloadList)-1;
        }
      }
      RpaintList(RlFirst,RlSelect);
    }
    if (Rkey == KEY_PPAGE || Rkey == 'l') 
    {
      if (RlSelect > 1) 
      {
        if (RlFirst == RlSelect) 
        {
          if (RlFirst - RmaxY < 1) 
          {
            RlFirst = 1;
            RlSelect = 1;
          }
          else 
          {
            RlFirst = RlFirst - RmaxY + 1;
            RlSelect = RlFirst;
          }
        }
        else RlSelect = RlFirst;
        RpaintList(RlFirst,RlSelect);
      }
    }
    if (Rkey == 'm') 
    {
      if (Rcut) 
      {
        Rcut = 0;
        RAstatusBar(STATUS_DEFAULT_FOR_DULIST);
      }
      else 
      {
        Rcut = 1;
        RAstatusBar(STATUS_DEFAULT_FOR_DULIST_MOVE);
      }
    }
    if (Rkey == 'q') 
    {
      Ri = 1;
      while (RdownloadList && Ri) 
      {
        if (RdownloadList->active == 'D') 
        {
          if (RdownloadList == RdownloadListAct) 
          {
            Rsmbc_close(RdownloadHandleRD);
            close(RdownloadHandleWR);
            RdownloadHandleWR = -1;
            RdownloadHandleRD = -1;
            RdownloadListAct = NULL;
          }
          RfreeOneTTdownloadAndGoToFirst(&RdownloadList);
        }
        else 
        {
          if (RdownloadList->next != NULL)
            RdownloadList = RdownloadList->next;
          else
          {
            RdownloadList = RgetdownloadListFirst(RdownloadList);
            Ri = 0;
          }
        }
      }
      Rdelwin(RwinList);
      Rdelwin(RwinCol1);
      Rdelwin(RwinCol3);
      Rdelwin(RwinCol4);
      Rdelwin(RwinCol5);
      Rdelwin(RwinCol6);
      Rdelwin(RwinCol7);
      RwinRedrawWithData();
      signal(SIGALRM, RpreviousAction);
      alarm(RCalarm);
      return (0);
    }
    Rkey = 0; 
    alarm(RCalarm); 
  }
  signal(SIGALRM, RpreviousAction);
  alarm(RCalarm);
  return(0);
}

void
RfreeMasterList()
{
  TTgroups *RgroupsT;

  while (Rgroups)
  {
    RgroupsT = Rgroups;
    Rgroups = Rgroups->next;
    Rfree(RgroupsT->groupname);
    Rfree(RgroupsT->mastername);
    Rfree(RgroupsT->masterip);
    Rfree(RgroupsT);
  }
}

int 
RgetMasterList(char RwithStatus)
{
  int Rfd;
  struct smbc_dirent *Rdirent = NULL;
  int Rcount = RretryReadGroups;
  char Rcontinue = 0;
  char *RstrA = NULL;
  char Rok = 0;
  int RgroupsCount = 0;

  void 
  RretryAgain()
  {
    if (Rcount == 0) Rcontinue = 1;
    if ((RwithStatus) && (!Rcontinue))
    { 
      RstrdupN(RstrA, Rm34);
      if (Rcount >= 0)
        Rintaddtoend(&RstrA, Rcount);
      RAstatusBarStr(RstrA, 0);
      Rfree(RstrA);
    }
    if (Rcount > 0)  Rcount--;
  }

  while (!Rcontinue)
  {
    RfreeMasterList();
    RgroupsCount = 0;
    RDBG("smbc: RgetMasterList 1\n");
    if ((Rfd = Rsmbc_opendir("smb://")) > 0) 
    {
      Rok = 1;
      RDBG("smbc: RgetMasterList 2\n");
      while(Rsmbc_readdir(Rfd, &Rdirent)) 
      {
        if (Rdirent->smbc_type == SMBC_WORKGROUP) 
        { 
          if (Rgroups)
          {
            Rgroups->next = Rmalloc(sizeof(TTgroups));
            Rgroups->next->previous = Rgroups;
            Rgroups = Rgroups->next;
          }
          else
          {
            Rgroups = Rmalloc(sizeof(TTgroups));
            Rgroups->previous = NULL;
          }
          Rgroups->next = NULL;

          Rgroups->groupname = NULL;
          Rgroups->mastername = NULL;
          Rgroups->masterip = NULL;

          RstrdupN(Rgroups->groupname, Rdirent->name);
          RgroupsCount++;
          Rcontinue = 1;
        }
      }
      Rsmbc_closedir(Rfd);
    }
    else
      RretryAgain();
  }
  RDBG("smbc: RgetMasterList Rok = "); RDBGi(Rok); RDBG("\n");
  if (Rok)
  {
    if (Rgroups)
      while (Rgroups->previous)
        Rgroups = Rgroups->previous;
  }
  else
    RgroupsCount = -1;
  Rfree(RstrA);
  return(RgroupsCount);
}

void 
RfreeHostsList()
{
  TThosts *RhostsT;

  while (Rhosts)
  {
    RhostsT = Rhosts;
    Rhosts = Rhosts->next;
    Rfree(RhostsT->hostname);
    Rfree(RhostsT->comment);
    Rfree(RhostsT->ip);
    Rfree(RhostsT);
  }
  Rhosts = NULL;
}

int 
RgetHostsFromGroup(char *group, char RwithStatus)
{
  int Rfd;
  struct smbc_dirent *Rdirent = NULL;
  char *Rstr = NULL;
  int Rcount = RretryReadHosts;
  char Rcontinue = 0;
  char *RstrA = NULL;
  char Rok = 0;
  int RhostsCount = 0;

  void 
  RretryAgain()
  {
    if (Rcount == 0) Rcontinue = 1;
    if ((RwithStatus) && (!Rcontinue))
    { 
      RstrdupN(RstrA, Rm35);
      if (Rcount >= 0)
        Rintaddtoend(&RstrA, Rcount);
      RAstatusBarStr(RstrA, 0);
      Rfree(RstrA);
    }
    if (Rcount > 0)  Rcount--;
  }

  RstrdupN(Rstr, "smb://");
  Rstraddtoend(&Rstr, group);
  Rstraddtoend(&Rstr, "/");

  while (!Rcontinue)
  {
    RfreeHostsList();
    RhostsCount = 0;
    if ((Rfd = Rsmbc_opendir(Rstr)) > 0) 
    {
      Rok = 1;
      while(Rsmbc_readdir(Rfd, &Rdirent)) 
      {
        if (Rdirent->smbc_type == SMBC_SERVER) 
        { 
          if (Rhosts)
          {
            Rhosts->next = Rmalloc(sizeof(TThosts));
            Rhosts->next->previous = Rhosts;
            Rhosts = Rhosts->next;
          }
          else
          {
            Rhosts = Rmalloc(sizeof(TThosts));
            Rhosts->previous = NULL;
          }
          Rhosts->next = NULL;

          Rhosts->hostname = NULL;
          Rhosts->comment = NULL;
          Rhosts->ip = NULL;

          RstrdupN(Rhosts->hostname, Rdirent->name);
          RstrdupN(Rhosts->comment, Rdirent->comment);
          Rcontinue = 1;
          RhostsCount++;
        }
      }
      Rsmbc_closedir(Rfd);
    }
    RretryAgain();
  }
  Rfree(Rstr);
  if (Rok)
  {
    if (Rhosts)
      while (Rhosts->previous)
        Rhosts = Rhosts->previous;
  }
  else
    RhostsCount = -1;
  Rfree(RstrA);
  return(RhostsCount);
}

void 
RfreeSharesList()
{
  TTshares *RsharesT;

  while (Rshares)
  {
    RsharesT = Rshares;
    Rshares = Rshares->next;
    Rfree(RsharesT->sharename);
    Rfree(RsharesT->comment);
    Rfree(RsharesT);
  }
  Rshares = NULL;
}

int 
RgetSharesFromHost(char *host, char RwithStatus)
{
  int Rfd = 0;
  struct smbc_dirent *Rdirent = NULL;
  char *Rstr = NULL;
  int Rcount = RretryReadShares;
  char Rcontinue = 0;
  char *RstrA = NULL;
  char Rok = 0;
  int RsharesCount;

  void 
  RretryAgain()
  {
    if (Rcount == 0) Rcontinue = 1;
    if ((RwithStatus) && (!Rcontinue))
    { 
      RstrdupN(RstrA, Rm36);
      if (Rcount >= 0)
        Rintaddtoend(&RstrA, Rcount);
      RAstatusBarStr(RstrA, 0);
      Rfree(RstrA);
    }
    if (Rcount > 0)  Rcount--;
  }

  RDBG("smbc: RgetSharesFromHost 1\n");
  RstrdupN(Rstr, "smb://");
  Rstraddtoend(&Rstr, host);
  Rstraddtoend(&Rstr, "/");

  RsharesCount = 0;
  while (!Rcontinue)
  {
    RfreeSharesList();
    RsharesCount = 0;
    if ((Rfd = Rsmbc_opendir(Rstr)) > 0) 
    {
      Rok = 1;
      while(Rsmbc_readdir(Rfd, &Rdirent)) 
      {
        if (Rdirent->smbc_type == SMBC_FILE_SHARE) 
          if (Rstrlen(Rdirent->name) > 0) 
          {
            if (Rdirent->name[Rstrlen(Rdirent->name)-1] != '$') 
            { 
              if (Rshares)
              {
                Rshares->next = Rmalloc(sizeof(TTshares));
                Rshares->next->previous = Rshares;
                Rshares = Rshares->next;
              }
              else
              {
                Rshares = Rmalloc(sizeof(TTshares));
                Rshares->previous = NULL;
              }
              Rshares->next = NULL;
              
              Rshares->sharename = NULL;
              Rshares->comment = NULL;

              RstrdupN(Rshares->sharename, Rdirent->name);
              RstrdupN(Rshares->comment, Rdirent->comment);
              RsharesCount++;
              Rcontinue = 1;
            }
          }
      }
      Rsmbc_closedir(Rfd);
    }
    else
      RretryAgain();
  }
  if (Rok)
  {
    if (Rshares)
      while (Rshares->previous)
        Rshares = Rshares->previous;
  }
  else
    RsharesCount = -1;
  Rfree(Rstr);
  return(RsharesCount);
}

void
RfreeFilesList(TTfiles **RfilesI)
{
  TTfiles *RfilesT;

  while (*RfilesI)
  {
    RfilesT = *RfilesI;
    *RfilesI = (*RfilesI)->next;
    Rfree(RfilesT->filename);
    Rfree(RfilesT->sizeShow);
    Rfree(RfilesT);
  }
}

int 
RgetFileFromDirR(char *dir, char RwithStatus)
{
  int dirhandle, filehandle;
  struct smbc_dirent *Rdirent = NULL;
  char *Rfn = NULL;
  struct stat remotestat;
  int Rsize;
  char *RstrA = NULL;
  char RfileSize[TfilesSizeShowLen];
  int RfilesCount = -1;
  char *RstrgFFD = NULL;

  RfreeFilesList(&RfilesR);

  RstrgFFD = RgetSmallPath(Rahost); 
  Rstraddtoend(&RstrgFFD, dir);

  dirhandle = Rsmbc_opendir(RstrgFFD);
  Rfree(RstrgFFD);
  RDBG("smbc : RgetFileFromDirR dirhandle = "); RDBGi(dirhandle); RDBG("\n");
  if(dirhandle > 0) 
  {
    RfilesCount = 0;
    while(Rsmbc_readdir(dirhandle, &Rdirent)) 
    {
      if (strcmp(Rdirent->name,".") && strcmp(Rdirent->name,"..")) 
      {
        if (RfilesR)
        {
          RfilesR->next = Rmalloc(sizeof(TTfiles)); 
          RfilesR->next->previous = RfilesR;
          RfilesR = RfilesR->next;
        }
        else
        {
          RfilesR = Rmalloc(sizeof(TTfiles)); 
          RfilesR->previous = NULL;
        }
        RfilesR->next = NULL;

        RfilesR->filename = NULL;
        RfilesR->sizeShow = NULL;
        RfilesR->type = 0;
        RfilesR->size = 0;

        RstrdupN(RfilesR->filename, Rdirent->name);

        RstrdupN(Rfn, "smb://");
        Rstraddtoend(&Rfn, Rahost);
        Rcharaddtoend(&Rfn, '/');
        Rstraddtoend(&Rfn, dir);
        Rstraddtoend(&Rfn, Rdirent->name);
        RDBG("smbc: RgetFileFromDirR Rfn = "); RDBG(Rfn); RDBG("\n");
        filehandle = Rsmbc_open(Rfn, O_RDONLY, 0755);
        if (Rsmbc_fstat(filehandle, &remotestat) < 0) 
        {
          Rsize = 0;
          RfilesR->type = RTDIR;
        }
        else 
        {
          Rsize = remotestat.st_size;
          RfilesR->type = RTFILE;
        }

        RgetHumanSize(RfileSize, TfilesSizeShowLen, Rsize);
        RstrdupN(RfilesR->sizeShow, RfileSize);

        RfilesR->size = Rsize;
        Rsmbc_close(filehandle);
        Rfree(Rfn);
        RfilesCount++;
      }
    }
    Rsmbc_closedir(dirhandle);
  }
  if (RfilesCount == -1)
    RstrdupN(RstrA, Rm9)
  else
  {
    if (RfilesCount == 0)
      RstrdupN(RstrA, Rm10)
    else
      RstrdupN(RstrA, Rm11);

    /* go to first record */
    if (RfilesR)
      while (RfilesR->previous)
        RfilesR = RfilesR->previous;
  }
  if (RwithStatus)
    RAstatusBarStr(RstrA, 0);
  Rfree(RstrA);
  return(RfilesCount);
}
 
int 
RgetFileFromDirL(char *dir)
{
  DIR *dirhandle;
  struct dirent *Rdirent;
  char *Rfn = NULL;
  struct stat localstat;
  int Rsize;
  char *RstrT1 = NULL;
  int RfilesCount;

  RDBG("smbc: RgetFileFromDirL start\n");
  RfreeFilesList(&RfilesL);
  dirhandle = opendir(to_sys(dir));
  if(dirhandle == NULL) 
  { 
    RshowMessage(INFO_I_CANT_ENTER_TO, dir, "", NULL, DO_REDRAW_MAIN_WINDOW);
    return -1;
  }
  RfilesCount = 0;
  while((Rdirent =  readdir(dirhandle))) 
  {
    RstrT1 = from_sys(Rdirent->d_name);
    if (strcmp(RstrT1, ".") && strcmp(RstrT1, "..")) 
    {
      RDBG("smbc: RgetFileFromDirL 1\n");
      RstrdupN(Rfn, dir);
      Rstraddtoend(&Rfn, RstrT1);
      stat(to_sys(Rfn), &localstat);
      Rfree(Rfn);
      RDBG("smbc: RgetFileFromDirL 4\n");

      if (RfilesL)
      {
        RfilesL->next = Rmalloc(sizeof(TTfiles)); 
        RfilesL->next->previous = RfilesL;
        RfilesL = RfilesL->next;
      }
      else
      {
        RfilesL = Rmalloc(sizeof(TTfiles)); 
        RfilesL->previous = NULL;
      }
      RfilesL->next = NULL;

      RfilesL->filename = NULL;
      RfilesL->sizeShow = NULL;
      RfilesL->type = 0;
      RfilesL->size = 0;

      RstrdupN(RfilesL->filename, RstrT1);
      RDBG("smbc: RgetFileFromDirL 10\n");
      RgetHumanSizeMalloc(&(RfilesL->sizeShow), TfilesSizeShowLen, localstat.st_size);
      RDBG("smbc: RgetFileFromDirL 11\n");
      RfilesL->size = localstat.st_size;

      if (localstat.st_mode & S_IFDIR) 
      {
         Rsize = 0;
         RfilesL->type = RTDIR;
      }
      else 
      {
        Rsize = Rdirent->d_reclen;
        RfilesL->type = RTFILE;
      }
      RfilesCount++;
      RDBG("smbc: RgetFileFromDirL 15\n");
    }
    RDBG("smbc: RgetFileFromDirL 16\n");
  }
  RDBG("smbc: RgetFileFromDirL 20\n");
  closedir(dirhandle);
  /* go to first record */
  if (RfilesL)
    while (RfilesL->previous)
      RfilesL = RfilesL->previous;
  return RfilesCount;
}

void 
RwinRedraw()
{
  box(RwinLeft,0,0);
  box(RwinRight,0,0);
  mvwhline(RwinLeftTop, 0, 0, 0, RxHalf-2);
  mvwhline(RwinRightTop, 0, 0, 0, RxHalf-2);
  mvwprintw(RwinHead, 0, 0,"Samba Commander %s", VERSION);
  mvwprintw(RwinHead, 0, RmainWinX-Rstrlen(Rm108), Rm108);
  mvwhline(RwinLeft, 2, 1, 0, RxHalf-2);
  mvwhline(RwinLeft, 2, 1, 0, RxHalf-2);
  mvwvline(RwinLeftList, 0, 1, 0, RmainWinY-6);
  mvwvline(RwinLeftList, 0, RxHalf-8, 0, RmainWinY-6);
  mvwhline(RwinRight, 2, 1, 0, RxHalf-2+Rdx);
  mvwvline(RwinRightList, 0, 1, 0, RmainWinY-6);
  mvwvline(RwinRightList, 0, RxHalf-8+Rdx, 0, RmainWinY-6);
  mvwvline(RwinLeftHead, 0, 1, 0, 1);
  mvwvline(RwinLeftHead, 0, RxHalf-8, 0, 1);
  mvwvline(RwinRightHead, 0, 1, 0, 1);
  mvwvline(RwinRightHead, 0, RxHalf-8+Rdx, 0, 1);
  Rmvwprintw(RwinLeftTop, 0, 0, "/");
  Rmvwprintw(RwinRightTop, 0, 0, "//");
  Rmvwprintw(RwinLeftHeadCol1, 0, 0, Rm123);
  Rmvwprintw(RwinLeftHeadCol2, 0, 0, Rm121);
  Rmvwprintw(RwinLeftHeadCol3, 0, 0, Rm125);
  Rmvwprintw(RwinRightHeadCol1, 0, 0, Rm123);
  Rmvwprintw(RwinRightHeadCol2, 0, 0, Rm124);
  Rmvwprintw(RwinRightHeadCol3, 0, 0, Rm125);
  wrefresh(RwinMain);
  wrefresh(RwinLeft);
  wrefresh(RwinRight);
  wrefresh(RwinHead);
  wrefresh(RwinLeftHead);
  wrefresh(RwinRightHead);
  wrefresh(RwinLeftList);
  wrefresh(RwinRightList);
  wrefresh(RwinLeftTop);
  wrefresh(RwinRightTop);
  wrefresh(RwinLeftCol1);
  wrefresh(RwinLeftCol2);
  wrefresh(RwinLeftCol3);
  wrefresh(RwinRightCol1);
  wrefresh(RwinRightCol2);
  wrefresh(RwinRightCol3);
  wrefresh(RwinLeftHeadCol1);
  wrefresh(RwinLeftHeadCol2);
  wrefresh(RwinLeftHeadCol3);
  wrefresh(RwinRightHeadCol1);
  wrefresh(RwinRightHeadCol2);
  wrefresh(RwinRightHeadCol3);
  RAstatusBar(STATUS_ADDING_FILES_TO_UPLOAD_LIST);
}

void 
RwriteMenu(int Rredraw)
{
  int Rnr;

  Rdelwin(RwinMain);
  Rdelwin(RwinLeft);
  Rdelwin(RwinRight);
  Rdelwin(RwinHead);
  Rdelwin(RwinStatus);
  Rdelwin(RwinLeftHead);
  Rdelwin(RwinLeftList);
  Rdelwin(RwinRightHead);
  Rdelwin(RwinRightList);
  Rdelwin(RwinLeftCol1);
  Rdelwin(RwinLeftCol2);
  Rdelwin(RwinLeftCol3);
  Rdelwin(RwinRightCol1);
  Rdelwin(RwinRightCol2);
  Rdelwin(RwinRightCol3);
  Rdelwin(RwinLeftHeadCol1);
  Rdelwin(RwinLeftHeadCol2);
  Rdelwin(RwinLeftHeadCol3);
  Rdelwin(RwinRightHeadCol1);
  Rdelwin(RwinRightHeadCol2);
  Rdelwin(RwinRightHeadCol3);
  endwin();
  initscr();

  start_color();
  init_pair(1, COLOR_BLACK, COLOR_CYAN);
  init_pair(2, COLOR_BLACK, COLOR_YELLOW);
  init_pair(3, COLOR_BLACK, COLOR_GREEN);
  init_pair(9, COLOR_BLACK, COLOR_GREEN);

  for (Rnr = 0; Rnr < 3; Rnr++) 
  {
    init_color(Rcolor[Rnr].realnumber, Rcolor[Rnr].red, Rcolor[Rnr].green, Rcolor[Rnr].blue);
    init_pair(10+Rnr, Rcolor[Rnr].realnumber,COLOR_BLACK);
  }

  curs_set(0);
  noecho();
  start_color();
  RwinMain = newwin(0, 0, 0, 0); 
  getmaxyx(RwinMain, RmainWinY, RmainWinX);
  RxHalf = RmainWinX/2;
  if ((RxHalf * 2) != RmainWinX) 
    Rdx = 1;
  else 
    Rdx = 0;
  RwinLeft = newwin(RmainWinY-2, RxHalf, 1, 0); 
  RwinRight = newwin(RmainWinY-2, RxHalf+Rdx, 1, RxHalf);
  RwinHead = newwin(1, RmainWinX, 0, 0);
  RwinStatus = newwin(1, RmainWinX, RmainWinY-1, 0);
  RwinLeftHead = newwin(1, RxHalf-2, 2, 1);
  RwinRightHead = newwin(1, RxHalf-2+Rdx, 2, RxHalf+1);
  RwinLeftList = newwin(RmainWinY-6, RxHalf-2, 4, 1);
  RwinRightList = newwin(RmainWinY-6, RxHalf-2+Rdx, 4, RxHalf+1);
  RwinLeftTop = newwin(1, RxHalf-2, 1, 1);
  RwinRightTop = newwin(1, RxHalf-2, 1, RxHalf+1);
  RwinLeftHeadCol1 = newwin(1, 1, 2, 1);
  RwinLeftHeadCol2 = newwin(1, RxHalf-10, 2, 3);
  RwinLeftHeadCol3 = newwin(1, 5, 2, RxHalf-6);
  RwinLeftCol1 = newwin(RmainWinY-6, 1, 4, 1);
  RwinLeftCol2 = newwin(RmainWinY-6, RxHalf-10, 4, 3);
  RwinLeftCol3 = newwin(RmainWinY-6, 5, 4, RxHalf-6);
  RwinRightHeadCol1 = newwin(1, 1, 2, RxHalf+1);
  RwinRightHeadCol2 = newwin(1, RxHalf-10+Rdx, 2, RxHalf+3);
  RwinRightHeadCol3 = newwin(1, 5, 2, RmainWinX-6);
  RwinRightCol1 = newwin(RmainWinY-6, 1, 4, RxHalf+1);
  RwinRightCol2 = newwin(RmainWinY-6, RxHalf-10+Rdx, 4, RxHalf+3);
  RwinRightCol3 = newwin(RmainWinY-6, 5, 4, RmainWinX-6);
  getmaxyx(RwinLeftList, RmainWinYlist, RmainWinXlistL);
  getmaxyx(RwinRightList, RmainWinYlist, RmainWinXlistR);
  if (Rredraw) RwinRedraw();
}

void 
RsortList(TTlist **RlistI, int RdoGroup, int RsortMethodI)
{
  char *Ridx1 = NULL;
  char *Ridx2 = NULL;

  char RgetVar(char *Rstr1, char *Rstr2, int Rsize1, int Rsize2, int RsortMethodI)
  {
    int Rti;
    char Rc1, Rc2;
    int Rii;

    switch (RsortMethodI) 
    {
     case 1:
       Rii = Rstrlen(Rstr1);
       if (Rii > Rstrlen(Rstr2)) 
         Rii = Rstrlen(Rstr2);
       for (Rti = 0; Rti < Rii; Rti++) 
       {
         Rc1 = tolower(Rstr1[Rti]);
         Rc2 = tolower(Rstr2[Rti]);
         if (Rc1 > Rc2) 
           return(Rselected?(RsortReverseR?0:1):(RsortReverseL?0:1));
         if (Rc1 < Rc2) 
           return(Rselected?(RsortReverseR?1:0):(RsortReverseL?1:0));
       }
       return(Rselected?(RsortReverseR?1:0):(RsortReverseL?1:0));
     case 2:
       Ridx1 = rindex(Rstr1, '.');
       Ridx2 = rindex(Rstr2, '.');

       if (Ridx1 == NULL) 
         return(Rselected?(RsortReverseR?1:0):(RsortReverseL?1:0));
       if (Ridx2 == NULL) 
         return(Rselected?(RsortReverseR?0:1):(RsortReverseL?0:1));

       Rii = Rstrlen(Ridx1);
       if (Rii > Rstrlen(Ridx2)) 
         Rii = Rstrlen(Ridx2);
       for (Rti = 0; Rti < Rii; Rti++) 
       {
         Rc1 = tolower(Ridx1[Rti]);
         Rc2 = tolower(Ridx2[Rti]);
         if (Rc1 > Rc2) 
           return(Rselected?(RsortReverseR?0:1):(RsortReverseL?0:1));
         if (Rc1 < Rc2) 
           return(Rselected?(RsortReverseR?1:0):(RsortReverseL?1:0));
       }
       return(Rselected?(RsortReverseR?1:0):(RsortReverseL?1:0));
     case 3:
       if (Rsize1 > Rsize2) 
         return(Rselected?(RsortReverseR?1:0):(RsortReverseL?1:0));
       else 
         return(Rselected?(RsortReverseR?0:1):(RsortReverseL?0:1));
    }
    return(-1);
  }

  void 
  RrecSort(TTlist *Rfirst, TTlist *Rlast, int RsortMethod)
  {
    TTlist *RlistT1 = Rfirst;
    TTlist *RlistT2;

    while (RlistT1 != Rlast)
    {
      RlistT2 = Rfirst;
      while (RlistT2 != Rlast)
      {
        if (RgetVar(RlistT2->col2, RlistT1->col2, RlistT2->size, RlistT1->size, RsortMethod)) 
          RrotateTwoRecords(RlistT1, RlistT2);
        RlistT2 = RlistT2->next;
      }
      RlistT1 = RlistT1->next;
    }
  }

  void 
  RpreSort(TTlist *RlistI, int RgroupFilesDirectories, int RsortMethod)
  {
    TTlist *RlistST1;
    TTlist *RlistST2;

    RDBG("smbc: RpreSort start\n");
    RlistST1 = RlistI;
    if (RgroupFilesDirectories)
    {
      while (RlistST1)
      {
        RlistST2 = RlistI;
        while (RlistST2)
        {
          if ((RlistST1->col1 == NULL) || (RlistST2->col1 == NULL))
            kill(0, SIGTERM);
          if (RlistST1->col1[0] == 'D' && RlistST2->col1[0] == 'F') 
            RrotateTwoRecords(RlistST1, RlistST2);
          RlistST2 = RlistST2->next;
        }
        RlistST1 = RlistST1->next;
      }
      RlistST1 = RlistI;
      while (RlistST1->col1[0] == 'D')
      {
        if (RlistST1->next)
          RlistST1 = RlistST1->next;
        else
          break;
      }
      RrecSort(RlistI, RlistST1, RsortMethod);
      RrecSort(RlistST1, RreturnLastTTlist(RlistI), RsortMethod);
    }
    else 
    {
      RrecSort(RlistI, RreturnLastTTlist(RlistI), RsortMethod);
    }
    RDBG("smbc: RpreSort end\n");
  }

  RgoToFirstTTlist(RlistI);
  if (RreturnCountTTlist(*RlistI) < 2) return;
  RpreSort((*RlistI)->next, RdoGroup, RsortMethodI);
  RDBG("smbc: RsortList 5\n");
  RgoToFirstTTlist(RlistI);
  RDBG("smbc: RsortList end\n");
}

void 
RinitList(int Rnr, int Rwhat, PANEL_TYPE Rcol)   // Rcol 0 - left(local)     1 - right(remote)
                                             // Rwat 0 - groups   1 - hosts   2 - shares  3 - dirs
{
  TTgroups *RgroupsT;
  TThosts *RhostsT;
  TTshares *RsharesT;
  TTfiles *RfilesT;

  if (Rcol == PANEL_LEFT) 
  { 
    RDBG("smbc: RinitList LOCAL start\n");

    RfreeAllInTTlist(&RlistLeft);
    RcreateNextOrNewTTlistAndAddData(&RlistLeft, 
                                     "D", "..", NULL, NULL, 
                                     0, 0);
    RDBG("smbc: RinitList LOCAL 1\n");
    RfilesT = RfilesL;

    switch (Rwhat)
    {
      case 0: while (RfilesT)
              {
                RcreateNextOrNewTTlistAndAddData(&RlistLeft, 
                                                 RfilesT->type == RTFILE?"F":"D", RfilesT->filename, 
                                                 RfilesT->sizeShow, RfilesT->filename,
                                                 RfilesT->size, 0);
                RfilesT = RfilesT->next;
              }
              break;
    }
    RgoToFirstTTlist(&RlistLeft);
    RDBG("smbc: RinitList 2\n");
    RsortList(&RlistLeft, RgroupFilesDirectoriesL, RsortMethodL);
    RDBG("smbc: RinitList 3\n");
    RgoToFirstTTlist(&RlistLeft);
    RDBG("smbc: RinitList 4\n");
    RlistTmainLeft = RlistLeft;
  }
  else 
  {
    RDBG("smbc: RinitList REMOTE start Rwhat = "); RDBGi(Rwhat); RDBG("\n");
    RfreeAllInTTlist(&RlistRight);
    RcreateNextOrNewTTlistAndAddData(&RlistRight, 
                                    "D", "..", "", "",
                                    0, 0);
    switch (Rwhat)
    {
      case 0: RgroupsT = Rgroups;
              while (RgroupsT)
              {
                RcreateNextOrNewTTlistAndAddData(&RlistRight, 
                                                 "W", RgroupsT->groupname, "", RgroupsT->groupname,
                                                 0, 0);
                RgroupsT = RgroupsT->next;
              }
              break;
      case 1: RhostsT = Rhosts;
              while (RhostsT)
              {
                RcreateNextOrNewTTlistAndAddData(&RlistRight, 
                                                 "H", RhostsT->hostname, "", RhostsT->hostname, 
                                                 0, 0);

                if (Rstrlen(RhostsT->comment) > 0) 
                {
                  Rstraddtoend(&RlistRight->col2, " | ");
                  Rstraddtoend(&RlistRight->col2, RhostsT->comment);
                }

                RhostsT = RhostsT->next;
              }
              break;
      case 2: RsharesT = Rshares;
              while (RsharesT)
              {
                RcreateNextOrNewTTlistAndAddData(&RlistRight, 
                                                 "S", RsharesT->sharename, "", RsharesT->sharename, 
                                                  0, 0);

                if (Rstrlen(RsharesT->comment) > 0) 
                {
                  Rstraddtoend(&RlistRight->col2, " | ");
                  Rstraddtoend(&RlistRight->col2, RsharesT->comment);
                }

                RsharesT = RsharesT->next;
              }
              break;
      case 3: RfilesT = RfilesR
              ;while (RfilesT)
              {
                RcreateNextOrNewTTlistAndAddData(&RlistRight, 
                                                 RfilesT->type == RTFILE?"F":"D", RfilesT->filename, 
                                                 RfilesT->sizeShow, RfilesT->filename,
                                                 RfilesT->size, 0);

                RfilesT = RfilesT->next;
              }
              break;
    }
    RgoToFirstTTlist(&RlistRight);
    RsortList(&RlistRight, RgroupFilesDirectoriesR, RsortMethodR);
    RgoToFirstTTlist(&RlistRight);
    RlistTmainRight = RlistRight;
  }
  RDBG("smbc: RinitList end\n");
}

void 
RsetColorExt(WINDOW *Rwin, char *Rstr, char Rset)
{
  int Ri, Rmax, Rfind;
  char Rextm[10];
  int Rt = Rstrlen(Rstr);
  TText *RextT = Rext;

  Rextm[0] = 0;
  if (Rt > 10) Rmax = 10;
  else Rmax = Rt;

  Ri = 0; 
  Rfind = 1;
  while ((Ri < Rmax) & Rfind) 
  {
    if ((Ri > 0) && (Rstr[Rt-Ri-1] == '.')) 
    {
      memcpy(Rextm, Rstr+Rt-Ri, Ri+2);
      Rfind = 0;
    }
    Ri++;
  }

  if (Rstrlen(Rextm)) 
  {
    while (RextT) 
    {
      if (RstrCmp(RextT->ext,Rextm)) 
      {
        if (Rset) 
           wattron(Rwin, COLOR_PAIR(10+RextT->colornr));
        else  
           wattroff(Rwin, COLOR_PAIR(10+RextT->colornr));
      }
      RextT = RextT->Text;
    } 
  }
}

void 
RwriteMenuTxt(int Rfirst, int Rwhat, PANEL_TYPE Rcol, int Rselect, TTlist *RlistI)  // Rcol = 0  left
{
  int Ry, Rx, Ri, Rk;
  char *RinfoP = NULL;
  int Rnr;
  TTlist *RlistT;
  
  RDBG("smbc: RwriteMenuTxt start\n");
  Rnr = RreturnCountTTlist(RlistI);

  mvwhline(RwinLeftTop, 0, 0, 0, RxHalf-2);
  mvwhline(RwinRightTop, 0, 0, 0, RxHalf-2);
  getmaxyx(RwinLeftCol2, Ry, Rx); Rk = Ry;
  if ((Rnr-Rfirst) < Ry) 
    Ry = Rnr-Rfirst;
  if (Rcol == PANEL_LEFT) 
  { 
    RDBG("smbc: RwriteMenuTxt 1\n");
    RgoToRecordInTTlistMaster(&RlistT, Rfirst, RlistLeft);

    for (Ri = 0; Ri < Ry; Ri++) 
    {
      if (RlistT->select) 
      { 
        wattron(RwinLeftCol1, COLOR_PAIR(2)); 
        wattron(RwinLeftCol2, COLOR_PAIR(2)); 
        wattron(RwinLeftCol3, COLOR_PAIR(2)); 
      }
      if (Rselected == PANEL_LEFT) 
      {
        if (Ri == Rselect) 
        { 
          wattron(RwinLeftCol1, COLOR_PAIR(1)); 
          wattron(RwinLeftCol2, COLOR_PAIR(1)); 
          wattron(RwinLeftCol3, COLOR_PAIR(1)); 
        }
        if ((RlistT->select) && (Ri == Rselect)) 
        { 
          wattron(RwinLeftCol1, COLOR_PAIR(3)); 
          wattron(RwinLeftCol2, COLOR_PAIR(3)); 
          wattron(RwinLeftCol3, COLOR_PAIR(3)); 
        }
      }
      mvwhline(RwinLeftCol1, Ri, 0, 32, 1);
      mvwhline(RwinLeftCol2, Ri, 0, 32, RxHalf-10);
      mvwhline(RwinLeftCol3, Ri, 0, 32, 5);
      Rmvwprintw(RwinLeftCol1, Ri, 0, RlistT->col1); 

      if ((Ri != Rselect) & (!RlistT->select)) 
         RsetColorExt(RwinLeftCol2, RlistT->col2, 1);
      
      Rmvwprintw(RwinLeftCol2, Ri, 0, to_scr(RlistT->col2)); 
      
      if ((Ri != Rselect) & (!RlistT->select)) 
         RsetColorExt(RwinLeftCol2, RlistT->col2, 0);

      Rmvwprintw(RwinLeftCol3, Ri, 0, to_scr(RlistT->col3)); 
      if (RlistT->select) 
      { 
        wattroff(RwinLeftCol1, COLOR_PAIR(2)); 
        wattroff(RwinLeftCol2, COLOR_PAIR(2)); 
        wattroff(RwinLeftCol3, COLOR_PAIR(2)); 
      }
      if (Ri == Rselect) 
      { 
        wattroff(RwinLeftCol1, COLOR_PAIR(1)); 
        wattroff(RwinLeftCol2, COLOR_PAIR(1)); 
        wattroff(RwinLeftCol3, COLOR_PAIR(1)); 
      }
      if ((RlistT->select) && (Ri == Rselect)) 
      { 
        wattroff(RwinLeftCol1, COLOR_PAIR(3)); 
        wattroff(RwinLeftCol2, COLOR_PAIR(3)); 
        wattroff(RwinLeftCol3, COLOR_PAIR(3)); 
      }
      RlistT = RlistT->next;
    }
    switch (Rwhat)
    {
      case LEVEL_LOCAL: Rmvwprintw(RwinLeftTop, 0, 0, to_scr(RadirL)); 
                        break;
    }
    getmaxyx(RwinLeftCol2, Ry, Rx);
    for (Ri = Rnr-Rfirst; Ri < Ry; Ri++) 
    {
      mvwhline(RwinLeftCol1, Ri, 0, 32, 1);
      mvwhline(RwinLeftCol2, Ri, 0, 32, RxHalf-10);
      mvwhline(RwinLeftCol3, Ri, 0, 32, 5);
    }
    wrefresh(RwinLeftTop);
    wrefresh(RwinLeftCol1);
    wrefresh(RwinLeftCol2);
    wrefresh(RwinLeftCol3);
  }
  else 
  {
    RDBG("smbc: RwriteMenuTxt remote start paint\n");
    RgoToRecordInTTlistMaster(&RlistT, Rfirst, RlistRight);

    for (Ri = 0; Ri < Rk; Ri++) 
    {
      mvwhline(RwinRightCol1, Ri, 0, 32, 1);
      mvwhline(RwinRightCol2, Ri, 0, 32, RxHalf+Rdx-10);
      mvwhline(RwinRightCol3, Ri, 0, 32, 5);
    }
    for (Ri = 0; Ri < Ry; Ri++) 
    {
      if (RlistT->select) 
      { 
        wattron(RwinRightCol1, COLOR_PAIR(2)); 
        wattron(RwinRightCol2, COLOR_PAIR(2)); 
        wattron(RwinRightCol3, COLOR_PAIR(2)); 
      }
      if (Rselected == PANEL_RIGHT) 
      {
        if (Ri == Rselect) 
        { 
          wattron(RwinRightCol1, COLOR_PAIR(1)); 
          wattron(RwinRightCol2, COLOR_PAIR(1)); 
          wattron(RwinRightCol3, COLOR_PAIR(1)); 
        }
        if ((RlistT->select) && (Ri == Rselect)) 
        {
          wattron(RwinRightCol1, COLOR_PAIR(3)); 
          wattron(RwinRightCol2, COLOR_PAIR(3)); 
          wattron(RwinRightCol3, COLOR_PAIR(3)); 
        }
      }
      mvwhline(RwinRightCol1, Ri, 0, 32, 1);                     // make line
      mvwhline(RwinRightCol2, Ri, 0, 32, RxHalf-10+Rdx);
      mvwhline(RwinRightCol3, Ri, 0, 32, 5); 

      Rmvwprintw(RwinRightCol1, Ri, 0, to_scr(RlistT->col1)); 

      if ((Ri != Rselect) & (!RlistT->select)) 
        RsetColorExt(RwinRightCol2, RlistT->col2, 1);
      Rmvwprintw(RwinRightCol2, Ri, 0, to_scr(RlistT->col2)); 
      if ((Ri != Rselect) & (!RlistT->select)) 
        RsetColorExt(RwinRightCol2, RlistT->col2, 0);

      Rmvwprintw(RwinRightCol3, Ri, 0, to_scr(RlistT->col3)); 
      if (RlistT->select) 
      { 
        wattroff(RwinRightCol1, COLOR_PAIR(2)); 
        wattroff(RwinRightCol2, COLOR_PAIR(2)); 
        wattroff(RwinRightCol3, COLOR_PAIR(2)); 
      }
      if (Ri == Rselect) 
      { 
        wattroff(RwinRightCol1, COLOR_PAIR(1)); 
        wattroff(RwinRightCol2, COLOR_PAIR(1)); 
        wattroff(RwinRightCol3, COLOR_PAIR(1)); 
      }
      if ((RlistT->select) && (Ri == Rselect)) 
      { 
        wattroff(RwinRightCol1, COLOR_PAIR(3)); 
        wattroff(RwinRightCol2, COLOR_PAIR(3)); 
        wattroff(RwinRightCol3, COLOR_PAIR(3)); 
      }
      RlistT = RlistT->next;
    }

    RDBG("smbc: RwriteMenuTxt remote 1\n");
    RstrdupN(RinfoP, "//");
    switch (Rwhat) 
    {
      //case 0: break;
      case 1: RDBG("smbc: RwriteMenuTxt remote 2\n");
              Rstraddtoend(&RinfoP, RgetStrVariable(V_GROUP));
              Rstraddtoend(&RinfoP, "/");
              break;
      case 2: RDBG("smbc: RwriteMenuTxt remote 3\n");
              Rstraddtoend(&RinfoP, RgetStrVariable(V_GROUP));
              Rstraddtoend(&RinfoP, "/");
              Rstraddtoend(&RinfoP, Rahost);
              Rstraddtoend(&RinfoP, "/");
              break;
      case 3: RDBG("smbc: RwriteMenuTxt remote 4\n");
              Rstraddtoend(&RinfoP, RgetStrVariable(V_GROUP));
              RDBG("smbc: RwriteMenuTxt remote 4.1\n");
              Rstraddtoend(&RinfoP, "/");
              Rstraddtoend(&RinfoP, Rahost);
              RDBG("smbc: RwriteMenuTxt remote 4.2 Radir = "); RDBG(Radir); RDBG("\n");
              Rstraddtoend(&RinfoP, "/");
              Rstraddtoend(&RinfoP, Radir);
              RDBG("smbc: RwriteMenuTxt remote 4.3\n");
              break; 
    }
    RDBG("smbc: RwriteMenuTxt remote 5\n");
    Rmvwprintw(RwinRightTop,0,0, to_scr(RinfoP));
    Rfree(RinfoP);

    getmaxyx(RwinLeftCol2, Ry, Rx);
    for (Ri = Rnr-Rfirst; Ri < Ry; Ri++) 
    {
      mvwhline(RwinRightCol1, Ri, 0, 32, 1);
      mvwhline(RwinRightCol2, Ri, 0, 32, RxHalf+Rdx-10);
      mvwhline(RwinRightCol3, Ri, 0, 32, 5);
    }
    wrefresh(RwinRightTop);
    wrefresh(RwinRightCol1);
    wrefresh(RwinRightCol2);
    wrefresh(RwinRightCol3);
  }
  curs_set(0);
  RDBG("smbc: RwriteMenuTxt finished\n");
}

char * 
RgetSmallPath(char *Rhost)
{
  char *Rdst = NULL;

  RstrdupN(Rdst, "smb://");
  Rstraddtoend(&Rdst, Rhost);
  Rstraddtoend(&Rdst, "/");
  return(Rdst);
}

void 
RdoWindowResize()
{
  if (RdoResize) 
  {
    RdoResize = 0;

    RleftPos = 0;
    RrightPos = 0;
    RleftFirst = 0;
    RrightFirst = 0;

    RwriteMenu(1);
    RwriteMenu(1);
    RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
    RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
    keypad(RwinMain, TRUE);
    nodelay(RwinMain,1);
    RactiveWindow = RwinMain;
    RDBG("smbc : RdoWindowResize RactiveWindow = RwinMain  2\n");
  }
}

void 
RresizeHandler(int sig) 
{
  RdoResize = 1;
  RdoResizeFind = 1;
  RdoResizeDownload = 1;
  RdoResizeBookmark = 1;
}

int 
RdownloadUpload()
{
  #define RstrSLen 1000
  int Ri;
  TTdownload *RdownloadListTmp = RdownloadList;
  char *Rstr = NULL;
  char *Rstr2 = NULL;
  char *Rstr3 = NULL;
  char *Rbuf = NULL;
  struct stat Rlocalstat, Rremotestat;
  char RstrS[RstrSLen];
  off_t R2, R1;
  ssize_t Rbread21, Rbread22;
  int RbufSize;
  char *Rbuf21 = NULL;
  char *Rbuf22 = NULL;
  char *RstrT1 = NULL;
  int Rerr;
  ssize_t Rbread;
  time_t Rtt;
  long int RtimeRt;
  int Rbwrite;

  alarm(0);

  RDBG("smbc: RdownloadUpload start\n");
  RDBG("smbc: RdownloadUpload RgetStrVariable(V_GROUP) start = |"); RDBG(RgetStrVariable(V_GROUP)); RDBG("|\n");
  
  void 
  RremoveActFromListAndCloseFiles()
  {
    if (RdownloadListAct->upload == 'D') 
    {
      Rsmbc_close(RdownloadHandleRD);
      close(RdownloadHandleWR);
    }
    else 
    {
      Rsmbc_close(RdownloadHandleWR);
      close(RdownloadHandleRD);
    }
    RfreeOneTTdownloadAndGoToFirst(&RdownloadListAct);
    RdownloadList = RdownloadListAct;
    RdownloadListAct = NULL;

    RdownloadHandleWR = -1;
    RdownloadHandleRD = -1;
  }

  int 
  Rchoice(int Rin, int *Rch) 
  {
    switch (Rin) 
    {
      case 0 : RremoveActFromListAndCloseFiles();    // cancel
               alarm(RCalarm);
               return(0);
      case 1 : if (RdownloadListAct->upload == 'D')    // overwrite
               {
                 close(RdownloadHandleWR);              
                 remove(RdownloadListAct->fullfilename);
                 RdownloadHandleWR = open(to_sys(RdownloadListAct->fullfilename), O_CREAT | O_NONBLOCK | O_RDWR, 0755);
               }
               else 
               {
                 Rsmbc_close(RdownloadHandleWR);              
                 Rsmbc_unlink(RdownloadListAct->fullfilename);
                 RcopyInterToStOrOtherWay(VI_ST_TO_INTER);
                 RsetTwoUserAndPassword(VV_USER_AND_PASS, RdownloadListAct->username, RdownloadListAct->password);
                 RdownloadHandleWR = Rsmbc_open(RdownloadListAct->fullfilename, O_CREAT | O_NONBLOCK | O_RDWR, 0755);
                 RcopyInterToStOrOtherWay(VI_INTER_TO_ST);
                 RsetTwoUserAndPassword(VV_USER_AND_PASS_INTER, NULL, NULL);
               }
               if ((fstat(RdownloadHandleWR, &Rlocalstat) < 0) && (RdownloadListAct->upload == 'D')) 
               {
                 Rsmbc_close(RdownloadHandleRD);
                 close(RdownloadHandleWR);
               }
               if ((Rsmbc_fstat(RdownloadHandleWR, &Rlocalstat) < 0) && (RdownloadListAct->upload == 'U')) 
               { 
                 Rsmbc_close(RdownloadHandleWR);
                 close(RdownloadHandleRD);
               }
               RdownloadHandleWR = -1;
               RdownloadHandleRD = -1;
               alarm(RCalarm);
               return(1);
      case 2 : RdownloadListAct->active = 'W';            // wait
               if (RdownloadListAct->upload == 'U') 
               {
                 Rsmbc_close(RdownloadHandleRD);
                 close(RdownloadHandleWR);
               }
               else 
               {
                 Rsmbc_close(RdownloadHandleWR);
                 close(RdownloadHandleRD);
               }
               RdownloadHandleWR = -1;
               RdownloadHandleRD = -1;
               RdownloadListAct = NULL;
               alarm(RCalarm);
               return(0);
      case 4 : *Rch = 0;        // retry without asking
      case 3 : if (RdownloadListAct->upload == 'U')    // retry
               {
                 Rsmbc_close(RdownloadHandleRD);
                 close(RdownloadHandleWR);
               }
               else 
               {
                 Rsmbc_close(RdownloadHandleWR);
                 close(RdownloadHandleRD);
               }
               RdownloadHandleWR = -1;
               RdownloadHandleRD = -1;
               RdownloadListAct = NULL;
               alarm(RCalarm);
               return(0);
     }
    alarm(RCalarm);
    return(1);
  }
 
  if (RdownloadListTmp == NULL) 
  {
     RDBG("smbc: RdownloadUpload 1\n");
     RopenSaveDownloadList(1);
     RAstatusBar(STATUS_ADDING_FILES_TO_UPLOAD_LIST);
     alarm(RCalarm);
     return(0);
  }
  else 
  {
    RDBG("smbc: RdownloadUpload 1_\n");
    if (RdownloadHandleRD == -1) 
    {
      RactuallyNothingDownloadUpload = 1;
      RdownloadListAct = NULL; 
      while ((RdownloadHandleRD < 0) & (RdownloadListTmp != NULL)) 
      {
        if ((RdownloadListTmp->active == 'X') && (time(NULL) > RdownloadListTmp->nextRetry)) 
        {
          if (RdownloadListTmp->upload == 'D') 
          {
            Rstr = RreturnFullSmbPath(FULLPATH_WITH_SMB, 
                                      "",
                                      RdownloadListTmp->hostname,
                                      RdownloadListTmp->pathR,
                                      RdownloadListTmp->filename);

            /* must be convert to to_scr, internal UTF-8 */
            RAstatusBarStr(to_scr(Rstr+4), 1);
            RcopyInterToStOrOtherWay(VI_ST_TO_INTER);
            RsetTwoUserAndPassword(VV_USER_AND_PASS, RdownloadListTmp->username, RdownloadListTmp->password);
            RdownloadHandleWR = Rsmbc_open(RdownloadListTmp->fullfilename, O_CREAT | O_NONBLOCK | O_RDWR, 0755);
            RdownloadHandleRD = Rsmbc_open(Rstr, O_RDONLY, 0755);
            RcopyInterToStOrOtherWay(VI_INTER_TO_ST);
            RsetTwoUserAndPassword(VV_USER_AND_PASS_INTER, NULL, NULL);
          }
          else 
          {
            RstrdupN(Rstr, RdownloadListTmp->pathL);
            Rstraddtoend(&Rstr, RdownloadListTmp->filename);
            RdownloadHandleRD = open(to_sys(Rstr), O_RDONLY, 0755);
          }
          if(RdownloadHandleRD < 0) 
          {

             /*  
                 Rm232 - output
                 Rstr  - UTF-8
                 Rm233 - output
                 Rstr3 - output
             */
             RreturnHumanDelayTime(&Rstr3, RdownloadListTmp->delayRetry);
             RstrdupN(Rstr2, Rm232);
             Rstraddtoend(&Rstr2, to_scr(Rstr));
             Rstraddtoend(&Rstr2, Rm233);
             Rstraddtoend(&Rstr2, Rstr3);

             RAstatusBarStr(Rstr2, 0);
             Rfree(Rstr2);
             Rfree(Rstr3);

             RdownloadListTmp->nextRetry = time(NULL)+RdownloadListTmp->delayRetry;
             if (RdownloadListTmp->next == NULL) {
               RdownloadListAct = NULL;
             }
             RdownloadListTmp = RdownloadListTmp->next;

             RDBG("smbc: RdownloadUpload return pre 1 last\n");
             alarm(RCalarm);
             return(0);
          }
          RDBG("!!smbc : RdownloadUpload 1.1 Rstr = "); RDBG(Rstr); RDBG("|\n");
          Rfree(Rstr);
        }
        else 
        {
          RdownloadListTmp = RdownloadListTmp->next;
        }
      }
      if (RdownloadHandleRD > 0) 
        RdownloadListAct = RdownloadListTmp;
    }
    else 
    {
       RDBG("smbc: RdownloadUpload 2\n");
       if ((RdownloadHandleWR == -1) &
           (RdownloadHandleRD != -1)) 
       {
         RDBG("smbc: RdownloadUpload 3\n");
         RactuallyNothingDownloadUpload = 1;
         if (RdownloadListAct->upload == 'D') 
         // download
         {
           RstrdupN(Rstr, RdownloadListAct->pathL);
           if (Rstrlen(RdownloadListAct->pathRLbase) != Rstrlen(RdownloadListAct->pathR)) 
             Rstraddtoend(&Rstr, RdownloadListAct->pathR+Rstrlen(RdownloadListAct->pathRLbase));

           Rstraddtoend(&Rstr, RdownloadListAct->filename);

           for (Ri = 0; Ri < Rstrlen(Rstr); Ri++) 
           {
             if (Rstr[Ri] == '/') 
             {
               RstrdupN(Rstr2, Rstr);
               RtruncateStr(&Rstr2, Ri);
               mkdir(to_sys(Rstr2), 0755);
               Rfree(Rstr2);
             }
           }
           
           RdownloadHandleWR = open(to_sys(Rstr), O_CREAT | O_NONBLOCK | O_RDWR, 0755);
           RstrdupN(RdownloadListAct->fullfilename, Rstr);
           Rfree(Rstr);
         }
         else 
         // upload
         {
            RactuallyNothingDownloadUpload = 0;

            /*
             *                                        RdownloadListAct->pathL - UTF-8
             *   RdownloadListAct->pathR+Rstrlen(RdownloadListAct->pathRLbase) - UTF-8
             *                                     RdownloadListAct->filename - UTF-8
             */
            Rstr = RreturnFullSmbPath(FULLPATH_WITH_SMB, 
                                      "",
                                      RdownloadListAct->hostname,
                                      RdownloadListAct->pathR,
                                      ""); 
            RDBG("smbc: RdownloadUpload Rstr = "); RDBG(Rstr); RDBG("\n");
            RDBG("smbc: RdownloadUpload RdownloadListAct->pathRLbase = "); RDBG(RdownloadListAct->pathRLbase); RDBG("\n");
            RDBG("smbc: RdownloadUpload RdownloadListAct->pathR = "); RDBG(RdownloadListAct->pathR); RDBG("\n");
            if (Rstrlen(RdownloadListAct->pathRLbase) != Rstrlen(RdownloadListAct->pathL)) 
              Rstraddtoend(&Rstr, RdownloadListAct->pathL+Rstrlen(RdownloadListAct->pathRLbase));

            Rstraddtoend(&Rstr, RdownloadListAct->filename);
            RDBG("smbc: RdownloadUpload Rstr = "); RDBG(Rstr); RDBG("\n");

            for (Ri = 0; Ri < Rstrlen(Rstr); Ri++) 
            {
              if (Rstr[Ri] == '/') 
              {
                RstrdupN(Rstr2, Rstr);
                RtruncateStr(&Rstr2, Ri);
                Rsmbc_mkdir(to_sys(Rstr2), 0755);
                Rfree(Rstr2);
              }
            }

            /*
             *   Rstr - UTF-8
             */
            RAstatusBarStr(to_scr(Rstr+4), 1);

            RcopyInterToStOrOtherWay(VI_ST_TO_INTER);

            /*
             *   RdownloadListAct->username - UTF-8
             *   RdownloadListAct->password - UTF-8
             *    libsmbclient input/output - UTF-8
             */
            RsetTwoUserAndPassword(VV_USER_AND_PASS, RdownloadListAct->username, RdownloadListAct->password);
            RdownloadHandleWR = Rsmbc_open(Rstr, O_CREAT | O_NONBLOCK | O_RDWR, 0755);
            RcopyInterToStOrOtherWay(VI_INTER_TO_ST);
            RsetTwoUserAndPassword(VV_USER_AND_PASS_INTER, NULL, NULL);

            RstrdupN(RdownloadListAct->fullfilename, Rstr);
            Rfree(Rstr);
         }
         if (RdownloadHandleWR < 0) {
           RdownloadListAct->nextRetry = time(NULL)+RdownloadListAct->delayRetry;
           RdownloadHandleWR = -1;
           alarm(RCalarm);
           return(0);
         }
       }
       else 
       {
         RDBG("smbc: RdownloadUpload 4\n");
         RactuallyNothingDownloadUpload = 0;
         if (RdownloadListAct->upload == 'D') 
         {
           RDBG("smbc: RdownloadUpload 5\n");
           if ((Rsmbc_fstat(RdownloadHandleRD, &Rremotestat) < 0) || (fstat(RdownloadHandleWR,&Rlocalstat) < 0)) 
           {  
             Rsmbc_close(RdownloadHandleRD);
             close(RdownloadHandleWR);
             RdownloadHandleWR = -1;
             RdownloadHandleRD = -1;
           }
         }
         else 
         {
           RDBG("smbc: RdownloadUpload 6\n");
           if ((fstat(RdownloadHandleRD,&Rremotestat) < 0) || (Rsmbc_fstat(RdownloadHandleWR,&Rlocalstat) < 0)) 
           {  
             close(RdownloadHandleRD);
             Rsmbc_close(RdownloadHandleWR);
             RdownloadHandleWR = -1;
             RdownloadHandleRD = -1;
           }
         }
         RDBG("smbc: RdownloadUpload 7\n");

         if ((int)Rlocalstat.st_size < (int)Rremotestat.st_size) 
         {
           RgetHumanSizeMalloc(&(RdownloadListAct->sizeDisplayDownR), TdownloadsizeDisplayRLen-1, (int)Rremotestat.st_size-Rlocalstat.st_size);
           RDBG("wmbc : RdownloadUpload 6.1 RdownloadListAct->sizeDisplayDownR = "); RDBG(RdownloadListAct->sizeDisplayDownR); RDBG("|\n");

           if ((int)(Rremotestat.st_size)/1024 > 0) 
           {
              itoaMalloc(&(RdownloadListAct->percent), TdownloadpercentLen, (int)100*(Rlocalstat.st_size/1024)/(Rremotestat.st_size/1024));
           }
           else 
             RstrdupN(RdownloadListAct->percent, "0");
           itoa(RstrS, RstrSLen, RgetdownloadListCount(RdownloadList));

           // X, downloading(uploading) xx/s, remaing
           /*
            *   Rm234 - to_scr
                Rm235 - to_scr
            */
           sprintf(RstrS+Rstrlen(RstrS), ", %s ", (RdownloadListAct->upload == 'D'?Rm234:Rm235));

           RDBG("smbc: RdownloadUpload 9.1 Rstrlen(RstrS) = "); RDBGi(Rstrlen(RstrS)); RDBG("\n");
           RDBG("smbc: RdownloadUpload 9.2 RavrSizeDisplay = "); RDBGi(RavrSizeDisplay); RDBG("\n");

           RgetHumanSize(RstrS+Rstrlen(RstrS), RstrSLen-Rstrlen(RstrS)>5?5:RstrSLen-Rstrlen(RstrS), RavrSizeDisplay);

           RDBG("smbc: RdownloadUpload 9.3 Rstrlen(RstrS) = "); RDBGi(Rstrlen(RstrS)); RDBG("\n");
           /*
            *   Rm236 - to_scr
            */
           Rstrcpy(RstrS+Rstrlen(RstrS), RstrSLen, Rm236, -1);
           RDBG("smbc: RdownloadUpload 8.2.1 RstrS = "); RDBG(RstrS); RDBG("\n");

           // xxs,
           /* 
            *   RdownloadListAct->sizeDisplayDownR - number
            */
           Rstrcpy(RstrS+Rstrlen(RstrS), RstrSLen, RdownloadListAct->sizeDisplayDownR, TdownloadsizeDisplayDownR);
           Rstrcpy(RstrS+Rstrlen(RstrS), RstrSLen, ", ", 3);
           RDBG("smbc: RdownloadUpload 9.4 RstrS = "); RDBG(RstrS); RDBG("|\n");
          
           RDBG("smbc: RdownloadUpload 10\n");
           // time
           if (RavrSizeDisplay > 0)
             RtimeRt = (Rremotestat.st_size-Rlocalstat.st_size)/(RavrSizeDisplay);
           else 
             RtimeRt = 0;
           RDBG("smbc: RdownloadUpload 10-1\n");
           RtimeR = (RtimeR+RtimeRt)/2;
             RtimeRt = RtimeR % 60;
             RtimeR = RtimeR/60;
           RDBG("smbc: RdownloadUpload 10.2\n");
           RDBG("smbc: RdownloadUpload 10.2 Rstrlen(RstrS) = "); RDBGi(Rstrlen(RstrS)); RDBG("\n");
             itoa(RstrS+Rstrlen(RstrS), RstrSLen-Rstrlen(RstrS), RtimeR);
           RDBG("smbc: RdownloadUpload 10-3\n");
             Rstrcpy(RstrS+Rstrlen(RstrS), RstrSLen, ".", 2);
             itoa(RstrS+Rstrlen(RstrS), RstrSLen-Rstrlen(RstrS), RtimeRt);
             /*
              *   Em237 - to_scr
              */
             Rstrcpy(RstrS+Rstrlen(RstrS), RstrSLen, Rm237, -1);

           // percent
           /* 
            *   RdownloadListAct->percent - number
                                    Rm231 - to_scr
               RdownloadListAct->filename - UTF-8
            */
           memcpy(RstrS+Rstrlen(RstrS), RdownloadListAct->percent, Rstrlen(RdownloadListAct->percent)+1);
           memcpy(RstrS+Rstrlen(RstrS), "%", 2);
           memcpy(RstrS+Rstrlen(RstrS), Rm231, Rstrlen(Rm231)+1);
           RstrT1 = to_scr(RdownloadListAct->filename);
           memcpy(RstrS+Rstrlen(RstrS), RstrT1, Rstrlen(RstrT1)+1);
           RDBG("smbc: RdownloadUpload 8.2.1 RstrS = "); RDBG(RstrS); RDBG("\n");

           /*
            * insert spaces to remaing space into status bar line
            */
           Ri = RmainWinX - Rstrlen(RstrS);
           if (Ri > 0) 
           {
             memset(RstrS+Rstrlen(RstrS), ' ', Ri);
             RstrS[RmainWinX] = 0;
           }
           RAstatusBarStr(RstrS, 0);
           RDBG("smbc: RdownloadUpload 9.4.1 RstrS = "); RDBG(RstrS); RDBG("\n");

           if ((int)Rlocalstat.st_size > 0) 
           {
             if ((int)Rlocalstat.st_size > 1024) 
               RbufSize = 1024; 
             else 
               RbufSize = (int)Rlocalstat.st_size;
             Rbuf21 = Rmalloc(RbufSize);
             Rbuf22 = Rmalloc(RbufSize);
             Ri = (int)Rlocalstat.st_size-RbufSize;
             if (RdownloadListAct->upload == 'D') 
             {
               R2 = lseek(RdownloadHandleWR, Ri, SEEK_SET);
               R1 = Rsmbc_lseek(RdownloadHandleRD, Ri, SEEK_SET);
               RDBG("smbc : RdownloadUpload 1 R1 == "); RDBGi(R1); RDBG("\n");
               RDBG("smbc : RdownloadUpload 1 R2 == "); RDBGi(R2); RDBG("\n");
               Rbread21 = Rsmbc_read(RdownloadHandleRD, Rbuf21, RbufSize);
               Rbread22 = read(RdownloadHandleWR, Rbuf22, RbufSize);
             }
             else 
             {
               R2 = lseek(RdownloadHandleRD, Ri, SEEK_SET);
               R1 = Rsmbc_lseek(RdownloadHandleWR, Ri, SEEK_SET);
               RDBG("smbc : RdownloadUpload 2 R1 == "); RDBGi(R1); RDBG("\n");
               RDBG("smbc : RdownloadUpload 2 R2 == "); RDBGi(R2); RDBG("\n");
               Rbread21 = read(RdownloadHandleRD, Rbuf22, RbufSize);
               Rbread22 = Rsmbc_read(RdownloadHandleWR, Rbuf21, RbufSize);
             }
             RDBG("smbc : RdownloadUpload 2 Rbread21 == "); RDBGi(Rbread21); RDBG("\n");
             RDBG("smbc : RdownloadUpload 2 Rbread22 == "); RDBGi(Rbread22); RDBG("\n");
             Rerr = 0;
             if ((Rbread21 == Rbread22) && (Rbread21 >= 0) && (Rbread21 >= 0)) 
             {
               RDBG("smbc : RdownloadUpload com 1\n");
               if (memcmp(Rbuf21, Rbuf22, Rbread21) != 0) 
               {
                 Rerr = 1;
                 RDBG("smbc : RdownloadUpload com 1.1\n");
               }
             }
             else 
             {
               RDBG("smbc : RdownloadUpload com 2\n");
               if (Rbread21 != RbufSize) 
                 Rerr = 2;
               else 
                 Rerr = 3;
             }
             if (R1 != R2) 
             {
               Rerr = 2;
               RDBG("smbc : RdownloadUpload com 3\n");
             }
             Rfree(Rbuf21);
             Rfree(Rbuf22);
             RDBG("smbc : RdownloadUpload 1 Rerr == "); RDBGi(Rerr); RDBG("\n");
             if (Rerr) 
             {
               RDBG("smbc : RdownloadUpload 2\n");         // Rmsg38 read
               if (!Rchoice(RaskRetry == 1?
                    RshowMessage((Rerr == 1?INFO_FILES_FROM_X_TO_Y_ARE_DIFFERENT:INFO_WARNING_CANT_X_Y_CONNECTION_BROKEN),
                                 (Rerr == 2?Rm238:Rm239), "", RdownloadListAct, DO_REDRAW_MAIN_WINDOW):4, &RaskRetry)
                  )
               {
                 RDBG("smbc: RdownloadUpload return 1\n");
                 alarm(RCalarm);
                 return(0);
               }
             }
           } 
           RDBG("smbc : RdownloadUpload 3\n");
           Rbuf = Rmalloc(64000);
           if (RdownloadListAct->upload == 'D') 
           {
             lseek(RdownloadHandleWR, Rlocalstat.st_size, SEEK_SET);
             Rsmbc_lseek(RdownloadHandleRD, Rlocalstat.st_size, SEEK_SET);

             Rbread = Rsmbc_read(RdownloadHandleRD, Rbuf, 64000);
             Rbwrite = write(RdownloadHandleWR, Rbuf, Rbread);

             if (Rbwrite != Rbread) 
               if (!Rchoice(RaskRetryDiskFull == 1?
                            Rerr = RshowMessage(INFO_PROBABLY_X_IS_FULL, Rm240, NULL, NULL, DO_REDRAW_MAIN_WINDOW):
                            4, 
                            &RaskRetryDiskFull)) {
                 Rfree(Rbuf);
                 RDBG("smbc: RdownloadUpload return 2\n");
                 alarm(RCalarm);
                 return(0);
               }
           }
           else 
           {
             lseek(RdownloadHandleRD, Rlocalstat.st_size, SEEK_SET);
             Rsmbc_lseek(RdownloadHandleWR, Rlocalstat.st_size, SEEK_SET);

             Rbread = read(RdownloadHandleRD, Rbuf, 64000);
             Rbwrite = Rsmbc_write(RdownloadHandleWR, Rbuf, Rbread);

             if (Rbwrite != Rbread) 
               if (!Rchoice(RaskRetryDiskFull == 1?
                    Rerr = RshowMessage(INFO_PROBABLY_X_IS_FULL, Rm241, NULL, NULL, DO_REDRAW_MAIN_WINDOW):
                    4, 
                    &RaskRetryDiskFull)) 
               {
                 Rfree(Rbuf);
                 RDBG("smbc: RdownloadUpload return 3\n");
                 alarm(RCalarm);
                 return(0);
               }
           }

           RDBG("smbc : RdownloadUpload 4\n");
           Rtt = time(NULL);
           if (RavrTime != Rtt) 
           {
             RavrTime = Rtt;
             RavrSizeDisplay = (10*RavrSizeDisplay+RavrSize)/11;
             RavrSize = Rbread;
           } 
           else 
             RavrSize+= Rbread;

           Rfree(Rbuf);
         } 
         else RremoveActFromListAndCloseFiles();
       }
    }
  }
  RDBG("smbc: RdownloadUpload RgetStrVariable(V_GROUP) = |"); RDBG(RgetStrVariable(V_GROUP)); RDBG("|\n");
  RDBG("smbc: RdownloadUpload return last\n");
  alarm(RCalarm);
  return(0);
}

int 
RcreateFileFromDirR(char *RadirL, char *RadirR, char *Rfilename, char *Rworkgroup, char *Rhostname, char Rtype)
{
  int Rys = 6;
  int Rxs = 78;
  WINDOW *RwinAdd = NULL;
  char *RstrLocalCF = NULL;

  void 
  RsmbDownloadFile(char *RpathToFile, char *Rfilename)
  {
    int Rfilehandle;
    struct stat Rremotestat;
    char *RstrLocal = NULL;

    RstrLocal = RreturnFullSmbPath(FULLPATH_WITH_SMB, "", Rahost, RpathToFile, Rfilename);

    RDBG("smbc: RcreateFileFromDirR RsmbDownloadDir Rname = "); RDBG(RstrLocal); RDBG("\n");
    Rfilehandle = Rsmbc_open(RstrLocal, O_RDONLY, 0755);
    if (Rsmbc_fstat(Rfilehandle, &Rremotestat) >= 0) 
    {
      RaddOneTTdownloadToLast(&RdownloadList,
                              RgetStrVariable(V_USERNAME), RgetStrVariable(V_PASSWORD), 
                              Rworkgroup, Rhostname, RadirR, RpathToFile, RadirL,
                              "", Rfilename, Rremotestat.st_size,
                              NULL, NULL, 
                              "0", 'X', 'D',
                              RdefaultDelayRetry, time(NULL));

      RgetHumanSizeMalloc(&(RdownloadList->sizeDisplayR), TdownloadsizeDisplayRLen, Rremotestat.st_size);
      RgetHumanSizeMalloc(&(RdownloadList->sizeDisplayDownR), TdownloadsizeDisplayDownR, Rremotestat.st_size);
    }
    Rfree(RstrLocal);
    Rsmbc_close(Rfilehandle);
  }

  int 
  RsmbDownloadDir(char *RpathTo)
  {
    struct smbc_dirent *Rdirent = NULL;
    int Rdirhandle;
    char *RnewName = NULL;
    char *RstrLocal = NULL;
    
    RDBG("smbc: RcreateFileFromDirR RsmbDownloadDir RpathTo = "); RDBG(RpathTo); RDBG("\n");
    RstrLocal = RreturnFullSmbPath(FULLPATH_WITH_SMB, "", Rahost, RpathTo, "");
    Rdirhandle = Rsmbc_opendir(RstrLocal);
    if (Rdirhandle < 1) 
      return(-1);
    while(Rsmbc_readdir(Rdirhandle, &Rdirent)) 
    {
      if (RstrCmp(Rdirent->name, ".") || RstrCmp(Rdirent->name, "..")) 
        continue;
      switch(Rdirent->smbc_type) 
      {
        case SMBC_DIR:
             RstrdupN(RnewName, RpathTo); 
             Rstraddtoend(&RnewName, Rdirent->name); 
             Rstraddtoend(&RnewName, "/"); 
             RsmbDownloadDir(RnewName);
             Rfree(RnewName);
             break;
        case SMBC_FILE:
             RsmbDownloadFile(RpathTo, Rdirent->name);
             break;
      }
    }
    Rfree(RstrLocal);
    Rsmbc_closedir(Rdirhandle);
    return 1;
  } 

  RDBG("smbc: RcreateFileFromDirR RadirL = "); RDBG(RadirL);
  RDBG(", RadirR = "); RDBG(RadirR); RDBG(", Rfilename = "); RDBG(Rfilename);
  RDBG(", Rworkgroup = "); RDBG(Rworkgroup); RDBG(", Rhostname = "); RDBG(Rhostname);
  RDBG(", Rtype = "); RDBGc(Rtype); RDBG("\n");

  RwinAdd = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
  box(RwinAdd, 0, 0);
  Rmvwprintw(RwinAdd, 1, Rxs/2-Rstrlen(Rm58)/2, Rm58);
  wrefresh(RwinAdd);

  if (Rtype == 'D') 
  {
    RstrdupN(RstrLocalCF, RadirR);
    Rstraddtoend(&RstrLocalCF, Rfilename);
    Rcharaddtoend(&RstrLocalCF, '/');
    RsmbDownloadDir(RstrLocalCF);
    Rfree(RstrLocalCF);
  }
  else 
    RsmbDownloadFile(RadirR, Rfilename); 

  Rdelwin(RwinAdd);
  RdownloadList = RgetdownloadListFirst(RdownloadList);
  return(0);
}

int 
RcreateFileFromDirL(char *RadirL, char *RadirR, char *Rfilename, char *Rworkgroup, char *Rhostname, char Rtype)
{
  char *RnewNameR = NULL;
  int Rys = 6;
  int Rxs = 78;
  WINDOW *RwinAdd = NULL;

  void RsmbDownloadFile(char *RpathToFile, char *Rfilename)
  {
    int Rfilehandle;
    struct stat Rlocalstat;
    char *RstrLocal = NULL;

    RstrdupN(RstrLocal, RpathToFile);
    Rstraddtoend(&RstrLocal, Rfilename);
    Rfilehandle = open(to_sys(RstrLocal), O_RDONLY, 0755);
    if (fstat(Rfilehandle, &Rlocalstat) >= 0) 
    {
      RaddOneTTdownloadToLast(&RdownloadList,
                              RgetStrVariable(V_USERNAME), RgetStrVariable(V_PASSWORD),
                              Rworkgroup, Rhostname, RadirL, RadirR, RpathToFile,
                              "", Rfilename, Rlocalstat.st_size,
                              NULL, NULL, 
                              "0", 'X', 'U',
                              RdefaultDelayRetry, time(NULL));

      RgetHumanSizeMalloc(&(RdownloadList->sizeDisplayR), TdownloadsizeDisplayRLen, Rlocalstat.st_size);
      RgetHumanSizeMalloc(&(RdownloadList->sizeDisplayDownR), TdownloadsizeDisplayDownR, Rlocalstat.st_size);
    }
    Rfree(RstrLocal);
    close(Rfilehandle);
  }

  int 
  RsmbDownloadDir(char *Rname)
  {
    struct dirent *Rdirent;
    DIR *Rdirhandle;
    char *RnewName = NULL;
    char *RstrT1 = NULL;
    
    Rdirhandle = opendir(to_sys(Rname));
    if (Rdirhandle == NULL) 
      return(-1);
    while((Rdirent = readdir(Rdirhandle))) 
    {
      RstrT1 = from_sys(Rdirent->d_name);
      if (RstrCmp(RstrT1, ".") || RstrCmp(RstrT1, "..")) 
         continue;
      switch(Rdirent->d_type) 
      {
        case DT_DIR:
             RstrdupN(RnewName, Rname); 
             Rstraddtoend(&RnewName, "/"); 
             Rstraddtoend(&RnewName, RstrT1); 
             RsmbDownloadDir(RnewName);
             Rfree(RnewName);
             break;
        default:
             RsmbDownloadFile(Rname, RstrT1);
             break;
      }
    }
    closedir(Rdirhandle);
    return 1;
  } 

  RwinAdd = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
  box(RwinAdd, 0, 0);
  Rmvwprintw(RwinAdd, 1, Rxs/2-Rstrlen(Rm59)/2, to_scr(Rm59));
  wrefresh(RwinAdd);

  if (Rtype == 'D') 
  {
    RstrdupN(RnewNameR, RadirL);
    Rstraddtoend(&RnewNameR, Rfilename);
    Rstraddtoend(&RnewNameR, "/");
    RsmbDownloadDir(RnewNameR);
    Rfree(RnewNameR);
  }
  else 
    RsmbDownloadFile(RadirL, Rfilename); 

  Rdelwin(RwinAdd);
  RdownloadList = RgetdownloadListFirst(RdownloadList);
  return(0);
}

int 
Rreadline(register int fd, register char *ptr, register int maxlen) 
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
    else if (rc == 0) 
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

void 
RreadRcfile()
{

#define RvaL 200
#define RvlL 200

  char *Rfile = NULL;
  int Rfd;
  char Rva[RvaL], Rvl[RvlL];
  char *Rvlp = NULL;
  char Rstr[1000];
  int RstrP;
  char Rtc[20];
  int RstrRvlp, RstrRva;
  int Rline = -1;
  TText *RextT = NULL;
  char Rclr[5];
  int Rwar;

  RstrdupN(Rfile, from_sys(getenv("HOME")));
  Rstraddtoend(&Rfile, "/.smbc/smbcrc");
  if (access(Rfile, F_OK) != 0) 
  {
    printf(Rm242, Rfile);
    printf(Rm243);
    printf(Rm244);
    return;
  }
  Rfd = open(Rfile, O_RDONLY);
  if (!Rfd) 
  {
    printf(Rm245, Rfile);
    exit(0);
  }
  while (Rreadline(Rfd, Rstr, sizeof(Rstr))) 
  {
    Rline++;
    RstrP = -1;
    Rva[0] = 0; Rvl[0] = 0;
    while (Rstr[++RstrP]) 
    {
       if (Rstr[RstrP] == ' ') 
       {
         memcpy(Rva, Rstr, RstrP); memcpy(Rva+RstrP, "\0", 1);
         memcpy(Rvl, Rstr+RstrP+1, Rstrlen(Rstr)-RstrP-1);
         if (Rstr[Rstrlen(Rstr)] == '\n') 
            memcpy(Rvl+Rstrlen(Rstr)-RstrP-3, "\0", 1);
         else 
            memcpy(Rvl+Rstrlen(Rstr)-RstrP-2, "\0", 1);
         if ((Rvl[0] == '"') && (Rvl[Rstrlen(Rvl)-1] == '"')) 
         {
           Rvl[Rstrlen(Rvl)-1] = 0;
           Rvlp = Rvl+1;
         }
         else 
           Rvlp = Rvl;
         break;
       }
    } 
    RstrRvlp = Rstrlen(Rvlp);
    RstrRva = Rstrlen(Rva);
    if ((RstrRva > 0) & (RstrRvlp > 0) & (Rva[0] != '#')) 
    {
      if (RstrCmp(Rva, "username")) RsetStrVariable(V_USERNAME, Rvlp); 
      if (RstrCmp(Rva, "password")) RsetStrVariable(V_PASSWORD, Rvlp);
      if (RstrCmp(Rva, "localdir")) RstrdupN(RadirL, Rvlp); 
      if (RstrCmp(Rva, "workgroup")) RsetStrVariable(V_GROUP, Rvlp);
      if (RstrCmp(Rva, "host")) RstrdupN(Rahost, Rvlp);
      if (RstrCmp(Rva, "smbclient")) RstrdupN(Rsmbclient, Rvlp); 

      if (RstrCmp(Rva, "share")) 
      {
        RstrdupN(Radir, Rvlp);
        RparsingDir(&Radir);
      }

      if (RstrCmp(Rva, "timeout")) Rtimeout = atoi(Rvlp)*1000; 
      if (RstrCmp(Rva, "indextime")) RindexTime = atoi(Rvlp); 
      if (RstrCmp(Rva, "typeofsort-left")) RsortMethodL = atoi(Rvlp); 
      if (RstrCmp(Rva, "typeofsort-right")) RsortMethodR = atoi(Rvlp); 
      if (RstrCmp(Rva, "reversesort-left")) RsortMethodL = atoi(Rvlp); 
      if (RstrCmp(Rva, "reversesort-right")) RsortMethodR = atoi(Rvlp); 
      if (RstrCmp(Rva, "group-left")) RgroupFilesDirectoriesL = atoi(Rvlp); 
      if (RstrCmp(Rva, "group-right")) RgroupFilesDirectoriesR = atoi(Rvlp); 
      if (RstrCmp(Rva, "char-convert")) RDoCharConvertSet(atoi(Rvlp)); 
      if (RstrCmp(Rva, "retry-delay-time")) RdefaultDelayRetry = atoi(Rvlp); 

      if (RstrCmp(Rva, "retry-read-groups")) RretryReadGroups = atoi(Rvlp); 
      if (RstrCmp(Rva, "retry-read-hosts")) RretryReadHosts = atoi(Rvlp); 
      if (RstrCmp(Rva, "retry-read-shares")) RretryReadShares = atoi(Rvlp); 

      if (RstrCmp(Rva, "askquit")) Raskquit = atoi(Rvlp); 
      if (RstrCmp(Rva, "autoresume")) {
        if (RstrCmp(Rvlp, "enable")) RaskRetry = 0; 
        if (RstrCmp(Rvlp, "disable")) RaskRetry = 1; 
      }
      if (RstrCmp(Rva, "warningdiskisfull")) 
      {
        if (RstrCmp(Rvlp, "enable")) RaskRetryDiskFull = 1; 
        if (RstrCmp(Rvlp, "disable")) RaskRetryDiskFull = 0; 
      }

      if ((RstrRva == 6) & (RstrRvlp == 14)) 
      {
        memcpy(Rtc, Rva, 5); Rtc[5] = 0;
        if ((RstrCmp(Rtc, "color")) & (isdigit(Rva[5]) > 0)) 
        {
           Rwar = Rva[5]-48;
           if ((Rwar >= 0) & (Rwar <= 2)) 
           {
             Rclr[4] = 0;
             switch(Rwar) 
             {
               case 0 : Rcolor[Rwar].realnumber = 1;
                        break;
               case 1 : Rcolor[Rwar].realnumber = 4;
                        break;
               case 2 : Rcolor[Rwar].realnumber = 5;
                        break;
             }
             memcpy(Rclr, Rvlp, 4); Rcolor[Rwar].red = atoi(Rclr);
             memcpy(Rclr, Rvlp+5, 4); Rcolor[Rwar].green = atoi(Rclr);
             memcpy(Rclr, Rvlp+10, 4); Rcolor[Rwar].blue = atoi(Rclr);
           }
        }
      }
      if ((RstrRva == 9) & (RstrRvlp > 0) & (RstrRvlp <= 10)) 
      {
        memcpy(Rtc, Rva, 8); Rtc[8] = 0;
        if ((RstrCmp(Rtc, "extcolor")) & (isdigit(Rva[8]) > 0)) 
        {
           Rwar = Rva[8]-48;
           if ((Rwar >= 0) & (Rwar <= 2)) 
           {
             if (Rext == NULL) {
               Rext = Rmalloc(sizeof(TText));
               Rstrcpy(Rext->ext, EXTLEN, Rvlp, -1);
               Rext->colornr = Rwar;
               Rext->Text = NULL; 
               RextT = Rext;
             }
             else 
             {
               RextT->Text = Rmalloc(sizeof(TText));
               RextT = RextT->Text;
               Rstrcpy(RextT->ext, EXTLEN, Rvlp, -1);
               RextT->colornr = Rwar;
               RextT->Text = NULL;
             }
           }
        }
      }
   
    }
  }
  close(Rfd);
  Rfree(Rfile);
}

void 
RremoveDir(char *Rdir)
{
  DIR *Rdirhandle;
  struct dirent *Rdirent;
  char *RnewName = NULL;
  WINDOW *Rdwin = NULL;
  int Rys = 7;
  int Rxs = 60;
  int RstrS = 0;
  char *RstrT1 = NULL;
  char *RstrT2 = NULL;

  RDBG("smbc: RremoveDir open\n");
  Rdwin = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
  box(Rdwin, 0, 0);
  Rmvwprintw(Rdwin, 2, Rxs/2-Rstrlen(Rm119)/2, Rm119);
  wrefresh(Rdwin);
  Rdirhandle = opendir(to_sys(Rdir));
  while ((Rdirent = readdir(Rdirhandle))) 
  {
     RstrT2 = from_sys(Rdirent->d_name);
     if (RstrCmp(RstrT2, ".") || RstrCmp(RstrT2, "..")) 
       continue;
     RstrdupN(RnewName, Rdir);
     Rstraddtoend(&RnewName, "/");
     Rstraddtoend(&RnewName, RstrT2);
     mvwhline(Rdwin, 4, 1, ' ', Rxs-2);
     if (Rstrlen(RnewName) > Rxs-2) 
       RstrS = Rstrlen(RnewName)-(Rxs-2);
     RstrT1 = to_scr(RnewName+RstrS);
     Rmvwprintw(Rdwin, 4, Rxs/2-Rstrlen(RstrT1)/2, RstrT1); 
     wrefresh(Rdwin);
     switch(Rdirent->d_type) 
     {
       case DT_DIR : RremoveDir(RnewName);
                     break;
       default     : RDBG("smbc: RremoveDir 1 RnewName = "); RDBG(RnewName); RDBG("\n");
                     unlink(to_sys(RnewName));
                     break;
     }
     Rfree(RnewName);
  }
  closedir(Rdirhandle);
  rmdir(to_sys(Rdir));
}

int 
RremoveDirR(char *Rdir)
{
  int Rdirhandle;
  struct smbc_dirent *Rdirent = NULL;
  char *RnewName = NULL;
  char *Rstr = NULL;
  int Rstat = 0;
  WINDOW *Rdwin = NULL;
  int Rys = 7;
  int Rxs = 60;
  int RstrS = 0;
  char *RstrT1 = NULL;

  Rdwin = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
  box(Rdwin, 0, 0);
  Rmvwprintw(Rdwin, 2, Rxs/2-Rstrlen(Rm119)/2, Rm119);
  wrefresh(Rdwin);
  if (Rdir[Rstrlen(Rdir)-1] == '/') 
    RstrdupN(Rstr, Rdir)
  else 
  {
    RstrdupN(Rstr, Rdir);
    Rstraddtoend(&Rstr, "/");
  }
  Rdirhandle = Rsmbc_opendir(Rstr);
  while (Rsmbc_readdir(Rdirhandle, &Rdirent)) 
  {
    if (RstrCmp(Rdirent->name, ".") || RstrCmp(Rdirent->name, "..")) 
      continue;
    RstrdupN(RnewName, Rstr);
    Rstraddtoend(&RnewName, Rdirent->name);
    mvwhline(Rdwin, 4, 1, ' ', Rxs-2);
    if (Rstrlen(RnewName+4) > Rxs-2) 
      RstrS = Rstrlen(RnewName+4)-(Rxs-2);
    RstrT1 = to_scr(RnewName+RstrS+4);
    Rmvwprintw(Rdwin, 4, Rxs/2-Rstrlen(RstrT1)/2, RstrT1); wrefresh(Rdwin);
    switch(Rdirent->smbc_type) 
    {
       case SMBC_DIR : Rstat = RremoveDirR(RnewName);
                       break;
       default       : Rstat = Rsmbc_unlink(RnewName);
                       break;
     }
     Rfree(RnewName);
  
  }
  Rsmbc_closedir(Rdirhandle);
  Rstat = Rsmbc_rmdir(Rdir);
  Rfree(Rstr);
  return(Rstat);
}

void 
RopenSaveDownloadList(int Rnr)  
{
  char *Rfile = NULL;
  char *Rdir = NULL;
  int Rfhandle = 0;
  TTdownload *Rdls;
  DIR *Rdhandle;
  struct dirent *Rsdirent;
  int RdufileNr = 0;
  int RfindF = 1;
  int RfindF2 = 1;
  char *RdufileName = NULL;
  char *RdufileName2 = NULL;
  int Ri;

  RDBG("smbc: RopenSaveDownloadList start\n");
  RstrdupN(Rdir, from_sys(getenv("HOME")));
  Rstraddtoend(&Rdir, "/.smbc/");
  mkdir(Rdir, 0700);
  RDBG("smbc: RopenSaveDownloadList 1\n");
  if (Rnr) 
  {         // read
    Rdhandle = opendir(Rdir);
    if (Rdhandle) 
    {
      RfindF = 1;
      while (RfindF) 
      {
        Rsdirent = readdir(Rdhandle);
        if (Rsdirent > 0) 
        {
          if (Rstrlen(Rsdirent->d_name) >= 6) 
            if (RstrCmpIncludeFirst(Rsdirent->d_name, "dulist")) 
            {
              RstrdupN(RdufileName, from_sys(getenv("HOME")));
              Rstraddtoend(&RdufileName, "/.smbc/");
              Rstraddtoend(&RdufileName, Rsdirent->d_name);
              RfindF = 0;
            }
        }
        else 
          RfindF = 0; 
      }
      closedir(Rdhandle);
    }
    RfreeAllTTdownload(&RdownloadList);
    if (RdufileName) 
    {
      Rfhandle = open(RdufileName, O_NONBLOCK | O_RDONLY, 0755);
      Ri = 0;
      while (Ri != -1)
      {
        RaddOneTTdownloadToLast(&RdownloadList,
                                NULL, NULL,
                                NULL, NULL, NULL, NULL, NULL,
                                NULL, NULL, 0, 
                                NULL, NULL,
                                NULL, 0, 0,
                                0, 0);
        Ri = RreadStr(Rfhandle, &(RdownloadList->username));
        if (Ri != -1) Ri = RreadStr(Rfhandle, &(RdownloadList->password));
        if (Ri != -1) Ri = RreadStr(Rfhandle, &(RdownloadList->groupnameR));
        if (Ri != -1) Ri = RreadStr(Rfhandle, &(RdownloadList->hostname));
        if (Ri != -1) Ri = RreadStr(Rfhandle, &(RdownloadList->pathRLbase));
        if (Ri != -1) Ri = RreadStr(Rfhandle, &(RdownloadList->pathR));
        if (Ri != -1) Ri = RreadStr(Rfhandle, &(RdownloadList->pathL));
        if (Ri != -1) Ri = RreadStr(Rfhandle, &(RdownloadList->fullfilename));
        if (Ri != -1) Ri = RreadStr(Rfhandle, &(RdownloadList->filename));
        if (Ri != -1) Ri = RreadUnsignedInt(Rfhandle, &(RdownloadList->sizeR));
        if (Ri != -1) Ri = RreadStr(Rfhandle, &(RdownloadList->sizeDisplayR));
        if (Ri != -1) Ri = RreadStr(Rfhandle, &(RdownloadList->sizeDisplayDownR));
        if (Ri != -1) Ri = RreadStr(Rfhandle, &(RdownloadList->percent));
        if (Ri != -1) Ri = RreadChar(Rfhandle, &(RdownloadList->active));
        if (Ri != -1) Ri = RreadChar(Rfhandle, &(RdownloadList->upload));
        if (Ri != -1) Ri = RreadTime_t(Rfhandle, &(RdownloadList->delayRetry));
        if (Ri != -1) Ri = RreadTime_t(Rfhandle, &(RdownloadList->nextRetry));

        if (Ri == -1)
          RfreeOneTTdownloadAndGoToFirst(&RdownloadList);
      }
      RdownloadList = RgetdownloadListFirst(RdownloadList);
      close(Rfhandle);
      unlink(to_sys(RdufileName));
      Rfree(Rfile);
      Rfree(RdufileName);
    }
  } 
  else 
  {           // write
    Rdls = RgetdownloadListFirst(RdownloadList);
    RDBG("smbc : RopenSaveDownloadList Rdls == "); RDBGi((int)Rdls); RDBG("\n");
    if (Rdls != NULL) 
    {
      RfindF = 1;
      RdufileNr = 0;
      while (RfindF) 
      {
         RstrdupN(RdufileName, from_sys(getenv("HOME")));
         Rstraddtoend(&RdufileName, "/.smbc/");

         RstrdupN(RdufileName2, "dulist");
         Rintaddtoend(&RdufileName2, RdufileNr);
         Rstraddtoend(&RdufileName, RdufileName2);
         RfindF2 = 1;
         Rdhandle = opendir(Rdir);
         while (RfindF2) 
         {
           Rsdirent = readdir(Rdhandle);
           if (Rsdirent > 0) 
           {
             if (RstrCmp(Rsdirent->d_name, RdufileName2)) 
             {
                RfindF2 = 0;
                RfindF = 1;
             }
           } 
           else 
           {
             RfindF2 = 0;
             RfindF = 0;
           }
         }
         closedir(Rdhandle);
         RdufileNr++;
         if (RfindF)
           Rfree(RdufileName); 
         Rfree(RdufileName2);
      }
      Rfhandle = open(RdufileName, O_CREAT | O_NONBLOCK | O_WRONLY | O_TRUNC, 0644);
      RDBG("smbc: RopenSaveDownloadList 1 RdufileName = "); RDBG(RdufileName); RDBG("\n");
      Rfree(RdufileName);
      while (Rdls) 
      {
        RwriteStr(Rfhandle, Rdls->username);
        RwriteStr(Rfhandle, Rdls->password);
        RwriteStr(Rfhandle, Rdls->groupnameR);
        RwriteStr(Rfhandle, Rdls->hostname);
        RwriteStr(Rfhandle, Rdls->pathRLbase);
        RwriteStr(Rfhandle, Rdls->pathR);
        RwriteStr(Rfhandle, Rdls->pathL);
        RwriteStr(Rfhandle, Rdls->fullfilename);
        RwriteStr(Rfhandle, Rdls->filename);
        RwriteUnsignedInt(Rfhandle, Rdls->sizeR);
        RwriteStr(Rfhandle, Rdls->sizeDisplayR);
        RwriteStr(Rfhandle, Rdls->sizeDisplayDownR);
        RwriteStr(Rfhandle, Rdls->percent);
        RwriteChar(Rfhandle, Rdls->active);
        RwriteChar(Rfhandle, Rdls->upload);
        RwriteTime_t(Rfhandle, Rdls->delayRetry);
        RwriteTime_t(Rfhandle, Rdls->nextRetry);

        Rdls = Rdls->next;
      }
      close(Rfhandle);
    }
  }
  Rfree(Rdir);
  RDBG("smbc: RopenSaveDownloadList finished\n");
}

int 
RsetManualyLocation(int Rsel,char *group, char **host, char **dir, char *user, char *pass)
{
  WINDOW *RwinMsg = NULL;
  int Rys, Rxs;
  int Rkey, Rii;
  char *Rstrt = NULL;
  int Rrol = 3;
  int Rxb = 0;
  char Rtgroup[RMAXGHLEN], Rthost[RMAXGHLEN], Rtdir[RMAXDIRLEN], Rtuser[RMAXGHLEN], Rtpass[RMAXGHLEN];
  char Rttgroup[RMAXGHLEN], Rtthost[RMAXGHLEN], Rttdir[RMAXDIRLEN], Rttuser[RMAXGHLEN], Rttpass[RMAXGHLEN];

  int Rdx = 0;
  int Rctrlp, Rin1, Rin2, Rfirst;
  int RmaxX;

  alarm(0);
  void Rrefresh(int Rmax, int Rsel)
  {
    int Ri;
    int Rall = Rstrlen(Rm136)+Rstrlen(Rm137);

    Rstrcpy(Rttgroup, RMAXGHLEN, Rtgroup, RMAXGHLEN); 
    if (Rstrlen(Rttgroup) > Rmax) 
      Rttgroup[Rmax] = 0;

    Rstrcpy(Rtthost, RMAXGHLEN, Rthost, RMAXGHLEN); 
    if (Rstrlen(Rtthost) > Rmax) 
      Rtthost[Rmax] = 0;

    Rstrcpy(Rttdir, RMAXDIRLEN, Rtdir, RMAXDIRLEN); 
    if (Rstrlen(Rttdir) > Rmax) 
      Rttdir[Rmax] = 0;

    Rstrcpy(Rttuser, RMAXGHLEN, Rtuser, RMAXGHLEN); 
    if (Rstrlen(Rttuser) > Rmax) 
      Rttuser[Rmax] = 0;

    for (Ri = 0; Ri<Rstrlen(Rtpass); Ri++) 
      Rttpass[Ri] = '*'; 
    Rttpass[Ri] = 0;

    if (Rstrlen(Rttpass) > Rmax) 
      Rttpass[Rmax] = 0;

    wattron(RwinMsg, COLOR_PAIR(2)); 
    mvwhline(RwinMsg, 3, Rdx+1, 32, Rxs-Rdx-3); 
      Rmvwprintw(RwinMsg, 3, Rdx+1, Rttgroup);
    mvwhline(RwinMsg, 4, Rdx+1, 32, Rxs-Rdx-3); 
      Rmvwprintw(RwinMsg, 4, Rdx+1, Rtthost);
    mvwhline(RwinMsg, 5, Rdx+1, 32, Rxs-Rdx-3); 
      Rmvwprintw(RwinMsg, 5, Rdx+1, Rttdir);
    mvwhline(RwinMsg, 6, Rdx+1, 32, Rxs-Rdx-3); 
      Rmvwprintw(RwinMsg, 6, Rdx+1, Rttuser);
    mvwhline(RwinMsg, 7, Rdx+1, 32, Rxs-Rdx-3); 
      Rmvwprintw(RwinMsg, 7, Rdx+1, Rttpass);
    wattroff(RwinMsg, COLOR_PAIR(2)); 
    switch (Rsel) 
    {
       case 1 : Rii = (Rxs-Rall)/2; 
                  Rmvwprintw(RwinMsg, Rys-2, Rii, Rm136);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm136)+1; 
                  wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Rys-2, Rii, Rm137); 
                  wattroff(RwinMsg, COLOR_PAIR(1)); 
                break;
       case 2 : Rii = (Rxs-Rall)/2; 
                  wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Rys-2, Rii, Rm136); 
                  wattroff(RwinMsg, COLOR_PAIR(1)); 
                Rii = (Rxs-Rall)/2+Rstrlen(Rm136)+1; 
                  Rmvwprintw(RwinMsg, Rys-2, Rii, Rm137);
                break;
       case 3 : Rii = (Rxs-Rall)/2; 
                  Rmvwprintw(RwinMsg, Rys-2, Rii, Rm136);
                Rii = (Rxs-Rall)/2+Rstrlen(Rm136)+1; 
                  Rmvwprintw(RwinMsg, Rys-2, Rii, Rm137);
                break;
    } 
  }

  if (Rdx < Rstrlen(Rm131)+1) 
    Rdx = Rstrlen(Rm131)+2;

  if (Rdx < Rstrlen(Rm168)+1) 
    Rdx = Rstrlen(Rm168)+2;

  if (Rdx < Rstrlen(Rm133)+1) 
    Rdx = Rstrlen(Rm133)+2;

  if (Rdx < Rstrlen(Rm134)+1) 
    Rdx = Rstrlen(Rm134)+2;

  if (Rdx < Rstrlen(Rm135)+1) 
    Rdx = Rstrlen(Rm135)+2;

  Rstrcpy(Rtgroup, RMAXGHLEN, group, -1); 
  Rstrcpy(Rthost, RMAXGHLEN, *host, -1); 

  if (Rstrlen(*dir) > 6) 
    memcpy(Rtdir, dir+Rstrlen(Rthost)+6, Rstrlen(*dir)-Rstrlen(Rthost)-5); 
  else 
    Rtdir[0] = 0;

  Rstrcpy(Rtuser, RMAXGHLEN, user, -1); 
  Rstrcpy(Rtpass, RMAXGHLEN, pass, -1); 

  Rys = 12; Rxs = 60;
  Rxb = Rxs/2+Rdx-3;
  RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
  box(RwinMsg, 0, 0);

  Rii = Rxs/2-Rstrlen(Rm138)/2; 
  Rmvwprintw(RwinMsg, 1, Rii, Rm138); 

  Rii = Rdx-Rstrlen(Rm131); 
    Rmvwprintw(RwinMsg, 3, Rii, Rm131);
  Rii = Rdx-Rstrlen(Rm168); 
    Rmvwprintw(RwinMsg, 4, Rii, Rm168);
  Rii = Rdx-Rstrlen(Rm133); 
    Rmvwprintw(RwinMsg, 5, Rii, Rm133);
  Rii = Rdx-Rstrlen(Rm134); 
    Rmvwprintw(RwinMsg, 6, Rii, Rm134);
  Rii = Rdx-Rstrlen(Rm135); 
    Rmvwprintw(RwinMsg, 7, Rii, Rm135);

  Rii = 0; 
  Rrefresh(Rxb, Rrol);

  RmaxX = Rxs-Rdx-3;
  wrefresh(RwinMsg);
  nodelay(RwinMsg, 1);
  RactiveWindow = RwinMsg;
  RDBG("smbc : RsetManualyLocation RactiveWindow = RwinMsg  2\n");
  while (Rrol) {
    RtryDownloadUpload();
    if (Rrol == 3) 
    {
      Rin1 = 0; Rin2 = 0; Rfirst = 1;
      while (readlineOneCharNoDelay(RwinMsg, 3, Rdx+1, Rtgroup, RMAXGHLEN, RmaxX, 3, &Rctrlp, 1, &Rin1, &Rin2, &Rfirst) <= 0) 
        RtryDownloadUpload();
      Rrefresh(RmaxX, Rrol);
      Rin1 = 0; Rin2 = 0; Rfirst = 1;
      while (readlineOneCharNoDelay(RwinMsg, 4, Rdx+1, Rthost, RMAXGHLEN, RmaxX, 3, &Rctrlp, 1, &Rin1, &Rin2, &Rfirst) <= 0) 
        RtryDownloadUpload();
      Rrefresh(RmaxX, Rrol);
      Rin1 = 0; Rin2 = 0; Rfirst = 1;
      while (readlineOneCharNoDelay(RwinMsg, 5, Rdx+1, Rtdir, RMAXDIRLEN, RmaxX, 3, &Rctrlp, 1, &Rin1, &Rin2, &Rfirst) <= 0) 
        RtryDownloadUpload();
      Rrefresh(RmaxX, Rrol);
      Rin1 = 0; Rin2 = 0; Rfirst = 1;
      while (readlineOneCharNoDelay(RwinMsg, 6, Rdx+1, Rtuser, RMAXGHLEN, RmaxX, 3, &Rctrlp, 1, &Rin1, &Rin2, &Rfirst) <= 0) 
        RtryDownloadUpload();
      Rrefresh(RmaxX, Rrol);
      Rin1 = 0; Rin2 = 0; 
      Rfirst = 3; // first and password mode
      while (readlineOneCharNoDelay(RwinMsg, 7, Rdx+1, Rtpass, RMAXGHLEN, RmaxX, 3, &Rctrlp, 1, &Rin1, &Rin2, &Rfirst) <= 0) 
        RtryDownloadUpload();

      Rrefresh(RmaxX, --Rrol);
    } 
    else 
    {
      Rkey = mvwgetch(RwinMsg, 1,1);
      if (Rkey == 9) 
      {
        if ((--Rrol) == 0) 
          Rrol = 3;
        Rrefresh(RmaxX, Rrol);
      }
      if (Rkey == 10) 
      {
        if (Rrol == 2) 
        {
          Rfree(*host);
          RsetStrVariable(V_GROUP, Rtgroup);
          RstrdupN(*host, Rthost);

            if (Rtdir[0] == '/') 
            {
              RstrdupN(Rstrt, "smb://");
              Rstraddtoend(&Rstrt, Rahost);
              Rstraddtoend(&Rstrt, Rtdir);
            }
            else 
            {
              RstrdupN(Rstrt, "smb://");
              Rstraddtoend(&Rstrt, Rahost);
              Rstraddtoend(&Rstrt, "/");
              Rstraddtoend(&Rstrt, Rtdir);
            }
            Rfree(*dir);
            RstrdupN(*dir, Rstrt);
            if ((*dir+Rstrlen(*dir)-1)[0] != '/') 
              Rstraddtoend(dir, "/");
            Rfree(Rstrt);
          Rstrcpy(user, -1, Rtuser, RMAXGHLEN);
          Rstrcpy(pass, -1, Rtpass, RMAXGHLEN);
          Rdelwin(RwinMsg);
          RwinRedrawWithData();
          alarm(RCalarm);
          return(1);
        }
        else 
          Rrol = 0;
      }
      if (Rkey == 'q') 
        Rrol = 0;
    }
  } 
  Rdelwin(RwinMsg); 
  RwinRedrawWithData();
  alarm(RCalarm);
  return(0);
}

int 
RcreateDir(PANEL_TYPE RpanelSelect, char *Rpath)
{
  int Rys, Rxs;
  WINDOW *RwinMsg = NULL;
  const int MAXSTR = 100;
  char RdirT[MAXSTR];
  int Rrol = 3;
  char *Rstr = NULL;
  char Rkey;
  int Rin1 = 0;
  int Rin2 = 0;
  int Rfirst = 1;
  int Rctrlp;

  void 
  Rrefresh(int Rsel)
  {
    int Rall = Rstrlen(Rm136)+Rstrlen(Rm137);
    char *RdirTt = NULL;

    wattron(RwinMsg, COLOR_PAIR(2));
    mvwhline(RwinMsg, 3, 3, ' ', Rxs-6); 

    RdirTt = strdup(RdirT);
    if (Rstrlen(RdirTt) > Rxs-6) 
      RdirTt[Rxs-6] = 0;
    Rmvwprintw(RwinMsg, 3, 3, RdirTt);

    Rfree(RdirTt);
    wattroff(RwinMsg, COLOR_PAIR(2));
    switch(Rsel) {
      case 1 : Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2, Rm136);
               wattron(RwinMsg, COLOR_PAIR(1)); 
               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rm136)+1, Rm137); 
               wattroff(RwinMsg, COLOR_PAIR(1)); 
               break;
      case 2 : wattron(RwinMsg, COLOR_PAIR(1)); 
               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2, Rm136); 
               wattroff(RwinMsg, COLOR_PAIR(1)); 
               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rm136)+1, Rm137);
               break;
      case 3 : Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2, Rm136);
               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rm136)+1, Rm137);
               break;
    }
  }

  RdirT[0] = 0;
  Rys = 8; Rxs = 50;
  RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
  box(RwinMsg, 0, 0);

  RstrdupN(Rstr, Rm139); 
  Rmvwprintw(RwinMsg, 1, Rxs/2-Rstrlen(Rstr)/2, Rstr); 
  Rfree(Rstr);

  Rrefresh(Rrol);
  nodelay(RwinMsg, 1);
  RactiveWindow = RwinMsg;
  RDBG("smbc : RcreateDir RactiveWindow = RwinMsg  2\n");
  while (Rrol) 
  {
    RtryDownloadUpload();
    if (Rrol == 3) 
    {
      Rfirst = 1;
      Rin1 = 0;
      Rin2 = 0;
      while (readlineOneCharNoDelay(RwinMsg, 3, 3, RdirT, MAXSTR, Rxs-6, 3, &Rctrlp, 1, &Rin1, &Rin2, &Rfirst) <= 0) 
        RtryDownloadUpload();
      Rrefresh(--Rrol);
    }
    Rkey = mvwgetch(RwinMsg,1,1);
    if (Rkey == 9) 
    {
      if ((--Rrol) == 0) Rrol = 3;
      Rrefresh(Rrol);
    }
    if (Rkey == 10) 
    {
      if (Rrol == 2) 
      {
        if (RpanelSelect == PANEL_RIGHT) 
        {
          Rstr = RreturnFullSmbPath(FULLPATH_WITH_SMB, "", Rahost, Rpath, RdirT);
          Rsmbc_mkdir(Rstr, 0755);
        }
        else 
        {
          RstrdupN(Rstr, RadirL);
          Rstraddtoend(&Rstr, RdirT);
          mkdir(Rstr, 0755);
        }
        Rfree(Rstr);
        Rdelwin(RwinMsg);
        RwinRedrawWithData();
        return(0);
      } 
      else 
        Rrol = 0;
    }
    if (Rkey == 'q') 
      Rrol = 0;
  }
  Rdelwin(RwinMsg);
  RwinRedrawWithData();
  return(0);
}

//
// ------------------- RmanageFindFile START ------------------------
//
int 
RmanageFindFile(char *RfindStr, char *group, char **host, char **dir, char **SelectFile, TTfind *RidxNet, int Rcount)
{
  WINDOW *RwinList = NULL;
  WINDOW *RwinTop = NULL;
  WINDOW *RwinWorkgroup = NULL;
  WINDOW *RwinHost = NULL;
  WINDOW *RwinDir = NULL;
  WINDOW *RwinFilename = NULL;
  WINDOW *RwinStatus = NULL;

#define RTDIRLEN 2000

  int Rkey, Rsel, RmaxY, Rx6;
  int RxWorkgroup, RxHostname, RxDir, RxFilename;
  int RxFilenameLen = 0;
  int RlFirst = 1;
  int RlSelect = 1;
  TTfind *RfindIndex;
  char *Rmsg1 = NULL;
  int RtxtStartX = 10;
  char Rttgroup[RMAXGHLEN], Rtthost[RMAXGHLEN], Rttdir[RTDIRLEN], Rttfilename[RMAXGHLEN];
  char Rtgroup[RMAXGHLEN], Rthost[RMAXGHLEN], Rtdir[RTDIRLEN], Rtfilename[RMAXGHLEN];
  int Rrol = 1;
  int FindListCount;
  int ret = 0;
  int Rt = 0;
  int Rctrlp = 1;
  int RctrlpPrim = Rctrlp;
  TTindexTable *Ridx = NULL;
  int Rin11  = 0;
  int Rin12  = 0;
  int Rin21  = 0;
  int Rin22  = 0;
  int Rin31  = 0;
  int Rin32  = 0;
  int Rin41  = 0;
  int Rin42  = 0;
  int Rfirst = 1;
  char *RstrT = NULL;
  int Rtl, Rtl2;
  int RedrawWithStatusBar = WITH_STATUS_BAR;

  void 
  RpaintTop(int RnoDraw)
  {
    int RmaxT = RmainWinX-RtxtStartX-4;

    wattron(RwinTop, COLOR_PAIR(2));
    if (RnoDraw != 1) 
      mvwhline(RwinTop, 0, RtxtStartX+1, 32, RmaxT);
    if (RnoDraw != 2) 
      mvwhline(RwinTop, 1, RtxtStartX+1, 32, RmaxT);
    if (RnoDraw != 3) 
      mvwhline(RwinTop, 2, RtxtStartX+1, 32, RmaxT);
    if (RnoDraw != 4) 
      mvwhline(RwinTop, 3, RtxtStartX+1, 32, RmaxT);

    RstrT = to_scr(Rtfilename); 
    Rstrcpy(Rttfilename, RMAXGHLEN, RstrT, -1); 
    if (Rstrlen(Rttfilename) > RmaxT)
      Rttfilename[RmaxT] = 0;

    RstrT = to_scr(Rtdir); 
    Rstrcpy(Rttdir, RTDIRLEN, RstrT, -1); 
    if (Rstrlen(Rttdir) > RmaxT) 
      Rttdir[RmaxT] = 0;

    RstrT = to_scr(Rthost); 
    Rstrcpy(Rtthost, RMAXGHLEN, RstrT, -1); 
    if (Rstrlen(Rtthost) > RmaxT) 
      Rtthost[RmaxT] = 0;

    RstrT = to_scr(Rtgroup); 
    Rstrcpy(Rttgroup, RMAXGHLEN, RstrT, -1); 
    if (Rstrlen(Rttgroup) > RmaxT) 
      Rttgroup[RmaxT] = 0;

    if (RnoDraw != 1) 
      Rmvwprintw(RwinTop, 0, RtxtStartX+1, Rttfilename);
    if (RnoDraw != 2) 
      Rmvwprintw(RwinTop, 1, RtxtStartX+1, Rttdir);
    if (RnoDraw != 3) 
      Rmvwprintw(RwinTop, 2, RtxtStartX+1, Rtthost);
    if (RnoDraw != 4) 
      Rmvwprintw(RwinTop, 3, RtxtStartX+1, Rttgroup);
    wattroff(RwinTop, COLOR_PAIR(2));
    wrefresh(RwinTop);
  }

  void 
  RpaintList(int Rfirst, int Rselect)    // Rfirst and Rselect - position in IDX not on to index file
  {
    int Rlast, Ri, Rii;
    int Rk = 0;
    TTfind *RfindIndexT = NULL;
    char *Rstr = NULL;
    int Ritmp;
    int RfilePos;
    TTfind *RidxWork = RidxNet;
    TTindexTable *RidxSmallWork = Ridx;

    RDBG("smbc: RpaintList start\n");
    Ritmp = RgetCountRecordIndexTable(RidxSmallWork);
    RDBG("smbc: RpaintList Ritmp = "); RDBGi(Ritmp); RDBG("\n");
    if ((Rfirst == 1) & (Ritmp <= RmainWinY-13)) 
      Rlast = Ritmp;
    else 
      Rlast = Rfirst + RmainWinY-13;

    RDBG("smbc: RpaintList Rfirst = "); RDBGi(Rfirst);
    RDBG(" Rlast = "); RDBGi(Rlast); RDBG("\n");
    if (RgotoNIndextTable(&RidxSmallWork,Rfirst)) 
    {
      RindexNetSetSeek(&RidxWork, RidxSmallWork->number);
      RfilePos = RidxSmallWork->number;

      if (RedrawWithStatusBar == WITH_STATUS_BAR)
      {
        mvwhline(RwinStatus, 0, 0, 32, RmainWinX-2);
        wrefresh(RwinStatus);
      }

      RDBG("smbc: RpaintList Rfirst: "); RDBGi(Rfirst);
      RDBG(" Rlast: "); RDBGi(Rlast); 
      RDBG(" Rselected: "); RDBGi(Rselected); 
      RDBG("\n");
      for (Ri = Rfirst; Ri <= Rlast;) 
      {
        RindexNetReadOneRecord(&RidxWork, &RfindIndexT); RfilePos++;
        RDBG("smbc: RpaintList RfilePos = "); RDBGi(RfilePos);
        RDBG(" Ridx->number = "); RDBGi(RidxSmallWork->number); RDBG("\n");
        for (Rii = RfilePos; Rii <= RidxSmallWork->number; Rii++) 
        {
          RindexFreeOneTTfind(&RfindIndexT);
          RindexNetReadOneRecord(&RidxWork, &RfindIndexT);
          RfilePos++;
        }
        RgotoNextIndexTable(&RidxSmallWork);

        mvwhline(RwinWorkgroup, Rk, 0, 32, RxHostname-1);
        mvwhline(RwinHost, Rk, 0, 32, RxDir-RxHostname-1);
        mvwhline(RwinDir, Rk, 0, 32, RxFilename-RxDir-1);
        mvwhline(RwinFilename, Rk, 0, 32, RmainWinX-RxFilename-2);

        if (RedrawWithStatusBar == WITH_STATUS_BAR)
        {
          if ((Ri == Rselect)) 
          {
            RstrdupN(Rstr, "//");
            Rstraddtoend(&Rstr, RfindIndexT->host);
            Rstraddtoend(&Rstr, "/");
            Rstraddtoend(&Rstr, RfindIndexT->dir);
            Rstraddtoend(&Rstr, "/");
            Rstraddtoend(&Rstr, RfindIndexT->file);
            if (Rstrlen(Rstr) > RmainWinX-2) 
              Rstr[RmainWinX-2] = 0;
            if (Rstrlen(Rstr) > 4) 
              Rmvwprintw(RwinStatus, 0, 0, to_scr(Rstr));
            wrefresh(RwinStatus);
            Rfree(Rstr);
          }
        }
  
        if ((Ri == Rselect) && (Rrol == 2)) 
        {
          wattron(RwinWorkgroup, COLOR_PAIR(1));
          wattron(RwinHost, COLOR_PAIR(1));
          wattron(RwinDir, COLOR_PAIR(1));
          wattron(RwinFilename, COLOR_PAIR(1));
        
          mvwhline(RwinWorkgroup, Rk, 0, 32, RxHostname-1);
          mvwhline(RwinHost, Rk, 0, 32, RxDir-RxHostname-1);
          mvwhline(RwinDir, Rk, 0, 32, RxFilename-RxDir-1);
          mvwhline(RwinFilename, Rk, 0, 32, RmainWinX-RxFilename-2);
        }
 
        RstrdupN(Rstr, "");
        Rstraddtoend(&Rstr, RfindIndexT->access_denied);
        Rstraddtoend(&Rstr, RfindIndexT->group);
        if (Rstrlen(Rstr) > RxHostname-1) 
          RtruncateStr(&Rstr, RxHostname-2);
        Rmvwprintw(RwinWorkgroup, Rk, 0, to_scr(Rstr));
        Rfree(Rstr);

        if (Rstrlen(RfindIndexT->host) > RxDir-RxHostname-1) 
          RtruncateStr(&(RfindIndexT->host), RxDir-RxHostname-2);
        if (Rstrlen(RfindIndexT->dir) > RxFilename-RxDir-1) 
          RtruncateStr(&(RfindIndexT->dir), RxFilename-RxDir-2);
        if (Rstrlen(RfindIndexT->file) > RmainWinX-RxFilename-2) 
          RtruncateStr(&(RfindIndexT->file), RmainWinX-RxFilename-3);

        Rmvwprintw(RwinHost, Rk, 0, to_scr(RfindIndexT->host));
        Rmvwprintw(RwinDir, Rk, 0, to_scr(RfindIndexT->dir));
        Rmvwprintw(RwinFilename, Rk, 0, to_scr(RfindIndexT->file));

        if ((Ri == Rselect) && (Rrol == 2)) 
        {
          wattroff(RwinWorkgroup, COLOR_PAIR(1));
          wattroff(RwinHost, COLOR_PAIR(1));
          wattroff(RwinDir, COLOR_PAIR(1));
          wattroff(RwinFilename, COLOR_PAIR(1));
        }
        Ri++;
        Rk++;
        RindexFreeOneTTfind(&RfindIndexT);
      }
    }
    for (Ri = Rk; Ri < RmainWinY-11; Ri++) 
    {
      mvwhline(RwinWorkgroup, Ri, 0, 32, RxHostname-1);
      mvwhline(RwinHost, Ri, 0, 32, RxDir-RxHostname-1);
      mvwhline(RwinDir, Ri, 0, 32, RxFilename-RxDir-1);
      mvwhline(RwinFilename, Ri, 0, 32, RmainWinX-RxFilename-2);
    }
    wrefresh(RwinWorkgroup);
    wrefresh(RwinHost);
    wrefresh(RwinDir);
    wrefresh(RwinFilename);
    RDBG("\nsmbc: RpaintList finished\n");
  }

  void 
  RfindFileFromIndex()
  {
    int RidxNr = 1;
    TTfind *RidxNetWork = RidxNet;

    char Rtttgroup[RMAXGHLEN], Rttthost[RMAXGHLEN], Rtttdir[2000], Rtttfilename[RMAXGHLEN];
    char Rttttgroup[RMAXGHLEN], Rtttthost[RMAXGHLEN], Rttttdir[2000], Rttttfilename[RMAXGHLEN];
    int Rfind;

    RDBG("smbc: RfindFileFromIndex start\n");
    RlFirst = 1;
    RlSelect = 1;
    RstrToUpperCopy(Rttttgroup, Rtgroup);
    RstrToUpperCopy(Rtttthost, Rthost);
    RstrToUpperCopy(Rttttdir, Rtdir);
    RstrToUpperCopy(Rttttfilename, Rtfilename);

    RindexNetSetSeek(&RidxNetWork, 1);
    RfreeAllRecordFromIndexTable(&Ridx);
    while (RindexNetReadOneRecord(&RidxNetWork, &RfindIndex)) 
    {
      RstrToUpperCopy(Rtttgroup, RfindIndex->group); 
      RstrToUpperCopy(Rttthost, RfindIndex->host); 
      RstrToUpperCopy(Rtttdir, RfindIndex->dir); 
      RstrToUpperCopy(Rtttfilename, RfindIndex->file); 

      Rfind = 1;

      if (Rstrlen(Rttttgroup))    
        if (!strstr(Rtttgroup, Rttttgroup)) 
          Rfind = 0;

      if (Rstrlen(Rtttthost)) 
        if (!strstr(Rttthost, Rtttthost)) 
          Rfind = 0;

      if (Rstrlen(Rttttdir)) 
        if (!strstr(Rtttdir, Rttttdir)) 
          Rfind = 0;

      if (Rstrlen(Rttttfilename)) 
        if (!strstr(Rtttfilename, Rttttfilename)) 
          Rfind = 0; 
     
      if (Rfind) 
        RcreateNewRecordInIndexTable(&Ridx, RidxNr);
      RidxNr++;
      RindexFreeOneTTfind(&RfindIndex);
    }
    RDBG("smbc: RfindFileFromIndex finished\n");
  }

  void 
  RrefreshR(int RnoDraw)
  {
     RpaintTop(RnoDraw);
     if ((Rt != 2) && (Rctrlp))
       RfindFileFromIndex();
     RpaintList(RlFirst, RlSelect);
  }

  void 
  RcreateFindFileWindow()
  {
    TTfind *RidxNetWork = RidxNet;

    Rdelwin(RwinList);
    Rdelwin(RwinTop);
    Rdelwin(RwinWorkgroup);
    Rdelwin(RwinHost);
    Rdelwin(RwinDir);
    Rdelwin(RwinFilename);
    Rdelwin(RwinStatus);
    
    RwinList = newwin(RmainWinY-2, RmainWinX, 1, 0);
    RwinTop = newwin(4, RmainWinX-2, 2, 1);
    getmaxyx(RwinMain, RmaxY, Rsel);
    if (Rx6 > Rsel-18) Rx6 = Rsel-10;
    
    box(RwinList,0,0);
    RxWorkgroup = 1;
    RxHostname = 15;
    RxDir = 30;
    RindexNetSetSeek(&RidxNetWork, 1);
    while (RindexNetReadOneRecord(&RidxNetWork, &RfindIndex)) 
    {
      if ((RxFilenameLen < Rstrlen(RfindIndex->file)) & (RxFilenameLen < 20))
        RxFilenameLen = Rstrlen(RfindIndex->file);
      RindexFreeOneTTfind(&RfindIndex);
    }
    RxFilename = RmainWinX-2-RxFilenameLen;

    RwinWorkgroup = newwin(RmainWinY-13, RxHostname-1 , 9 , 1);
    RwinHost = newwin(RmainWinY-13, RxDir-RxHostname-1, 9, RxHostname+1);
    RwinDir = newwin(RmainWinY-13, RxFilename-RxDir-1, 9, RxDir+1);
    RwinFilename = newwin(RmainWinY-13, RmainWinX-RxFilename-2, 9, RxFilename+1);
    RwinStatus = newwin(1, RmainWinX-2, RmainWinY-3, 1);

    Rmvwprintw(RwinList, 6, RxWorkgroup, Rm140);
    Rmvwprintw(RwinList, 6, RxHostname+1, Rm141);
    Rmvwprintw(RwinList, 6, RxDir+1, Rm142);
    Rmvwprintw(RwinList, 6, RxFilename+1, Rm143);
    mvwvline(RwinList, 6, RxHostname, 0, RmainWinY-11);
    mvwvline(RwinList, 6, RxDir, 0, RmainWinY-11);
    mvwvline(RwinList, 6, RxFilename, 0, RmainWinY-11);
    mvwhline(RwinList, 5, 1, 0, RmainWinX-2);
    mvwhline(RwinList, 7, 1, 0, RmainWinX-2);
    mvwhline(RwinList, RmainWinY-5, 1, 0, RmainWinX-2);
    wrefresh(RwinList);
    Rmvwprintw(RwinTop, 0, RtxtStartX-Rstrlen(Rm147), Rm147);
    Rmvwprintw(RwinTop, 1, RtxtStartX-Rstrlen(Rm146), Rm146);
    Rmvwprintw(RwinTop, 2, RtxtStartX-Rstrlen(Rm145), Rm145);
    Rmvwprintw(RwinTop, 3, RtxtStartX-Rstrlen(Rm144), Rm144);
    wrefresh(RwinTop);
  
    keypad(RwinList, TRUE);
    nodelay(RwinList, 1);
    RactiveWindow = RwinList;
    RDBG("smbc : RmanageFindFile RactiveWindow = RwinList  2\n");
  }

  void 
  RresizeFindFile(int RnoDraw)
  {
    if (RdoResizeFind) 
    {
      RdoResizeFind = 0;
      RdoWindowResize();
      RcreateFindFileWindow();
      RrefreshR(RnoDraw);
    }
  }

  int 
  RdetectAndPaintStatusCtrlp()
  {
    if (Rctrlp == 2) 
    {
      Rdelwin(RwinList);
      Rdelwin(RwinTop);
      Rdelwin(RwinWorkgroup);
      Rdelwin(RwinHost);
      Rdelwin(RwinDir);
      Rdelwin(RwinFilename);
      Rdelwin(RwinStatus);
      return(1);
    }
    if (RctrlpPrim != Rctrlp) 
    {
      RAstatusBar(STATUS_INTERACTIVE_FILTERING_IS_OFF+Rctrlp);
      RctrlpPrim = Rctrlp;
    }
    return(0);
  }

  void 
  RfreeRmanageFindFile()
  {
    Rdelwin(RwinList);
    Rdelwin(RwinTop);
    Rdelwin(RwinWorkgroup);
    Rdelwin(RwinHost);
    Rdelwin(RwinDir);
    Rdelwin(RwinFilename);
    Rdelwin(RwinStatus);
  }

  RDBG("smbc: RmanageFindFile start\n");
  Rttgroup[0] = 0;
  Rtthost[0] = 0;
  Rttdir[0] = 0;
  Rstrcpy(Rttfilename, RMAXGHLEN, RfindStr, -1);
  Rtgroup[0] = 0;
  Rthost[0] = 0;
  Rtdir[0] = 0;
  Rstrcpy(Rtfilename, RMAXGHLEN, RfindStr, -1);

  RcreateFindFileWindow();
  RlFirst = 1; RlSelect = 1;
  nodelay(RwinTop, 1);
  RactiveWindow = RwinTop;
  RDBG("smbc : RmanageFindFile RactiveWindow = RwinTop  2\n");
  while (Rrol) {
    // files
    RedrawWithStatusBar = WITHOUT_STATUS_BAR;
    RrefreshR(0);
    RAstatusBar(STATUS_ENTER_FILE_NAME);
    Rt = 0;
    Rtl = 0;
    while (Rt <= 0) 
    {
      Rfirst = 1;
      while ((Rt = readlineOneCharNoDelay(RwinTop, 0, RtxtStartX+1, Rtfilename, RMAXGHLEN, 
                                          RmainWinX-RtxtStartX-4, 3, &Rctrlp, 1, &Rin41, &Rin42, 
                                          &Rfirst)) <= 0) 
      {
        Rfirst = 1;
        RtryDownloadUpload();
        RresizeFindFile(0);
        RdetectAndPaintStatusCtrlp();
        if (Rt >= 0)
          RfindFileFromIndex();
          Rtl2 = Rstrlen(Rtfilename);
          if (Rtl != Rtl2) 
          {
            Rtl = Rtl2;
            RrefreshR(4);
            RresizeFindFile(4);
          }
      }
      if (RdetectAndPaintStatusCtrlp()) 
      {
        RfreeRmanageFindFile();
        return(ret);
      }
      RrefreshR(0);
      RresizeFindFile(0);
    }
    // dirs
    RedrawWithStatusBar = WITHOUT_STATUS_BAR;
    RrefreshR(0);
    RresizeFindFile(0);
    RfindFileFromIndex();
    RAstatusBar(STATUS_ENTER_DIR_NAME);
    RresizeFindFile(0);
    RrefreshR(0);
    Rt = 0;
    Rtl = 0;
    while (Rt <= 0) 
    {
      Rfirst = 1;
      while ((Rt = readlineOneCharNoDelay(RwinTop, 1, RtxtStartX+1, Rtdir, RMAXGHLEN, 
                                          RmainWinX-RtxtStartX-4, 3, &Rctrlp, 1, &Rin31, &Rin32, 
                                          &Rfirst)) <= 0) 
      {
        Rfirst = 1;
        RtryDownloadUpload();
        RresizeFindFile(0);
        RdetectAndPaintStatusCtrlp();
        Rtl2 = Rstrlen(Rtdir);
        if (Rtl != Rtl2) 
        {
          Rtl = Rtl2;
          RrefreshR(3);
          RresizeFindFile(3);
        }
      }
      if (RdetectAndPaintStatusCtrlp()) 
      {
        RfreeRmanageFindFile();
        return(ret);
      }
      RrefreshR(0);
      RresizeFindFile(0);
    }
    // hosts
    RedrawWithStatusBar = WITHOUT_STATUS_BAR;
    RfindFileFromIndex();
    RAstatusBar(STATUS_ENTER_HOST_NAME);
    RresizeFindFile(0);
    RrefreshR(0);
    Rt = 0;
    while (Rt <= 0) 
    {
      Rfirst = 1;
      Rtl = 0;
      while ((Rt = readlineOneCharNoDelay(RwinTop, 2, RtxtStartX+1, Rthost, RMAXGHLEN, 
                                          RmainWinX-RtxtStartX-4, 3, &Rctrlp, 1, &Rin21, &Rin22, 
                                          &Rfirst)) <= 0) 
      {
        Rfirst = 1;
        RtryDownloadUpload();
        RresizeFindFile(0);
        Rtl2 = Rstrlen(Rthost);
        RdetectAndPaintStatusCtrlp();
        if (Rtl != Rtl2) 
        {
          Rtl = Rtl2;
          RrefreshR(2);
          RresizeFindFile(2);
        }
      }
      if (RdetectAndPaintStatusCtrlp()) 
      {
        RfreeRmanageFindFile();
        return(ret);
      }
      RrefreshR(0);
      RresizeFindFile(0);
    }
    // workgroups
    RedrawWithStatusBar = WITHOUT_STATUS_BAR;
    RfindFileFromIndex();
    RAstatusBar(STATUS_ENTER_WORKGROUP_NAME);
    RresizeFindFile(0);
    RrefreshR(0);
    Rt = 0;
    Rtl = 0;
    while (Rt <= 0) 
    {
      Rfirst = 1;
      RDBG("smbc : RmanageFindFile 2\n");
      Rtl = 0;
      while ((Rt = readlineOneCharNoDelay(RwinTop, 3, RtxtStartX+1, Rtgroup, RMAXGHLEN, 
                                          RmainWinX-RtxtStartX-4, 3, &Rctrlp, 1, &Rin11, &Rin12, 
                                          &Rfirst)) <= 0) 
      {
        Rfirst = 1;
        RtryDownloadUpload();
        RresizeFindFile(0);
        RdetectAndPaintStatusCtrlp();
        Rtl2 = Rstrlen(Rtgroup);
        if (Rtl != Rtl2) 
        {
          Rtl = Rtl2;
          RrefreshR(1);
          RresizeFindFile(1);
        }
      }
      if (RdetectAndPaintStatusCtrlp()) 
      {
        RfreeRmanageFindFile();
        return(ret);
      }
    }

    RedrawWithStatusBar = WITH_STATUS_BAR;
    RfindFileFromIndex();
    RAstatusBar(STATUS_DEFAULT);
    Rrol = 2;
    RresizeFindFile(0);
    RrefreshR(0);
    RlFirst = 1; RlSelect = 1;
    Rt = 2;
    FindListCount = RgetCountRecordIndexTable(Ridx);
    while ((Rrol == 2) && (FindListCount > 0)) {
      while ((Rkey = mvwgetch(RwinList, 1,1)) <= 0) 
        RresizeFindFile(0);
        RtryDownloadUpload();
        if ((Rkey == 9) || (Rkey == 'i'))    // tab
        {
          Rrol = 1;
          break;
        }
        if (Rkey == KEY_DOWN || Rkey == 'o') 
        {
          if ((RlSelect+1 == RlFirst+RmainWinY-13) && (RlFirst+RmainWinY-13 < FindListCount+1)) 
          {
            RlFirst++;
            RlSelect++;
          }
          else 
            if (RlSelect < FindListCount) 
              RlSelect++;
          RpaintList(RlFirst, RlSelect);
        }
        if (Rkey == KEY_UP || Rkey == 'p') 
        {
          if (RlSelect > 1) 
          {
            if ((RlFirst > 1) && (RlSelect == RlFirst)) 
            { 
              RlSelect--; 
              RlFirst--;
            }
            else 
              RlSelect--;
            RpaintList(RlFirst, RlSelect);
          }
        }
        if (Rkey == KEY_NPAGE || Rkey == 'k') 
        {
          if (RlFirst+RmainWinY-13 <= FindListCount) 
          {
            if (RlSelect+1 == RlFirst+RmainWinY-13) 
            {
              if (RlSelect+RmainWinY-13 < FindListCount) 
              {
                 RlFirst = RlSelect;
                 RlSelect = RlFirst+RmainWinY-13-1;
               }
               else 
               {
                 RlFirst = FindListCount-(RmainWinY-13);
                 RlSelect = FindListCount-1;
               }
           }
           else 
           {
              if (RlSelect+RmainWinY-13 <= FindListCount) 
                RlSelect = RlFirst+RmainWinY-13-1;
              else
                RlSelect = FindListCount-1;
           }
         }
         RpaintList(RlFirst, RlSelect);
        }
        if (Rkey == KEY_PPAGE || Rkey == 'l') 
        {
          if (RlSelect > 1) 
          {
            if (RlFirst == RlSelect) 
            {
              if (RlFirst-(RmainWinY-13) < 1) 
              {
                RlFirst = 1;
                RlSelect = 1;
              }
              else 
              {
                RlFirst = RlFirst-(RmainWinY-13)+1;
                RlSelect = RlFirst;
              }
            }
            else RlSelect = RlFirst;
            RpaintList(RlFirst, RlSelect);
          }
        }
        if (Rkey == 10)     // enter
        {
          RgotoNIndextTable(&Ridx, RlSelect);
          RindexNetSetSeek(&RidxNet, Ridx->number);
          RindexNetReadOneRecord(&RidxNet, &RfindIndex);

          Rstraddtoend(&Rmsg1, RfindIndex->host);
          Rstraddtoend(&Rmsg1, "/");
          Rstraddtoend(&Rmsg1, RfindIndex->dir);
          Rstraddtoend(&Rmsg1, "/");
          if (RsmbCheckFileExist(RfindIndex))
          {
            RsetStrVariable(V_GROUP, RfindIndex->group);
            RstrdupN(*host, RfindIndex->host);
            RstrdupN(*dir, RfindIndex->dir);
            RstrdupN(*SelectFile, RfindIndex->file);
            ret = 1;
            Rkey = 'q';
          }
          else
          {
            RindexNetSetSeek(&RidxNet, Ridx->number);
            RstrdupN(RidxNet->access_denied, "!");
            RshowMessage(INFO_I_CANT_ENTER_TO_X_ACCESS_DENIED, RfindIndex->file, NULL, NULL, DONT_REDRAW_MAIN_WINDOW);
            RpaintList(RlFirst, RlSelect);
          }
          Rfree(Rmsg1);
          RindexFreeOneTTfind(&RfindIndex);
        }
        if (Rkey == 'q') 
        { 
          RfreeRmanageFindFile();
          return(ret);
        }
    } 
  }
  return(ret);
}
//
// ------------------- RmanageFindFile END ------------------------
//

void 
RcreateIndexFileOnline(int RfileFd)
{
  int Rys = 10;
  int Rxs = 76;
  WINDOW *RwinMsg = NULL;
  WINDOW *RwinMsg2 = NULL;
  char *Rstr = NULL;
  char *RstrW = NULL;
  char *RstrShare = NULL;
  char *RstrDir = NULL;
  char *RstrDir2 = NULL;
  int Rii, Rk, Rkk;
  char *RdirT = NULL;
  
  RDBG("smbc: RcreateIndexFileOnline start\n");
  RwinMsg = newwin(Rys+1, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
  RwinMsg2 = newwin(5, Rxs-15, RmainWinY/2-Rys/2+2, RmainWinX/2-Rxs/2+13);
  box(RwinMsg,0,0);
  RstrdupN(Rstr, Rm148); Rii = Rxs/2-Rstrlen(Rstr)/2; Rmvwprintw(RwinMsg, 1, Rii, Rstr); Rfree(Rstr);
  Rmvwprintw(RwinMsg, 3, 1, Rm149);
  Rmvwprintw(RwinMsg, 4, 1, Rm150);
  Rmvwprintw(RwinMsg, 5, 1, Rm151);
  Rmvwprintw(RwinMsg, 6, 1, Rm31);

  wattron(RwinMsg, COLOR_PAIR(1));
     RstrdupN(Rstr, Rm137); Rii = Rxs/2-Rstrlen(Rstr)/2; Rmvwprintw(RwinMsg, 8, Rii, Rstr); Rfree(Rstr);
  wattroff(RwinMsg, COLOR_PAIR(1));

  wrefresh(RwinMsg);
  RstrdupN(Rstr, "smb//");
  Rstraddtoend(&Rstr, Rahost);
  Rstraddtoend(&Rstr, "/");
  Rkk = 1;
  if (Rstrlen(Radir) > Rstrlen(Rstr)) 
  {
    RstrW = Radir+Rstrlen(Rstr)+1;
    if (Rstrlen(RstrW) > 0) 
    {
      if (RstrW[0] == '/') 
        RstrW++;
      Rk = Rstrlen(RstrW) - Rstrlen(strstr(RstrW, "/"));
      RstrShare = Rmalloc(Rstrlen(RstrW)+1);
      memcpy(RstrShare, RstrW, Rk);
      RstrShare[Rk] = 0;
      
      RstrDir = strdup(RstrW+Rk);
      if (RstrDir[0] == '/') 
        RstrDir2 = RstrDir+1;
      Rkk = 0;
    }
  } 
  if (Rkk) {
      RstrShare = strdup("");
      RstrDir = strdup("");
      RstrDir2 = RstrDir;
  }
  if (RexistAnySelect(RlistRight)) 
  {
    RlistTmainRight = RlistRight;
    while (RlistTmainRight)
    {
      if (RlistTmainRight->select) 
      {
        if (RremoteLevel == LEVEL_WORKGROUPS)
          RcreateFindListIndex(0, RlistTmainRight->col4, "", "", RstrDir2, RwinMsg2, RfileFd);
        else
        if (RremoteLevel == LEVEL_HOSTS)
          RcreateFindListIndex(0, RgetStrVariable(V_GROUP), RlistTmainRight->col4, "", RstrDir2, RwinMsg2, RfileFd);
        else
        if (RremoteLevel == LEVEL_SHARES)
          RcreateFindListIndex(0, RgetStrVariable(V_GROUP), Rahost, RlistTmainRight->col4, RstrDir2, RwinMsg2, RfileFd);
        else
        if (RremoteLevel ==  LEVEL_FILES)
        {
          RdirT = strdup(RstrDir2);
          Rstraddtoend(&RdirT, RlistTmainRight->col2);
          Rcharaddtoend(&RdirT, '/');
          RcreateFindListIndex(0, RgetStrVariable(V_GROUP), Rahost, RstrShare, RdirT, RwinMsg2, RfileFd);
          Rfree(RdirT);
        }
      }
      RlistTmainRight = RlistTmainRight->next;
    }
    RlistTmainRight = RlistRight;
  }
  else
    RcreateFindListIndex(0, RgetStrVariable(V_GROUP), Rahost, RstrShare, RstrDir2, RwinMsg2, RfileFd);
  Rfree(Rstr);
  Rfree(RstrShare);
  Rfree(RstrDir);

  Rdelwin(RwinMsg2);
  Rdelwin(RwinMsg);
  RDBG("smbc: RcreateIndexFileOnline stop\n");
}

int 
RfindFileSmb(char *dir, char **SelectFile)
{
  int RmaxX;
  int Rys, Rxs;
  WINDOW *RwinMsg = NULL;
  const int MAXSTR = 100;
  char RdirT[MAXSTR];
  int Rrol = 4;
  char Rkey;
  int ret = 0;
  int RfileFd = 0;
  char *RfileName = NULL;
  int Rcount;
  TTfind *RidxNet = NULL;
  int RdirTpos = 0;
  int Rin1 = 0;
  int Rin2 = 0;
  int Rfirst = 1;
  int Rii = 0;

  void 
  Rrefresh(int Rsel)
  {
    char *RdirTt = NULL;

    int Rall = Rstrlen(Rm152)+Rstrlen(Rm153)+Rstrlen(Rm137)+2;

    wattron(RwinMsg, COLOR_PAIR(2));
    mvwhline(RwinMsg, 3, 3, ' ', Rxs-6); 

    RdirTt = strdup(RdirT);
    if (Rstrlen(RdirTt) > RmaxX)
      RdirTt[RmaxX] = 0;
    Rmvwprintw(RwinMsg, 3, 3, to_scr(RdirTt)); 
    Rfree(RdirTt);
    
    wattroff(RwinMsg, COLOR_PAIR(2));
    switch(Rsel) 
    {
      case 1 : Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2, Rm152); 
               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rm152)+1, Rm153); 

               wattron(RwinMsg, COLOR_PAIR(1)); 
               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rm152)+Rstrlen(Rm153)+2, Rm137); 
               wattroff(RwinMsg, COLOR_PAIR(1));
               break;  
      case 2 : Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2, Rm152); 

               wattron(RwinMsg, COLOR_PAIR(1)); 
               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rm152)+1, Rm153); 
               wattroff(RwinMsg, COLOR_PAIR(1));

               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rm152)+Rstrlen(Rm153)+2, Rm137); 
               break;
      case 3 : wattron(RwinMsg, COLOR_PAIR(1)); 
               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2, Rm152); 
               wattroff(RwinMsg, COLOR_PAIR(1));

               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rm152)+1, Rm153); 
               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rm152)+Rstrlen(Rm153)+2, Rm137); 
               break;
      case 4 : Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2, Rm152); 
               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rm152)+1, Rm153); 
               Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rm152)+Rstrlen(Rm153)+2, Rm137); 
               break;
    }
  }

  alarm(0);
  RdirT[0] = 0;
  Rys = 8; Rxs = 76;
  RmaxX = Rxs-6; 
  RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
  box(RwinMsg,0,0);
  Rmvwprintw(RwinMsg, 1, Rxs/2-Rstrlen(Rm154)/2, Rm154);
  Rrefresh(Rrol);
  RcancelFinding = 0;

  nodelay(RwinMsg,1);
  RactiveWindow = RwinMsg;
  RDBG("smbc : RfindFileSmb RactiveWindow = RwinMsg  2\n");
  while (Rrol) {
    RtryDownloadUpload();
    if (Rrol == 4) 
    { 
      Rfirst = 1;
      Rin1 = 0;
      Rin2 = 0;
      while (readlineOneCharNoDelay(RwinMsg, 3, 3, RdirT, MAXSTR, RmaxX,3, &RdirTpos, 1, &Rin1, &Rin2, &Rfirst) <= 0) 
        RtryDownloadUpload();
      Rrefresh(--Rrol);
    }
    Rkey = mvwgetch(RwinMsg,1,1);
    if (Rkey == 9) 
    { 
      if ((--Rrol) == 0) 
        Rrol = 4;
      Rrefresh(Rrol);
    }
    if (Rkey == 10) 
    {
      RstrdupN(RfileName, "/tmp/smbc_");
      Rintaddtoend(&RfileName, getuid());
      Rstraddtoend(&RfileName, "_");
      Rintaddtoend(&RfileName, getpid());
      if (Rrol == 2) 
      {    // slow find
        RindexFileCreate(RfileName, &RfileFd);                         // create file descriptor and remove file to other use
        RindexFileUnlink(RfileName);
        RcreateIndexFileOnline(RfileFd);
      } 
      else 
      {
        if (Rrol == 3) 
        {    // quick find
          if (!RmakeCopyFileWithPID(RfileName, RfileNameForFindList)) 
          {
            RshowMessage(INFO_INDEX_IS_EMPTY, NULL, NULL, NULL, DO_REDRAW_MAIN_WINDOW);
            RcancelFinding = 1;
            ret = 0;
          }
          else 
          {
            RindexFileOpen(RfileName, &RfileFd);
            RindexFileUnlink(RfileName);
          }
        }
        else
          Rrol = 0;
      }
      Rfree(RfileName);
      if (RcancelFinding)
        Rrol = 0;

      if (Rrol) 
      {
        RDBG("smbc: RfindFileSmb before Rrol\n");
        if (RindexFileExistAnyRecords(RfileFd))
        {

          // message
          // Loading find index file from disk to memory, please wait...
          Rys = 7; Rxs = Rstrlen(Rm49)+6;
          RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
          keypad(RwinMsg, TRUE);
          box(RwinMsg, 0, 0);
          Rii = Rxs/2-Rstrlen(Rm49)/2;
          Rmvwprintw(RwinMsg, 2, Rii, Rm49);
          wrefresh(RwinMsg);
          // END message

          //Rcount = RindexFileGetCount(RfileFd);
          Rcount = RindexFileReadCounter(RfileFd);
          RloadIndexFile(RfileFd, &RidxNet, Rcount, RwinMsg);
          Rdelwin(RwinMsg);
          ret = RmanageFindFile(RdirT, RgetStrVariable(V_GROUP), &Rahost, &Radir, SelectFile, RidxNet, Rcount);
          RfreeIndexFile(&RidxNet);
          if (ret == 1) 
          {
            if (Rstrlen(RgetStrVariable(V_GROUP)) > 0) 
              RremoteLevel = LEVEL_HOSTS;
            else 
              RremoteLevel = LEVEL_WORKGROUPS;
            if (Rstrlen(Rahost) > 0) 
            {
              RremoteLevel = LEVEL_SHARES;
              if (Rstrlen(Radir) > 1)
                RremoteLevel = LEVEL_FILES;
            }
            ret = 1;
          }
          RwinRedrawWithData();
          Rrol = 0;
        } 
        else 
        {
          RshowMessage(INFO_INDEX_IS_EMPTY, "", "", NULL, DO_REDRAW_MAIN_WINDOW); 
          Rrol = 0;
        }
      }
    }
    if (Rkey == 'q') 
      Rrol = 0;
  }
  RindexFileClose(&RfileFd);
  Rdelwin(RwinMsg);
  RwinRedrawWithData();
  RDBG("smbc: RfindFileSmb exit with result ");
  RDBGi(ret);
  RDBG("\n");
  alarm(RCalarm);
  return(ret);
}

void 
RcreateFindListIndexSecond(int Isave, char *Igroup, char *Ihost, char *Ipath, char *Idir, WINDOW *RwinI, int RfileFd, int *RrecordCount)
{
  int Rfd;
  int RxT, RyT;
  struct smbc_dirent *Rdirent = NULL;
  char *Rstr = NULL;
  char *Rstr2 = NULL;
  char *RstrTmpFLIS = NULL;
  char *IIgroup = NULL;
  char *IIhost = NULL;
  char *IIpath = NULL;
  char *IIdir = NULL;
  char *IItmp = NULL;
    
  void RcancelCheck()
  {
    char Rch = 0;

    while (Rch != ERR) 
    {
      Rch = wgetch(RwinI);
      if (Rch == 10) 
        RcancelFinding = 1;
    }
  }

  if (!Isave) 
  {
    keypad(RwinI, TRUE);
    nodelay(RwinI, 1);
    RactiveWindow = RwinI;
    RDBG("smbc : RcreateFindFileIndex RactiveWindow = RwinI  2\n");
    RcancelCheck();
    if (RcancelFinding) 
      return;
  }

  RDBG("smbc: RcreateFindListIndex start\n");

  RstrdupN(IIgroup, Igroup);
  RstrdupN(IIhost, Ihost);
  RstrdupN(IIdir, Idir);
  RstrdupN(IIpath, Ipath);
  getmaxyx(RwinI, RyT, RxT);

  RDBG("smbc: RcreateFindListIndex s5\n");

  if (!Isave) 
  {
    RDBG("smbc: RcreateFindListIndex a1\n");
    Rstr2 = strdup(to_scr(IIgroup)); 
    if (Rstrlen(Rstr2) > RxT) 
      RtruncateStr(&Rstr2, RxT);
    mvwhline(RwinI, 1, 0, 32, RxT); 
    Rmvwprintw(RwinI, 1, 0, Rstr2); 
    Rfree(Rstr2);

    RDBG("smbc: RcreateFindListIndex a3\n");
    Rstr2 = strdup(to_scr(IIhost)); 
    if (Rstrlen(Rstr2) > RxT) 
      RtruncateStr(&Rstr2, RxT);
    mvwhline(RwinI, 2, 0, 32, RxT); 
    Rmvwprintw(RwinI, 2, 0, Rstr2); 
    Rfree(Rstr2);
  
    RDBG("smbc: RcreateFindListIndex a5\n");
    RstrdupN(Rstr2, Ipath); 
    Rstraddtoend(&Rstr2, Rstrlen(Ipath)>0?"/":""); 
    Rstraddtoend(&Rstr2, IIdir); 
    if (Rstrlen(Rstr2) > RxT) 
      RtruncateStr(&Rstr2, RxT);
    mvwhline(RwinI, 3, 0, 32, RxT); 
    Rmvwprintw(RwinI, 3, 0, Rstr2); 
    Rfree(Rstr2);

    wrefresh(RwinI);
    
  }

  if (Rstrlen(IIgroup) == 0) 
    RstrdupN(Rstr, "smb://")
  else 
  {
    if (Rstrlen(IIhost) == 0) 
    {
      RstrdupN(Rstr, "smb://");
      Rstraddtoend(&Rstr, IIgroup);
      Rstraddtoend(&Rstr, "/");
    }
    else 
    {
      if (Rstrlen(Ipath) == 0) 
      {
        RstrdupN(Rstr, "smb://");
        Rstraddtoend(&Rstr, IIhost);
        Rstraddtoend(&Rstr, "/");
      }
      else 
      {
        if (Rstrlen(IIdir) == 0) 
        {
          RstrdupN(Rstr, "smb://");
          Rstraddtoend(&Rstr, IIhost);
          Rstraddtoend(&Rstr, "/");
          Rstraddtoend(&Rstr, IIpath);
          Rstraddtoend(&Rstr, "/");
        }
        else 
        {
          RstrdupN(Rstr, "smb://");
          Rstraddtoend(&Rstr, IIhost);
          Rstraddtoend(&Rstr, "/");
          Rstraddtoend(&Rstr, Ipath);
          Rstraddtoend(&Rstr, "/");
          Rstraddtoend(&Rstr, IIdir);
          Rstraddtoend(&Rstr, "/");

          RstrdupN(IIpath, Ipath);
          Rstraddtoend(&IIpath, "/");
          Rstraddtoend(&IIpath, Idir);
        }
      }
    }
  }
  Rfd = Rsmbc_opendir(Rstr);

  if (Rfd > 0) 
  {
    while (Rsmbc_readdir(Rfd, &Rdirent)) 
    {
      if (Rdirent != NULL) 
      {
        if ((Rstrlen(Rdirent->name) > 0) && (!RcancelFinding)) 
        {
          if (!Isave) 
            RcancelCheck();
          if (Rdirent->name[Rstrlen(Rdirent->name)-1] != '$') 
          {
            RstrdupN(IItmp, Rdirent->name);
            switch (Rdirent->smbc_type) 
            {
              case SMBC_WORKGROUP:  RDBG("smbc: RcreateFindListIndex !!! 4 - 1 !!!\n");
                                    RcreateFindListIndexSecond(Isave, IItmp, "", "", "", RwinI, RfileFd, RrecordCount);
                                    break;
              case SMBC_SERVER:     RDBG("smbc: RcreateFindListIndex !!! 4 - 2 !!!\n");
                                    RcreateFindListIndexSecond(Isave, IIgroup, IItmp, "", "", RwinI, RfileFd, RrecordCount);
                                    break;
              case SMBC_FILE_SHARE: RDBG("smbc: RcreateFindListIndex !!! 4 - 3 !!!\n");
                                    RcreateFindListIndexSecond(Isave, IIgroup, IIhost, IItmp, "", RwinI, RfileFd, RrecordCount);
                                    break;
              case SMBC_COMMS_SHARE:RDBG("smbc: RcreateFindListIndex !!! 4 - 4 !!!\n"); 
                                    break;
              case SMBC_DIR:        RDBG("smbc: RcreateFindListIndex !!! 4 - 5 !!!\n");
                                    if ((!RstrCmp(IItmp,".")) & (!RstrCmp(IItmp,".."))) 
                                    {
                                       RDBG("smbc: RcreateFindListIndex !!! 4 - 6 !!!\n");
                                       RcreateFindListIndexSecond(Isave, IIgroup, IIhost, IIpath, IItmp, RwinI, RfileFd, RrecordCount);
                                    }
                                    break;
              case SMBC_FILE:       RDBG("smbc: RcreateFindListIndex !!! 4 - 7 !!!\n");
                                    (*RrecordCount)++;
                                    RwriteStr(RfileFd, IIgroup);
                                    RwriteStr(RfileFd, IIhost);

                                    RstrdupN(RstrTmpFLIS, IIpath);
                                    Rcharaddtoend(&RstrTmpFLIS, '/');
                                    RwriteStr(RfileFd, RstrTmpFLIS);
                                    Rfree(RstrTmpFLIS);

                                    RwriteStr(RfileFd, IItmp);
                                    RwriteStr(RfileFd, "");                         // RfindIndex->access_denied
                                    RDBG("smbc: RcreateFindListIndex !!! 6 !!!\n");
                                    if (!Isave) 
                                    {
                                       mvwhline(RwinI, 4, 0, 32, RxT); 
                                       mvwprintw(RwinI, 4, 0, "%i", *RrecordCount); 
                                       wrefresh(RwinI); 
                                    }
                                    break;
              case SMBC_LINK:       break;
            }
            RDBG("smbc: RcreateFindListIndex !!! 8 !!!\n");
            Rfree(IItmp);
            RDBG("smbc: RcreateFindListIndex !!! 9 !!!\n");
          }
       }
      } 
    }
    smbc_closedir(Rfd);
  }
  RDBG("smbc: RcreateFindListIndex !!! 10 !!!\n");
  Rfree(Rstr);
  Rfree(IIgroup);
  Rfree(IIhost);
  Rfree(IIpath);
  Rfree(IIdir);
  RDBG("smbc: RcreateFindListIndex !!! 11 !!!\n");
  return;
}

void 
RcreateFindListIndex(int Isave, char *Igroup, char *Ihost, char *Ipath, char *Idir, WINDOW *RwinI, int RfileFd)
{
  int RrecordCount = 0;
  int RfileNameMessageTmpFd = 0;
  char *RfileNameMessageTmp = NULL;

  RstrdupN(RfileNameMessageTmp, from_sys(getenv("HOME")));
  Rstraddtoend(&RfileNameMessageTmp, "/.smbc/messageprim_");
  Rintaddtoend(&RfileNameMessageTmp, getuid());
  Rstraddtoend(&RfileNameMessageTmp, "_");
  Rintaddtoend(&RfileNameMessageTmp, getpid());

  if (RindexFileCreate(RfileNameMessageTmp, &RfileNameMessageTmpFd) == 0)
    kill(RchildId, SIGHUP);
  RindexFileUnlink(RfileNameMessageTmp);
  RcreateFindListIndexSecond(Isave, Igroup, Ihost, Ipath, Idir, RwinI, RfileNameMessageTmpFd, &RrecordCount);

  RwriteUnsignedInt(RfileFd, RrecordCount);
  lseek(RfileNameMessageTmpFd, 0, SEEK_SET);
  RmakeCopyFileFromPIDtoPID(RfileFd, RfileNameMessageTmpFd);
  close(RfileNameMessageTmpFd);
  Rfree(RfileNameMessageTmp);
}

void 
RsigTerm()
{
  int Ri;

  Ri = can_change_color();

  RdelwinE(RwinLeft);
  RdelwinE(RwinRight);
  RdelwinE(RwinHead);
  RdelwinE(RwinStatus);
  RdelwinE(RwinLeftList);
  RdelwinE(RwinRightList);
  RdelwinE(RwinLeftHead);
  RdelwinE(RwinRightHead);
  RdelwinE(RwinLeftTop);
  RdelwinE(RwinRightTop);
  RdelwinE(RwinLeftCol1);
  RdelwinE(RwinLeftCol2);
  RdelwinE(RwinLeftCol3);
  RdelwinE(RwinRightCol1);
  RdelwinE(RwinRightCol2);
  RdelwinE(RwinRightCol3);
  RdelwinE(RwinLeftHeadCol1);
  RdelwinE(RwinLeftHeadCol2);
  RdelwinE(RwinLeftHeadCol3);
  RdelwinE(RwinRightHeadCol1);
  RdelwinE(RwinRightHeadCol2);
  RdelwinE(RwinRightHeadCol3);
  RdelwinE(RwinMain);
  endwin();
  if (!Ri) 
  {
    printf(Rm156);
    printf(Rm157);
  }
  if ((RmainWinX < 80) || (RmainWinY < 25)) 
    printf(Rm158, RmainWinX, RmainWinY);
  RopenSaveDownloadList(0);
  RDBG("smbc : RsigTerm 2\n");
  if (RlockFile > 0) 
    close(RlockFile);

  RDBG("smbc : RsigTerm 3\n");
  if (Rerror) 
  {
    printf("**********************************************************\n");
    printf(Rm159);
    printf("**********************************************************\n");
  }

  RdoneIconv();

  RDBG("smbc : RsigTerm 5\n");
  RdoneStrings();
  
  exit(0);
}

void 
RsigCHLD()
{
  waitpid(RchildId, NULL, WNOHANG);
}

void 
RcreateFindIndex(int Rl, int Rlock, char *Rstr)
{
  int RlockL;
  int RindexFileOld;
  int RmessageFilePrim;
  int RindexFileFd = 0;
  char *RstrTcFI1 = NULL;

  if ((RchildId = fork()) == 0) 
  {
    if (RindexTime == 0) 
      exit(0);         // child
    if (Rl) 
    {
      if (Rlock) 
        close(Rlock);

      RlockL = open(to_sys(Rstr), O_RDWR | O_CREAT, 0600);

      RmessageFilePrim = open(to_sys(RfileNameMessagePrim), O_RDWR | O_CREAT, 0600);
      if (RmessageFilePrim > 0) 
      {
        write(RmessageFilePrim, Rm67, Rstrlen(Rm67));
        close(RmessageFilePrim);
        rename(to_sys(RfileNameMessagePrim), to_sys(RfileNameMessage));
      }

      while (1) 
      {
        if (RlockL > 0) 
          if (!flock(RlockL, LOCK_EX | LOCK_NB)) 
             exit(0);                                // kill smbc-child when parent was killed SIGKILL

        if (RindexFileFd == 0) 
          if (RindexFileCreate(RfileNameForFindListPrim, &RindexFileFd)) 
          {
             RcreateFindListIndex(1, "","","","", NULL, RindexFileFd);

             unlink(to_sys(RfileNameMessagePrim));
             RmessageFilePrim = open(to_sys(RfileNameMessagePrim), O_RDWR | O_CREAT, 0600);

             RindexFileOld = open(to_sys(RfileNameForFindList), O_RDONLY);
             RcompareTwoIndexFile(RindexFileFd, RindexFileOld, RmessageFilePrim);
             if (RindexFileOld > 0)
               close(RindexFileOld);
             close(RindexFileFd); 
             close(RmessageFilePrim);
             RindexFileFd = 0;

             RstrdupN(RstrTcFI1, to_sys(RfileNameMessagePrim));
             rename(RstrTcFI1, to_sys(RfileNameMessage));

             RstrdupN(RstrTcFI1, to_sys(RfileNameForFindListPrim));
             rename(RstrTcFI1, to_sys(RfileNameForFindList));
             Rfree(RstrTcFI1);
          }
        if (RindexTime > 1) 
          sleep(RindexTime); 
        else
          exit(0); 
      }
    } 
    else 
      exit(0);
  }
  RDBG("smbc : RcreateFindIndex parent\n");
}

/*     output:  RrightFirstI
                RrightPosI
        input:  RstrTmpI    - file name
                Rselected
*/
void 
RsetSelect(int *RFirstI, int *RPosI, char *RstrTmpI, PANEL_TYPE Rselected)
{
  int Ri = 0;
  char *Rcmp = NULL;
  TTlist *RlistT;

  if (RstrTmpI == NULL) 
  {
    *RFirstI = 0;
    *RPosI = 0;
    return;
  }
  if (Rselected == PANEL_RIGHT) 
  {
    RlistT = RlistRight;
    while (RlistT) 
    {
      switch (RremoteLevel) 
      {
        case (LEVEL_HOSTS) : 
        case (LEVEL_SHARES) : 
             Rcmp = RlistT->col4;
             break;
        default : 
             Rcmp = RlistT->col2;
      }
      if (RstrCmp(Rcmp, RstrTmpI)) 
      {
        if (Ri > RmainWinY-6) 
        {
          if (RreturnCountTTlist(RlistRight)-Ri < RmainWinY-6) 
          {
            *RFirstI = RreturnCountTTlist(RlistRight) - (RmainWinY-6);
            *RPosI = Ri-(*RFirstI);
          }
          else 
          {
            *RFirstI = Ri-1;
            *RPosI = 1;
          }
          return; 
        }
        else 
        {
          *RPosI = Ri;
          *RFirstI = 0;
          return;
        }
      }
      if (Ri+1 < RreturnCountTTlist(RlistRight)) 
        Ri++;
      else 
        return;
      RlistT = RlistT->next;
    }
  }
  else 
  {
    RlistT = RlistLeft;
    while (RlistT) 
    {
      if (RstrCmp(RlistT->col2, RstrTmpI)) 
      {
        if (Ri > RmainWinY-6) 
        {
          if (RreturnCountTTlist(RlistLeft)-Ri < RmainWinY-6)
          {
            *RFirstI = RreturnCountTTlist(RlistLeft) - (RmainWinY-6);
            *RPosI = Ri-(*RFirstI);
          }
          else 
          {
            *RFirstI = Ri-1;
            *RPosI = 1;
          }
          return; 
        }
        else 
        {
          *RPosI = Ri;
          *RFirstI = 0;
          return;
        }
      }
      if (Ri+1 < RreturnCountTTlist(RlistLeft)) 
        Ri++;
      else 
      {
        return;
      }
      RlistT = RlistT->next;
    }
  }
}

void 
RcompareTwoIndexFile(int RfdNew, int RfdOld, int RfdMessage)
{
  TTfind *RfindIndexNew;
  TTfind *RfindIndexOld;
  int Rfound;
  int RnewAdd = 0;
  int RnewDel = 0;
  char *RnewHosts = NULL;
  char *RoldHosts = NULL;
  char *RstrT = NULL;

  TThostsTable *RhostsTableNew = NULL;
  TThostsTable *RhostsTableOld = NULL;
  TThostsTable *RhostsTableNewTmp;
  TThostsTable *RhostsTableOldTmp;
  
  RDBG("smbc: RcompareTwoIndexFile start\n");
  if (RfdOld < 0) 
  {
    write(RfdMessage, Rm162, Rstrlen(Rm162)+1);
    RDBG("smbc: RcompareTwoIndexFile return short\n");
    return;
  }
  else 
  {
    RDBG("smbc: RcompareTwoIndexFile seek files\n");
    RindexFileSetSeek(RfdNew, 1);
    RindexFileSetSeek(RfdOld, 1);

    //RDBG("smbc: RcompareTwoIndexFile while 1\n");
    while (RindexFileReadOneRecord(RfdNew, &RfindIndexNew)) 
    {
       //RDBG("smbc: RcompareTwoIndexFile while 1 - 2\n");
       if (!(RFindHostInToHostsTable(RhostsTableNew, RfindIndexNew->host)))
         RAddHostToHostsTable(&RhostsTableNew, RfindIndexNew->host);
       RindexFreeOneTTfind(&RfindIndexNew);
    }

#ifdef HAVE_DEBUG
    RDBG("smbc: RcompareTwoIndexFile RhostsTableNew = ");
    RhostsTableNewTmp = RhostsTableNew;
    while (RhostsTableNewTmp) 
    {
      RDBG(RhostsTableNewTmp->name); RDBG(" ");
      RhostsTableNewTmp = RhostsTableNewTmp->next;
    }
    RDBG("\n");
#endif /* HAVE_DEBUG */    

    //RDBG("smbc: RcompareTwoIndexFile while 2\n");
    while (RindexFileReadOneRecord(RfdOld, &RfindIndexOld)) 
    {
       //RDBG("smbc: RcompareTwoIndexFile while 2 - 2\n");
       if (!(RFindHostInToHostsTable(RhostsTableOld, RfindIndexOld->host)))
         RAddHostToHostsTable(&RhostsTableOld, RfindIndexOld->host);
       RindexFreeOneTTfind(&RfindIndexOld);
    }

#ifdef HAVE_DEBUG
    RDBG("smbc: RcompareTwoIndexFile RhostsTableOld = ");
    RhostsTableOldTmp = RhostsTableOld;
    while (RhostsTableOldTmp) 
    {
      RDBG(RhostsTableOldTmp->name); RDBG(" ");
      RhostsTableOldTmp = RhostsTableOldTmp->next;
    }
    RDBG("\n");
#endif /* HAVE_DEBUG */    

    /* find diference - add */
    RDBG("smbc: RcompareTwoIndexFile seek before add while\n");
    RhostsTableNewTmp = RhostsTableNew;
    while (RhostsTableNewTmp) 
    {
      //RDBG("smbc: RcompareTwoIndexFile while 1 - 1\n");
      RhostsTableOldTmp = RhostsTableOld;
      Rfound = 0;
      while (RhostsTableOldTmp && !Rfound) 
      {
        RDBG("smbc: RcompareTwoIndexFile : "); 
        RDBG(RhostsTableNewTmp->name); 
        RDBG(RstrCmp(RhostsTableNewTmp->name, RhostsTableOldTmp->name)?" == ":" != "); 
        RDBG(RhostsTableOldTmp->name);
        RDBG("\n");

        if (RstrCmp(RhostsTableNewTmp->name, RhostsTableOldTmp->name)) 
          Rfound = 1;
        else
          RhostsTableOldTmp = RhostsTableOldTmp->next;
      }
      RDBG("smbc: RcompareTwoIndexFile while after\n");
      if (!Rfound) 
      {
        Rcharaddtoend(&RnewHosts, ',');
        Rstraddtoend(&RnewHosts, RhostsTableNewTmp->name);
        RnewAdd++;
      }
      RhostsTableNewTmp = RhostsTableNewTmp->next;
    } 
    RDBG("smbc: RcompareTwoIndexFile while 1, result : ");
    RDBG(RnewHosts); RDBG("\n");
    /* find diference - del */
    RDBG("smbc: RcompareTwoIndexFile seek before del while\n");
    RhostsTableOldTmp = RhostsTableOld;
    while(RhostsTableOldTmp) 
    {
      RhostsTableNewTmp = RhostsTableNew;
      Rfound = 0;
      while (RhostsTableNewTmp && !Rfound) 
      {
        if (RstrCmp(RhostsTableNewTmp->name, RhostsTableOldTmp->name)) 
          Rfound = 1;
        else
          RhostsTableNewTmp = RhostsTableNewTmp->next;
      }
      if (!Rfound) 
      {
        Rcharaddtoend(&RoldHosts, ',');
        Rstraddtoend(&RoldHosts, RhostsTableOldTmp->name);
        RnewDel++;
      }
      RhostsTableOldTmp = RhostsTableOldTmp->next;
    }
    RDBG("smbc: RcompareTwoIndexFile while 2, result : ");
    RDBG(RoldHosts); RDBG("\n");

    RDBG("smbc: RcompareTwoIndexFile seek before making return\n");
    if ((RnewAdd == 0) && (RnewDel == 0)) 
      write(RfdMessage, Rm162, Rstrlen(Rm162)+1);
    else 
    {
      RstrT = strdup(Rm163);
      if (RnewAdd > 0) 
      {
        Rintaddtoend(&RstrT, RnewAdd);
        Rstraddtoend(&RstrT, Rm179);
      }
      if (RnewDel > 0) 
      {
        Rintaddtoend(&RstrT, RnewDel); 
        Rstraddtoend(&RstrT, Rm180); 
      }
      if (RnewAdd > 0) 
      {
        Rstraddtoend(&RstrT, Rm181);
        Rstraddtoend(&RstrT, RnewHosts);
        Rstraddtoend(&RstrT, RoldHosts?",":"");
      }
      if (RnewDel > 0) 
      {
        Rstraddtoend(&RstrT, Rm182);
        Rstraddtoend(&RstrT, RoldHosts);
      }
      write(RfdMessage, RstrT, Rstrlen(RstrT)+1);
    }
    Rfree(RstrT);
    Rfree(RnewHosts);
    Rfree(RoldHosts);
    RFreeHostsTable(&RhostsTableNew);
    RFreeHostsTable(&RhostsTableOld);
  }
  RDBG("smbc: RcompareTwoIndexFile finished\n");
}

void 
RcheckNewIndexFile(char *RfileName)
{
  int Rfd;
  char *RstrT1 = NULL;
  char *RstrT2 = NULL;
  char Rchar;
  struct stat Rfstat;
  int Rlen;

  Rfd = stat(to_sys(RfileName), &Rfstat);
  if (Rfd == 0) 
  {
    if (RlastTimeFileMessage != Rfstat.st_ctime) 
    {
      RlastTimeFileMessage = Rfstat.st_ctime;
      Rfd = open(to_sys(RfileName), O_RDONLY);
      if (Rfd > 0) 
      {
        RstrdupN(RstrT1, "");
        while (read(Rfd, &Rchar, 1) == 1) 
          Rcharaddtoend(&RstrT1, Rchar);
      }
      Rlen = (Rstrlen(RstrT1)+1>RmainWinX)?RmainWinX:Rstrlen(RstrT1)+1;
      RstrT2 = Rmalloc(Rlen);
      memcpy(RstrT2, RstrT1, Rlen-1);
      RstrT2[Rlen] = 0;

      mvwhline(RwinStatus, 0, 0, ' ', RmainWinX);
      Rmvwprintw(RwinStatus, 0, 0, RstrT2);
      wrefresh(RwinStatus);

      Rfree(RstrT1);
      Rfree(RstrT2);
      close(Rfd);
    }
  }
}

void 
RviewFile(PANEL_TYPE Rsel, char *Rstr)
{
  WINDOW *WinView = NULL;
  WINDOW *WinViewScroll = NULL;
  int Rkey = 0;
  int Rfilehandle = 0;
  struct stat remotestat;
  int Rsize = 0;
  off_t Rseek = 0;
  off_t RseekTmp = 0;
  int RwSize, RwReaded;
  int Rtx, Rty;
  char *Rbuf = NULL;
  int RfirstLineLen = 0;
  int RPageLen = 0;
  char RwrapView = 0;
  char Rloop;
  int Rtyt, Rtxt;

  void RreadAndPaint(WINDOW *RWin, PANEL_TYPE Rsel, int Rhandle, off_t Rseek, int Rsize, int Rwrap)
  {
    int Rt;
    int Rix = 0;
    int Riy = 0;
    char *RbufT = NULL;

    memset(Rbuf, 0, RwSize);
    if (Rsel == PANEL_LEFT) 
    {
      lseek(Rhandle, Rseek, SEEK_SET);
      RwReaded = read(Rhandle, Rbuf, RwSize);
    }
    else 
    {
      Rsmbc_lseek(Rhandle, Rseek, SEEK_SET);
      RwReaded = Rsmbc_read(Rhandle, Rbuf, RwSize);
    }
    RDBG("smbc: RbufT = Rbuf;\n");
    RbufT = Rbuf;
    RwReaded = Rstrlen(RbufT);
    memcpy(Rbuf, RbufT, RwReaded+1);
    RDBG("smbc: memcpy(Rbuf, RbufT, RwReaded+1);\n");
    
    werase(RWin);
    RPageLen = 0;
    if (RwReaded > 0) 
      for (Rt = 0; Rt < RwReaded; Rt++) 
      {
        if (Rwrap) 
        { 
          if (Riy == 0) 
          {
            if (Rix == Rtx)
              RfirstLineLen = Rix;
            else 
              RfirstLineLen = Rix+1;
          }
          if (Rix == Rtx) 
          {
            Rix = 0;
            Riy++;
          }
        } 
        if (Riy < Rty)
          RPageLen++;
        if (Rbuf[Rt] == 10) 
        {
          if ((!Rwrap) && (Riy == 0))
              RfirstLineLen = Rix+1;
          Rix = 0;
          Riy++;
        }
        else 
          mvwprintw(RWin, Riy, Rix++, "%c", Rbuf[Rt]);
      }
    wrefresh(RWin);
  }

  if (Rsel == PANEL_LEFT) 
  {
    Rfilehandle = open(to_sys(Rstr), O_RDONLY, 0755);
    if (Rfilehandle < 1) 
    {
      RshowMessage(INFO_PERMISSION_DENIED, Rstr, NULL, NULL, DO_REDRAW_MAIN_WINDOW);
      return;
    }
    fstat(Rfilehandle, &remotestat);
    Rsize = remotestat.st_size;
  } 
  else 
  {
    Rfilehandle = Rsmbc_open(Rstr, O_RDONLY, 0755);
    if (Rfilehandle < 1) 
    {
      RshowMessage(INFO_PERMISSION_DENIED, Rstr, NULL, NULL, DO_REDRAW_MAIN_WINDOW);
      return;
    }
    Rsmbc_fstat(Rfilehandle, &remotestat);
    Rsize = remotestat.st_size;
  }

  WinView = newwin(RmainWinY-2, RmainWinX, 1, 0);  
  WinViewScroll = newwin(RmainWinY-4, RmainWinX-2, 2, 1);
  getmaxyx(WinViewScroll, Rty, Rtx);
  RwSize = Rtx*Rty;
  Rbuf = Rmalloc(RwSize);
  
  box(WinView, 0, 0);
  RAstatusBar(STATUS_DEFAULT_FOR_VIEW_FILE);
  wrefresh(WinView);

  RreadAndPaint(WinViewScroll, Rsel, Rfilehandle, Rseek, RwSize, RwrapView);
  nodelay(RwinMain, 1);
  RactiveWindow = RwinMain;
  RDBG("smbc : RviewFile RactiveWindow = RwinMain  2\n");
  while ((Rkey = mvwgetch(RwinMain, 1,1))) 
  {
    RtryDownloadUpload();
    if ((Rkey == 267) || (Rkey == 'q'))    // F3
      break;
    if ((Rkey == 272) || (Rkey == 'w')) 
    {                                      // F8
      if (RwrapView) 
        RwrapView = 0;
      else 
        RwrapView = 1;
      RreadAndPaint(WinViewScroll, Rsel, Rfilehandle, Rseek, RwSize, RwrapView);
    }
    if ((Rkey == KEY_DOWN || Rkey == 'o')) 
    {
      if (Rseek < Rsize)
        Rseek = Rseek+RfirstLineLen;
      RreadAndPaint(WinViewScroll, Rsel, Rfilehandle, Rseek, RwSize, RwrapView);
    }
    if (Rkey == KEY_UP || Rkey == 'p') 
    {
      if (Rseek > 0) 
      {
        Rloop = 1;
        RseekTmp = Rseek;
        while (Rloop) 
        {
          RseekTmp--;
          if (Rseek - RseekTmp == 1) 
            RseekTmp--; 
          if (RseekTmp <= 0) 
            Rloop = 0;
          if (Rsel == PANEL_LEFT) 
          {
            lseek(Rfilehandle, RseekTmp, SEEK_SET);
            RwReaded = read(Rfilehandle, Rbuf, 1);
          }
          else 
          {
            Rsmbc_lseek(Rfilehandle, RseekTmp, SEEK_SET);
            RwReaded = Rsmbc_read(Rfilehandle, Rbuf, 1);
          }
          if (Rbuf[0] == 10) 
          {
            Rloop = 0;
            RseekTmp++;
          }
          if (Rseek - RseekTmp > Rtx)
            Rloop = 0;
        }
        Rseek = RseekTmp;
      }
      RreadAndPaint(WinViewScroll, Rsel, Rfilehandle, Rseek, RwSize, RwrapView);
    }
    if ((Rkey == KEY_NPAGE || Rkey == 'k')) 
    {
      if ((Rseek < Rsize) && (Rseek + RPageLen < Rsize)) 
        Rseek = Rseek+RPageLen;
      RreadAndPaint(WinViewScroll, Rsel, Rfilehandle, Rseek, RwSize, RwrapView);
    }
    if ((Rkey == KEY_PPAGE || Rkey == 'l')) 
    {
      if (Rseek > 0) 
      {
        Rloop = 1;
        Rtyt = 0;
        Rtxt = 0;
        RseekTmp = Rseek;
        while (Rloop) 
        {
          RseekTmp--;
          if (RseekTmp <= 0) 
            Rloop = 0;
          if (Rsel == PANEL_LEFT) 
          {
            lseek(Rfilehandle, RseekTmp, SEEK_SET);
            RwReaded = read(Rfilehandle, Rbuf, 1);
          }
          else 
          {
            Rsmbc_lseek(Rfilehandle, RseekTmp, SEEK_SET);
            RwReaded = Rsmbc_read(Rfilehandle, Rbuf, 1);
          }
          if ((Rbuf[0] == 10) || (Rtxt >= Rtx)) 
          {
            Rtyt++;
            Rtxt = 0;
          }
          if (Rtyt >= Rty)
             Rloop = 0;
          Rtxt++;
        }
        Rseek = RseekTmp;
      }
      RreadAndPaint(WinViewScroll, Rsel, Rfilehandle, Rseek, RwSize, RwrapView);
    }
  }
  Rdelwin(WinView);
  Rdelwin(WinViewScroll);
  if (Rfilehandle) 
  {
    if (Rsel == PANEL_LEFT) 
      close(Rfilehandle);
    else
      Rsmbc_close(Rfilehandle);
  }
}

void 
RtryReadMessage()
{
  DIR *Rdirh;
  struct dirent *Rsdirent;
  char *RfileName = NULL;
  int Rfh;
  char Rch;
  char *Rl = NULL;
  char *Rll = NULL;
  char *Rl1 = NULL;
  char *Rl2 = NULL;
  int Rfound = 1;

  // ----- read popup message START ----- 
  while (Rfound) 
  {
    Rfound = 0;
    if ((Rdirh = opendir(to_sys(RMESSAGEDIR))) > 0) 
    {
      while ((Rsdirent = readdir(Rdirh)) > 0) 
      {
        //RDBG("smbc: RtryReadMessage 1:"); RDBG(Rsdirent->d_name); RDBG(":"); RDBG(RMESSAGEFILE); RDBG("\n");
        if (RstrCmpIncludeFirst(Rsdirent->d_name, RMESSAGEFILE)) 
        {
          Rfound = 1;
          //asprintf(&RfileName, "%s/%s", RMESSAGEDIR, Rsdirent->d_name);
          RstrdupN(RfileName, RMESSAGEDIR);
          Rstraddtoend(&RfileName, "/");
          Rstraddtoend(&RfileName, Rsdirent->d_name);
          RDBG("smbc: RtryReadMessage found:"); RDBG(RfileName); RDBG("\n");
          if ((Rfh = open(to_sys(RfileName), O_RDONLY)) > 0) 
          {
            while (read(Rfh, &Rch, 1) == 1) 
            {
              if ((Rch == 10) && (!Rl1)) 
              {
                Rl1 = Rll;
                RDBG("smbc: RtryReadMessage Rll = "); RDBG(Rll); RDBG("\n");
                Rll = NULL;
              }
              else 
              {
                if (Rll) 
                {
                  //asprintf(&Rl, "%s%c", Rll, Rch);
                  RstrdupN(Rl, Rll);
                  Rcharaddtoend(&Rl, Rch);
                }
                else
                {
                  asprintf(&Rl, "%c", Rch);
                }
                Rfree(Rll);
                Rll = Rl;
              }
            }
            Rl2 = Rll;
            close(Rfh);
            RshowMessage(INFO_NEW_MESSAGE_X, Rl1, Rl2, NULL, DO_REDRAW_MAIN_WINDOW);
            Rfree(Rl1);
            Rfree(Rl2);
          }
          unlink(to_sys(RfileName));
          Rfree(RfileName);
        }
      }
      closedir(Rdirh);
    }
  }
  // ----- read popup message END ----- 
}

void 
RsendWinpopupInput(char *RgroupInput)
{
  WINDOW *RwinMsg = NULL;
  int Rys, Rxs;
  int Rrol = 3;
  int Rxb = 0;
  char Rthost[RMAXGHLEN];
  char Rtmessage[RWINPOPUPMAXLEN];
  int Rdx = 12;
  int Rkey;
  int Rin1 = 0;
  int Rin2 = 0;
  int Rfirst = 1;
  int Rctrlp;
  char *RstrT1 = NULL;
  
  void 
  Rrefresh(int Rmax, int Rsel)
  {
    char *Rmsg41 = NULL;
    char *Rmsg42 = NULL;

    RstrdupN(Rmsg41, Rm136);
    RstrdupN(Rmsg42, Rm137);
    int Rall = Rstrlen(Rmsg41)+Rstrlen(Rmsg42);

    wattron(RwinMsg, COLOR_PAIR(2));
    RstrT1 = to_scr(Rthost);                      
    if (Rstrlen(RstrT1) > Rmax) 
      RstrT1[Rmax] = 0;
    mvwhline(RwinMsg, 4, Rdx+1, 32, Rxs-Rdx-3); 
    Rmvwprintw(RwinMsg, 4, Rdx+1, RstrT1);

    RstrT1 = to_scr(Rtmessage); 
    if (Rstrlen(RstrT1) > Rmax) 
      RstrT1[Rmax] = 0;
    mvwhline(RwinMsg, 6, Rdx+1, 32, Rxs-Rdx-3); 
    Rmvwprintw(RwinMsg, 6, Rdx+1, RstrT1);

    wattroff(RwinMsg, COLOR_PAIR(2));
    switch (Rsel) 
    {
       case 1 : Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2, Rmsg41);
                wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rmsg41)+1, Rmsg42); 
                  wattroff(RwinMsg, COLOR_PAIR(1)); 
                break;
       case 2 : wattron(RwinMsg, COLOR_PAIR(1)); 
                  Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2, Rmsg41); 
                  wattroff(RwinMsg, COLOR_PAIR(1)); 
                Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rmsg41)+1, Rmsg42);
                break;
       case 3 : Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2, Rmsg41);
                Rmvwprintw(RwinMsg, Rys-2, (Rxs-Rall)/2+Rstrlen(Rmsg41)+1, Rmsg42);
                break;
    }
    Rfree(Rmsg41); 
    Rfree(Rmsg42);
  }
 
  memcpy(Rthost, RgroupInput, Rstrlen(RgroupInput)+1);
  Rtmessage[0] = 0;

  Rys = 11; Rxs = 70;
  Rxb = Rxs-Rdx-3;
  RwinMsg = newwin(Rys, Rxs, RmainWinY/2-Rys/2, RmainWinX/2-Rxs/2);
  box(RwinMsg,0,0);
  Rmvwprintw(RwinMsg, 1, Rxs/2-Rstrlen(Rm170)/2, Rm170);
  Rmvwprintw(RwinMsg, 4, Rdx-Rstrlen(Rm168), Rm168);
  Rmvwprintw(RwinMsg, 6, Rdx-Rstrlen(Rm169), Rm169);
  Rrefresh(Rxb, Rrol);
  nodelay(RwinMsg, 1);
  RactiveWindow = RwinMsg;
  RDBG("smbc : RsendWinpopupMessage RactiveWindow = RwinMsg  2\n");
  while (Rrol) {
    RtryDownloadUpload();
    if (Rrol == 3) {
      Rin1 = 0;
      Rin2 = 0;
      Rfirst = 1;
      while (readlineOneCharNoDelay(RwinMsg, 4, Rdx+1, Rthost, RMAXGHLEN, Rxb, 3, &Rctrlp, 1, &Rin1, &Rin2, &Rfirst) <= 0) 
        RtryDownloadUpload();
      Rrefresh(Rxb, Rrol);
      Rin1 = 0;
      Rin2 = 0;
      Rfirst = 1;
      while (readlineOneCharNoDelay(RwinMsg, 6, Rdx+1, Rtmessage, RWINPOPUPMAXLEN, Rxb, 3, &Rctrlp, 1, &Rin1, &Rin2, &Rfirst) <= 0) 
        RtryDownloadUpload();

      Rrefresh(Rxb, --Rrol);
    }
    else 
    {
      Rkey = mvwgetch(RwinMsg, 1,1);
      if (Rkey == 9) 
      { 
        if ((--Rrol) == 0) 
          Rrol = 3;
        Rrefresh(Rxb, Rrol);
      }
      if (Rkey == 10) 
      { 
        if (Rrol == 2) 
          RsendWinpopup(Rsmbclient, Rthost, Rtmessage);
        Rrol = 0;
      }
      if (Rkey == 'q') 
        Rrol = 0;
    }
  }
  Rdelwin(RwinMsg);
}

void 
RsigSEGV()
{
  int Rres = errno;

  RDBG("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  RDBG("Fatal error, errno = ");
  RDBGi(Rres);
  RDBG(Rres == EILSEQ?" EILSEQ - Illegal byte sequence":"");
  RDBG("\n");
  RDBG("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  Rerror = 1;
  RsigTerm();
  exit(Rres);
}

//
// ------------------- Rbookmark START ------------------------
//
int 
Rbookmark(PANEL_TYPE *Riselect, char *Riagroup, char **Riahost, char **Riadir, char **RiadirL, 
          char **Rifile, char *Riusername, char *Ripassword, char **RfileOutput)
{
  WINDOW *Rwin = NULL;
  WINDOW *RwinDel = NULL;
  WINDOW *RwinRL = NULL;
  WINDOW *RwinPath = NULL;
  int Rkey;
  TTbookmark *Rbookmarks = NULL;
  TTbookmark *RbookmarksTmp = NULL;
  int Rfirst = 0;
  int Rselect = 0;
  char *RbookmarkFileName = NULL;
  int RmaxY;
  unsigned int RcountBookmark;
  int RchangeBookmark = 0;
  int Raction = 0;
  int Riy, Rix;
  int Rcut = 0;
  
  void 
  RpaintList(int Rifirst, int Riselect)
  {
    int Rilast, Ri, Rt;
    int Rk = 0;
    TTbookmark *RbookmarksTmp = Rbookmarks;
    char *RstrTmp = NULL;
    
    RgotoXBookmark(&RbookmarksTmp, Rifirst);
    RDBG("smbc Rbookmark_RpaintList RcountBookmark = "); RDBGi(RcountBookmark); RDBG("\n");
    if ((Rifirst == 0) & (RcountBookmark <= RmaxY)) 
      Rilast = RcountBookmark;
    else 
      Rilast = Rifirst + RmaxY;

    for (Ri = Rfirst; Ri < Rilast; Ri++) 
    {
      mvwhline(RwinDel, Rk, 0, 32, 1);
      mvwhline(RwinRL, Rk, 0, 32, 1);
      mvwhline(RwinPath, Rk, 0, 32, Rix-6);
      if (Ri == Rselect) 
      {
        wattron(RwinDel, COLOR_PAIR(1));
        wattron(RwinRL, COLOR_PAIR(1));
        wattron(RwinPath, COLOR_PAIR(1));

        mvwhline(RwinDel, Rk, 0, 32, 1);
        mvwhline(RwinRL, Rk, 0, 32, 1);
        mvwhline(RwinPath, Rk, 0, 32, Rix-6);
      }
      mvwprintw(RwinDel, Rk, 0, "%c", RbookmarksTmp->del);
      mvwprintw(RwinRL, Rk, 0, "%c", RbookmarksTmp->type);

      if (RbookmarksTmp->type == 'R')
      {
        RstrdupN(RstrTmp, RbookmarksTmp->group);
        Rstraddtoend(&RstrTmp, strlen(RstrTmp)?" ":"");
        Rstraddtoend(&RstrTmp, RbookmarksTmp->dir);
        Rstraddtoend(&RstrTmp, RbookmarksTmp->file);
      }
      else
      {
        RstrdupN(RstrTmp, RbookmarksTmp->dir);
        Rstraddtoend(&RstrTmp, RbookmarksTmp->file);
      }
      Rt = Rstrlen(RstrTmp);
      if (Rt > Rix-6)
        RstrTmp[Rix-6] = 0;
      Rmvwprintw(RwinPath, Rk, 0, to_scr(RstrTmp));
      Rfree(RstrTmp);

      if (Ri == Rselect) 
      {
        wattroff(RwinDel, COLOR_PAIR(1));
        wattroff(RwinRL, COLOR_PAIR(1));
        wattroff(RwinPath, COLOR_PAIR(1));
      }
      RbookmarksTmp = RbookmarksTmp->next;
      Rk++;
    }
    for (Ri = Rilast+1; Ri < Rifirst+RmaxY+1; Ri++) 
    {
      mvwhline(RwinDel, ++Rk, 0, 32, 1);
      mvwhline(RwinRL, Rk, 0, 32, 1);
      mvwhline(RwinPath, Rk, 0, 32, Rix-6);
    }
    wrefresh(RwinPath);
    wrefresh(RwinDel);
    wrefresh(RwinRL);
  }

  void 
  RcreateBookmarkWindow()
  {
    Rdelwin(Rwin);
    Rdelwin(RwinDel);
    Rdelwin(RwinRL);
    Rdelwin(RwinPath);

    getmaxyx(RwinMain, Riy, Rix);
    RmaxY = Riy-6;
    Rwin = newwin(Riy-2, Rix, 1, 0);
    RwinDel = newwin(Riy-6, 1, 4, 1);
    RwinRL = newwin(Riy-6, 1, 4, 3);
    RwinPath = newwin(Riy-6, Rix-6, 4, 5);
    Rmvwprintw(Rwin, 1,1, Rm171);
    mvwvline(Rwin, 1, 2, 0, Riy-4);
    mvwvline(Rwin, 1, 4, 0, Riy-4);
    mvwhline(Rwin, 2, 1, 0, Rix-2);
    box(Rwin, 0, 0);
    wrefresh(Rwin);
    wrefresh(RwinDel);
    wrefresh(RwinRL);
    wrefresh(RwinPath);
    keypad(Rwin, TRUE);
    nodelay(Rwin, 1);
    RactiveWindow = Rwin;
    RDBG("smbc : RcreateBookmarkWindow RactiveWindow = Rwin  2\n");
    RAstatusBar(STATUS_DEFAULT_FOR_BOOKMARK);
    RpaintList(Rfirst, Rselect);
  }

  void 
  RresizeBookmark()
  {
    if (RdoResizeBookmark) 
    {
      RdoResizeBookmark = 0;
      RdoWindowResize();
      RcreateBookmarkWindow();
    }
  }

  //asprintf(&RbookmarkFileName, "%s/.smbc/bookmark", from_sys(getenv("HOME")));
  RstrdupN(RbookmarkFileName, from_sys(getenv("HOME")));
  Rstraddtoend(&RbookmarkFileName, "/.smbc/bookmark");
  RopenBookmark(&Rbookmarks, RbookmarkFileName);
  RcountBookmark = RgetBookmarkCount(Rbookmarks);

  RcreateBookmarkWindow();

  while (1) 
  {
    Rkey = mvwgetch(Rwin, 1, 1);
    RtryDownloadUpload();
    RresizeBookmark();
    if (Rkey == 'q') 
      break;
    if (Rkey == 'm') 
    {
      if (Rcut) 
      {
        Rcut = 0;
        RAstatusBar(STATUS_DEFAULT_FOR_BOOKMARK);
      }
      else 
      {
        Rcut = 1;
        RAstatusBar(STATUS_DEFAULT_FOR_BOOKMARK_MOVE);
      }
    }
    if (Rkey == 'b') 
    {
      if (!RfindInBookmark(&Rbookmarks, *Riselect == PANEL_RIGHT?'R':'L', Riagroup, *Riahost, 
          *Riselect == PANEL_RIGHT?*Riadir:*RiadirL, *Rifile, Riusername, Ripassword)) 
      {
        RaddToBookmark(&Rbookmarks, *Riselect == PANEL_RIGHT?'R':'L', Riagroup, *Riahost,
                       *Riselect == PANEL_RIGHT?*Riadir:*RiadirL, *Rifile, Riusername, Ripassword);
        RchangeBookmark = 1;
      }
      else 
        RshowMessage(INFO_X, Rm172, NULL, NULL, DO_REDRAW_MAIN_WINDOW);
      break;
    }
    if (Rkey == 'd' || Rkey == KEY_DC) 
    {
       RgotoXBookmark(&Rbookmarks, Rselect);
       Rbookmarks->del = 'D';
       RchangeBookmark = 1;
       Rkey = KEY_DOWN;
    }
    if (Rkey == 'u') 
    {
       RgotoXBookmark(&Rbookmarks, Rselect);
       Rbookmarks->del = 'X';
       RchangeBookmark = 1;
       Rkey = KEY_DOWN;
    }
    if (Rkey == KEY_DOWN || Rkey == 'o') 
    {
      RgotoXBookmark(&Rbookmarks, Rselect);
      if ((Rcut) && (Rbookmarks->next)) 
      {
        RchangeBookmark = 1;
        RbookmarksTmp = Rbookmarks;
        if (Rbookmarks->previous)
          Rbookmarks->previous->next = Rbookmarks->next;
        Rbookmarks->next->previous = Rbookmarks->previous;

        Rbookmarks = Rbookmarks->next;

        RbookmarksTmp->previous = Rbookmarks;
        RbookmarksTmp->next = Rbookmarks->next;
        
        if (Rbookmarks->next)
          Rbookmarks->next->previous = RbookmarksTmp;
        Rbookmarks->next = RbookmarksTmp;
      }
      if ((Rselect+1 == Rfirst+RmaxY) & (Rfirst+RmaxY < RcountBookmark)) 
      {
        Rfirst++; 
        Rselect++;
      }
      else 
        if (Rselect < RcountBookmark-1) 
          Rselect++;
      RDBG("smbc Rbookmark Rfirst = "); RDBGi(Rfirst); RDBG(", Rselect = "); RDBGi(Rselect); RDBG("\n");
      RpaintList(Rfirst,Rselect);
    }
    if (Rkey == KEY_UP || Rkey == 'p') 
    {
      RgotoXBookmark(&Rbookmarks, Rselect);
      if ((Rcut) && (Rbookmarks->previous)) 
      {
        RchangeBookmark = 1;
        RbookmarksTmp = Rbookmarks;
        if (Rbookmarks->next)
          Rbookmarks->next->previous = Rbookmarks->previous;
        Rbookmarks->previous->next = Rbookmarks->next;

        Rbookmarks = Rbookmarks->previous;

        RbookmarksTmp->next = Rbookmarks;
        RbookmarksTmp->previous = Rbookmarks->previous;
        
        if (Rbookmarks->previous)
          Rbookmarks->previous->next = RbookmarksTmp;
        Rbookmarks->previous = RbookmarksTmp;
      }
      if (Rselect > 0) 
      {
        if ((Rfirst > 0) && (Rselect == Rfirst)) 
        { 
          Rfirst--; 
          Rselect--; 
        } 
        else 
          Rselect--;
        RpaintList(Rfirst,Rselect);
      }
    }
    if (Rkey == KEY_NPAGE || Rkey == 'k') 
    {
      if (Rfirst+RmaxY <= RcountBookmark) 
      {
        if (Rselect+1 == Rfirst+RmaxY) 
        {
           if (Rselect+RmaxY < RcountBookmark) 
           {
              Rfirst = Rselect;
              Rselect = Rfirst+RmaxY-1;
           }
           else 
           {
              Rfirst = RcountBookmark-RmaxY;
              Rselect = RcountBookmark-1;
           }
        }
        else 
        {
          if (Rselect+RmaxY <= RcountBookmark) 
            Rselect = Rfirst+RmaxY-1;
          else
            Rselect = RcountBookmark-1;
        }
      }
      RpaintList(Rfirst,Rselect);
    }
    if (Rkey == KEY_PPAGE || Rkey == 'l') 
    {
      if (Rselect > 1) 
      {
        if (Rfirst == Rselect) 
        {
          if (Rfirst - RmaxY < 1) 
          {
            Rfirst = 1;
            Rselect = 1;
          }
          else 
          {
            Rfirst = Rfirst - RmaxY + 1;
            Rselect = Rfirst;
          }
        }
        else 
          Rselect = Rfirst;
        RpaintList(Rfirst,Rselect);
      }
    }
    if (Rkey == 10 || Rkey == 'x') 
    {
      Raction = 1;
      RgotoXBookmark(&Rbookmarks, Rselect);
      if (Rbookmarks->type == 'L') 
      {
        *Riselect = PANEL_LEFT;
        RstrdupN(*RiadirL, Rbookmarks->dir);
        RstrdupN(*RfileOutput, Rbookmarks->file);
      }
      else 
      {
        RDBG("smbc: Rbookmark 1\n");
        *Riselect = PANEL_RIGHT;
        RstrdupN(*Riahost, Rbookmarks->host);
        RstrdupN(*Riadir, Rbookmarks->dir);
        RstrdupN(*RfileOutput, Rbookmarks->file);
        RDBG("smbc: Rbookmark 2\n");
        RsetStrVariable(V_GROUP, Rbookmarks->group);
        RsetStrVariable(V_PASSWORD, Rbookmarks->username);
        RsetStrVariable(V_PASSWORD, Rbookmarks->password);
      }
      break;
    }
  };
  if (RchangeBookmark)
    RsaveBookmark(Rbookmarks, RbookmarkFileName);
  Rdelwin(Rwin);
  Rdelwin(RwinDel);
  Rdelwin(RwinRL);
  Rdelwin(RwinPath);
  RfreeBookmark(Rbookmarks);
  Rfree(RbookmarkFileName);
  return(Raction);
}
//
// ------------------- Rbookmark END ------------------------
//

/* detect smbc find index process and run them if not exist */

void 
RtryLockFileAndCreateIndexFile()
{
  RDBG("smbc: RtryLockFileAndCreateIndexFile start\n");
  if (!RlockFileFlag) 
  {
    RlockFile = open(to_sys(RsmbcLockFile), O_RDWR | O_CREAT, 0700);
    if (RlockFile > 0) 
    {
      if (!flock(RlockFile, LOCK_EX | LOCK_NB)) 
      {
        RlockFileFlag = 1;
        unlink(to_sys(RfileNameForFindList));
        unlink(to_sys(RfileNameMessage));
        RcreateFindIndex(RlockFileFlag, RlockFile, RsmbcLockFile);
      }
      else 
        close(RlockFile);
    }
    if (RrunFirst) 
    {
      signal(SIGCHLD, RsigCHLD);
      signal(SIGSEGV, RsigSEGV);
      signal(SIGTERM, RsigTerm);
      signal(SIGINT, RsigTerm);
      signal(SIGALRM, RsigAlarm);
      RrunFirst = 0;
    }
  }
  RDBG("smbc: RtryLockFileAndCreateIndexFile end\n");
}

int 
RtryDownloadUpload() 
{
  if (RdownloadList != NULL) 
  {
    if (!RdontDownload)
      RdownloadUpload();  
      RstatusBarStatus = 1;
      if (RactuallyNothingDownloadUpload) 
      {
        nodelay(RactiveWindow, 0);
        RDBG("smbc : RtryDownloadUpload nodelay = 0\n");
      }
      else 
      {
        nodelay(RactiveWindow, 1);
        RDBG("smbc : RtryDownloadUpload nodelay = 1\n");
      }
      return(1);
    }
    else 
    {
      if (RstatusBarStatus) 
      {
        RAstatusBar(STATUS_ADDING_FILES_TO_UPLOAD_LIST);
        RstatusBarStatus = 0;
      }
      nodelay(RactiveWindow, 0);
      RDBG("smbc : RtryDownloadUpload nodelay = 0  2\n");
    }
  return(0);
}

void 
RsigAlarm()
{
  char RinsertChar = 1;

  /*  enter unused char to keyboard buffer
      necessery because mvwgetch may be wait for char  */

  ioctl(0, TIOCSTI, &RinsertChar);

  if (!RdontResizeWindow)
    RdoWindowResize();

  RtryLockFileAndCreateIndexFile();
  RtryReadMessage();
  RDBG("smbc : RsigAlarm 4\n");

  alarm(RCalarm);
}

int 
main(int argc,char *argv[])
{
 int Rnr;
 int Rkey;
 char *RstrTmp = NULL;
 int Rwhat;
 char *Rstr = NULL;
 char *RstrT = NULL;
 char Ropt;
 poptContext Rpopt;
 int RnrDulist;
 char *RargMessageIn = NULL;
 char *RargFileNameIn = NULL;
 char *RargMessage = NULL;
 char *RargFileName = NULL;
 int RchangePos;
 char *Rtmps = NULL;

 struct poptOption Roptions[] = {
   { "help", 'h', POPT_ARG_NONE, NULL, 'h', 0, 0 },
   { "flush", 'f', POPT_ARG_NONE, NULL, 'f', 0, 0 },
   { "list", 'l', POPT_ARG_NONE, NULL, 'l', 0, 0 },
   { "show", 's', POPT_ARG_INT, &RnrDulist, 's', 0, 0 },
   { "sleep", 'p', POPT_ARG_NONE, NULL, 'p', 0, 0 },
   { "message", 'm', POPT_ARG_STRING, &RargMessageIn, 'm', 0, 0 },
   { "file", 'i', POPT_ARG_STRING, &RargFileNameIn, 'i', 0, 0 },
   { "version", 'V', POPT_ARG_NONE, NULL, 'V', 0, 0 },
   { NULL, 0, 0, NULL, 0, NULL, NULL }
 };

 void 
 RrefreshR()
 {
   if (Rselected == PANEL_LEFT) 
   {
     Rnr = RgetFileFromDirL(RadirL);
     RinitList(Rnr,0,0);
     RleftFirst = 0;
     RleftPos = 0;
     RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft); 
   }
   else 
   {
     if (RremoteLevel == LEVEL_WORKGROUPS)
       Rnr = RgetMasterList(1); 
     else
     if (RremoteLevel ==  LEVEL_HOSTS)
       Rnr = RgetHostsFromGroup(RgetStrVariable(V_GROUP), 1); 
     else
     if (RremoteLevel ==  LEVEL_SHARES)
       Rnr = RgetSharesFromHost(Rahost, 1);
     else
     if (RremoteLevel == LEVEL_FILES)
       Rnr = RgetFileFromDirR(Radir, 1); 

     RinitList(Rnr, RremoteLevel, 1); 
     RrightFirst = 0;
     RrightPos = 0;
     RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
   }
 }

// ----------------------------------------------------------------------- 
// ----------------------------------------------------------------------- 
 RsetStrVariable(V_GROUP, "");
 Rahost = Rstrdup("");
 Radir = Rstrdup("");
 RadirL = Rstrdup("");
 RsetStrVariable(V_USERNAME, RGUEST);
 RsetStrVariable(V_PASSWORD, "");

 setlocale(LC_ALL, "");
 bindtextdomain("smbc", LOCALEDIR);
 textdomain("smbc");

 RinitIconv();
 RinitStrings();

 RstrdupN(RsmbcLockFile, from_sys(getenv("HOME")));
 Rstraddtoend(&RsmbcLockFile, "/.smbc/debug");
 RinitDebug(RsmbcLockFile);

 Roptions[0].descrip = Rm300;
 Roptions[1].descrip = Rm301;
 Roptions[2].descrip = Rm302;
 Roptions[3].descrip = Rm303;
 Roptions[4].descrip = Rm304;
 Roptions[5].descrip = Rm305;
 Roptions[6].descrip = Rm306;
 Roptions[7].descrip = Rm307;

 Rpopt = poptGetContext("smbc", argc, (const char **) argv, Roptions, POPT_CONTEXT_KEEP_FIRST);
 poptSetOtherOptionHelp(Rpopt, Rm173);

 while ((Ropt = poptGetNextOpt(Rpopt)) >= 0) 
 {
   if (Ropt == POPT_ERROR_BADNUMBER) 
     exit(0);
   switch (Ropt) 
   {
     case 'h': poptPrintHelp(Rpopt, stderr, 0);
               exit(0);
     case 'f': RflushAllDownloadLists();
               exit(0);
     case 'l': RlistAllDownloadLists();
               exit(0);
     case 's': RShowAllItemsFromDUlist(RnrDulist);
               exit(0);
     case 'p': RdontDownload = 1;
               break;
     case 'm': RargMessage = strdup(RargMessageIn);
               if (RsaveMessage(RargMessage, RargFileName))
                 exit(0);
               break;
     case 'i': RargFileName = strdup(RargFileNameIn);
               if (RsaveMessage(RargMessage, RargFileName))
                 exit(0);
               break;
     case 'V': RdisplayVersion();
               exit(0);
   }
 }
 if (Ropt < -1) 
 {
   fprintf(stderr, "%s: %s\n", poptBadOption(Rpopt, POPT_BADOPTION_NOALIAS), poptStrerror(Ropt));
   exit(0);
 }
 poptFreeContext(Rpopt);

 Rfree(RsmbcLockFile);
 RDBG("----start----\n");

 RstrdupN(RsmbcLockFile, from_sys(getenv("HOME")));
 Rstraddtoend(&RsmbcLockFile, "/.smbc/smbc.lock");

 RstrdupN(RadirL, from_sys(getenv("HOME")));

 RreadRcfile(); 

 RopenSaveDownloadList(1);

 if (Rstrlen(RadirL) > 0) 
   if (RadirL[Rstrlen(RadirL)-1] != '/') 
     Rcharaddtoend(&RadirL, '/');

 signal(SIGWINCH, RresizeHandler);

 if(Rsmbc_init(Rget_auth_data, 0) < 0) 
 {
   fprintf(stderr, Rm175, errno);
   if (errno == 2) 
     fprintf(stderr, Rm176);
   exit(0);
 }

RDBG("smbc: main 100\n");
#ifdef HAVE_SMBCCTX
 Rstatcont = Rsmbc_new_context();
 if (!Rstatcont)
     return -1;
 Rstatcont->debug = 0;
 Rstatcont->callbacks.auth_fn = Rget_auth_data;
 if (!Rsmbc_init_context(Rstatcont)) 
 {
   Rsmbc_free_context(Rstatcont, 0);
   return -1;
 }
 Rstatcont->timeout = Rtimeout;
#endif /* HAVE_SMBCCTX */

 RstrdupN(RfileNameForFindList, from_sys(getenv("HOME")));
 Rstraddtoend(&RfileNameForFindList, "/.smbc/findfile");

 RstrdupN(RfileNameForFindListPrim, from_sys(getenv("HOME")));
 Rstraddtoend(&RfileNameForFindListPrim, "/.smbc/findfileprim");

 RstrdupN(RfileNameMessage, from_sys(getenv("HOME")));
 Rstraddtoend(&RfileNameMessage, "/.smbc/message");

 RstrdupN(RfileNameMessagePrim, from_sys(getenv("HOME")));
 Rstraddtoend(&RfileNameMessagePrim, "/.smbc/messageprim");

#ifdef HAVE_SET_CONTEXT
   Rsmbc_set_context(Rstatcont);
#else /* not HAVE_SET_CONTEXT */
   printf(Rm177);
#endif /* not HAVE_SET_CONTEXT */

 RtryLockFileAndCreateIndexFile();

 RwriteMenu(1);
 RinitList(RgetFileFromDirL(RadirL), 0, PANEL_LEFT);

 if ((Rstrlen(Radir) > 0) && (Rstrlen(Rahost) > 0) && (Rstrlen(RgetStrVariable(V_GROUP)) > 0)) 
   RremoteLevel = LEVEL_FILES;
 else
   if ((Rstrlen(Rahost) > 0) && (Rstrlen(RgetStrVariable(V_GROUP)) > 0)) 
     RremoteLevel = LEVEL_SHARES;
   else
     if (Rstrlen(RgetStrVariable(V_GROUP)) > 0) 
       RremoteLevel = LEVEL_HOSTS;
     else 
       RremoteLevel = LEVEL_WORKGROUPS;

 RDBG("smbc: main Radir = "); RDBG(Radir); RDBG("\n");
 RrefreshR();

 RinitList(Rnr, RremoteLevel, PANEL_RIGHT);
 RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
 RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);

 keypad(RwinMain, TRUE); 
 nodelay(RwinMain, 1);
 while ((Rkey = mvwgetch(RwinMain, 1, 1))) 
 {
   RactiveWindow = RwinMain;
   alarm(RCalarm);
   /* 
    *  (v, F3) - view file, execude directory
    */
   if ((Rkey == 'v') || (Rkey == 267)) 
   {                                                  
     Rkey = 0;
     if (Rselected == PANEL_LEFT) 
     { 
       RgoToRecordInTTlistMaster(&RlistTmainLeft, RleftFirst+RleftPos, RlistLeft);
       if (RlistTmainLeft->col1[0] == 'F') 
       {
         RstrdupN(Rstr, RadirL);
         Rstraddtoend(&Rstr, "/");
         Rstraddtoend(&Rstr, RlistTmainLeft->col2);
         RviewFile(Rselected, Rstr);
         Rfree(Rstr);
         wclear(RwinMain);
         RwinRedrawWithData(); 
       } 
       else 
         Rkey = 10;
     } 
     else 
     {
       RgoToRecordInTTlistMaster(&RlistTmainRight, RrightFirst+RrightPos, RlistRight);
       if (RlistTmainRight->col1[0] == 'F') 
       {
         RstrdupN(Rstr, Radir);
         Rstraddtoend(&Rstr, "/");
         Rstraddtoend(&Rstr, RlistRight->col2);
         RviewFile(Rselected, Rstr);
         Rfree(Rstr);
         wclear(RwinMain);
         RwinRedrawWithData(); 
       } 
       else 
         Rkey = 10;
     }
   }
   /* 
    *  (x, enter) - execude directory
    */
   if (Rkey == 10 || Rkey == 'x') 
   {
     if (Rselected == PANEL_LEFT) 
     { 
       if (RstrCmp(RadirL, "/"))
         continue;
       
       RgoToRecordInTTlistMaster(&RlistTmainLeft, RleftFirst+RleftPos, RlistLeft);
       if (RlistTmainLeft->col1[0] == 'D') 
       {
         RstrdupN(RstrT, RadirL);
         RDBG("smbc: main 1 RstrT = "); RDBG(RstrT); RDBG("\n");
         if (RstrCmp(RlistTmainLeft->col2, ".."))
         {
           // UP
           RchdirUp(&RstrT);
           RDBG("smbc: main 2 RstrT = "); RDBG(RstrT); RDBG("\n");
           Rnr = RgetFileFromDirL(RstrT);
           RinitList(Rnr, 0, 0);

           RgetFileFromTwoDirs(&RfileNamePos, RadirL, RstrT);
           RsetSelect(&RleftFirst, &RleftPos, RfileNamePos , PANEL_LEFT);
           Rfree(RfileNamePos);

           RstrdupN(RadirL, RstrT);
           RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
           if (Rnr == 0)
           {
             // dir is empty
             RAstatusBar(STATUS_DIR_IS_EMPTY);
           }
           if (Rnr == -1)
           {
             // access denied
             RAstatusBar(STATUS_ACCESS_DENIED);
           }
         }
         else
         {
           // DOWN
           Rstraddtoend(&RstrT, RlistTmainLeft->col2);
           Rstraddtoend(&RstrT, "/");
           RDBG("smbc: main 3 RstrT = "); RDBG(RstrT); RDBG("\n");
           
           Rnr = RgetFileFromDirL(RstrT);
           if (Rnr != -1)
           {
             RinitList(Rnr, 0, 0);

             RgetFileFromTwoDirs(&RfileNamePos, RadirL, RstrT);
             RsetSelect(&RleftFirst, &RleftPos, RfileNamePos , PANEL_LEFT);
             Rfree(RfileNamePos);

             RstrdupN(RadirL, RstrT);
             RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
             if (Rnr == 0)
             {
               // dir is empty
               RAstatusBar(STATUS_DIR_IS_EMPTY);
             }
           }
           else
           {
             // access denied
             RshowMessage(INFO_I_CANT_ENTER_TO_X_ACCESS_DENIED, RlistTmainLeft->col2, "", NULL, DO_REDRAW_MAIN_WINDOW);
           }
         }
         Rfree(RstrT);
       }
     }
     else 
     {
       RgoToRecordInTTlistMaster(&RlistTmainRight, RrightFirst+RrightPos, RlistRight);
       if (RlistTmainRight->col1[0] == 'D') 
       {
          if (RstrCmp(RlistTmainRight->col2, "..")) 
          {
            if (RremoteLevel == LEVEL_HOSTS) 
            /*
                 UP - Enter from hosts list to workgroups list
            */
            {
               RDBG("smbc: main RmainWinX = "); RDBGi(RmainWinX); RDBG("\n");
               Rnr = RgetMasterList(1);
               RremoteLevel = LEVEL_WORKGROUPS;
               RinitList(Rnr, RremoteLevel, 1);

               RsetSelect(&RrightFirst, &RrightPos, RgetStrVariable(V_GROUP), PANEL_RIGHT);
               RsetStrVariable(V_GROUP, "");

               RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight); 
               RAstatusBar(STATUS_FOUNDED_WORKGROUPS);
               if (Rnr == 0)
               {
                 // workgroups not found
                 RAstatusBar(STATUS_WORKGROUPS_NOT_FOUND);
               }
               if (Rnr == -1)
               {
                 // access denied 
                 RAstatusBar(STATUS_ACCESS_DENIED);
               }
            } 
            else
            if (RremoteLevel == LEVEL_SHARES) 
            /*
                 UP - Enter from shares to hosts
            */
            {
               Rnr = RgetHostsFromGroup(RgetStrVariable(V_GROUP), 1);
               RremoteLevel = LEVEL_HOSTS;
               RinitList(Rnr, RremoteLevel, 1);

               RsetSelect(&RrightFirst, &RrightPos, Rahost , PANEL_RIGHT);
               RstrdupN(Rahost, "");

               RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight); 
               RAstatusBar(STATUS_FOUNDED_HOSTS);
               if (Rnr == 0)
               {
                 // workgroup is empty
                 RAstatusBar(STATUS_WORKGROUP_IS_EMPTY);
               }
               if (Rnr == -1)
               {
                 // access denied 
                 RAstatusBar(STATUS_ACCESS_DENIED);
               }
            } 
            else
            if (RremoteLevel == LEVEL_FILES) 
              /*
                   UP - Enter from dirs to shares or to dirs
              */
            {
              RstrdupN(RstrT, Radir);
              RchdirUp(&RstrT);
              if ((RstrCmp(RstrT, "/")) || (strlen(RstrT) == 0))
              /*
                   UP - Enter from dirs to shares
              */
              {
                Rnr = RgetSharesFromHost(Rahost, 1);
                RremoteLevel = LEVEL_SHARES;
                RinitList(Rnr, RremoteLevel, 1);

                RgetFileFromTwoDirs(&RfileNamePos, Radir, RstrT);
                RsetSelect(&RrightFirst, &RrightPos, RfileNamePos, PANEL_RIGHT);
                Rfree(RfileNamePos);
                RstrdupN(Radir, "");

                RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
                RAstatusBar(STATUS_FOUNDED_SHARES);
                if (Rnr == 0)
                {
                  // host is empty
                  RAstatusBar(STATUS_HOST_IS_EMPTY);
                }
                if (Rnr == -1)
                {
                  // access denied 
                  RAstatusBar(STATUS_ACCESS_DENIED);
                }
              }
              else 
              /*
                   UP - Enter from dirs to dirs
              */
              {
                Rnr = RgetFileFromDirR(RstrT, 1);
                RinitList(Rnr, RremoteLevel, 1);

                RgetFileFromTwoDirs(&RfileNamePos, Radir, RstrT);
                RsetSelect(&RrightFirst, &RrightPos, RfileNamePos, PANEL_RIGHT);
                Rfree(RfileNamePos);
                RstrdupN(Radir, RstrT);

                RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
                RAstatusBar(STATUS_FOUNDED_DIRS);
                if (Rnr == 0)
                {
                  // dir is empty
                  RAstatusBar(STATUS_DIR_IS_EMPTY);
                }
                if (Rnr == -1)
                {
                  // access denied
                  RAstatusBar(STATUS_ACCESS_DENIED);
                }
              }
              Rfree(RstrT);
            } 
          } 
          else 
         /*
              DOWN - Enter from dir to dir
         */
          {
            RstrdupN(RstrT, Radir);
            Rstraddtoend(&RstrT, RlistTmainRight->col2);
            Rstraddtoend(&RstrT, "/");
            Rnr = RgetFileFromDirR(RstrT, 1);
            if (Rnr != -1) 
            {
              RstrdupN(Radir, RstrT);
              RinitList(Rnr, RremoteLevel, 1);
              RsetSelect(&RrightFirst, &RrightPos, NULL , PANEL_RIGHT);
              RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
              RAstatusBar(STATUS_FOUNDED_DIRS);
              if (Rnr == 0)
              {
                // dir is empty
                RAstatusBar(STATUS_DIR_IS_EMPTY);
              }
            } 
            else 
            {
              // access denied to file
              RshowMessage(INFO_I_CANT_ENTER_TO_X_ACCESS_DENIED, RlistTmainRight->col2, "", NULL, DO_REDRAW_MAIN_WINDOW);
            }
            Rfree(RstrT);
          }
       }
       else
       {
         /*
              DOWN - Enter from network to workgroup
         */
         if (RlistTmainRight->col1[0] == 'W') 
         {
           RstrdupN(RstrT, RlistTmainRight->col2);
           Rnr = RgetHostsFromGroup(RstrT, 1);
           if (Rnr != -1)
           {
             RsetStrVariable(V_GROUP, RstrT);
             RremoteLevel = LEVEL_HOSTS;
             RinitList(Rnr, RremoteLevel, 1);

             RrightFirst = 0;
             RrightPos = 0;
             RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight); 
             RAstatusBar(STATUS_FOUNDED_HOSTS);
             if (Rnr == 0) 
             {
               // workgroup is empty
               RAstatusBar(STATUS_WORKGROUP_IS_EMPTY);
             }
           } 
           else 
           {
             // access denied to workgroup
             RshowMessage(INFO_I_CANT_ENTER_TO_X_ACCESS_DENIED, RlistTmainRight->col2, "", NULL, DO_REDRAW_MAIN_WINDOW);
           }
           Rfree(RstrT);
         }
         else
           /*
               DOWN - Enter from workgroup to host
           */
           if (RlistTmainRight->col1[0] == 'H') 
           {
             RstrdupN(RstrT, RlistTmainRight->col4);
             Rnr = RgetSharesFromHost(RstrT, 1);
             RDBG("smbc: main 1 Rnr = "); RDBGi(Rnr); RDBG("\n");
             if (Rnr != -1) 
             {
               // host have some shares
               RstrdupN(Rahost, RstrT);
               RremoteLevel = LEVEL_SHARES;
               RinitList(Rnr, RremoteLevel, 1);

               RsetSelect(&RrightFirst, &RrightPos, NULL, PANEL_RIGHT);
               RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
               RAstatusBar(STATUS_FOUNDED_SHARES);
               if (Rnr == 0) 
               {
                 // host haven't any shares
                 RAstatusBar(STATUS_HOST_IS_EMPTY);
               }
             } 
             else 
             {
               // access denied to host
               RshowMessage(INFO_I_CANT_ENTER_TO_X_ACCESS_DENIED, RlistTmainRight->col4, "", NULL, DO_REDRAW_MAIN_WINDOW);
             }
             Rfree(RstrT);
           } 
           else
           /*
               DOWN - Enter from shares to dirs
           */
             if (RlistTmainRight->col1[0] == 'S') 
             {
               RDBG("smbc: main enter to dir 1\n");
               RstrdupN(RstrT, RlistTmainRight->col4);
               Rstraddtoend(&RstrT, "/");
               Rnr = RgetFileFromDirR(RstrT, 1);
               if (Rnr != -1) 
               {
                 // share have some files/dirs
                 RstrdupN(Radir, RstrT);
                 RremoteLevel = LEVEL_FILES;
                 RinitList(Rnr, RremoteLevel, 1);
                 RsetSelect(&RrightFirst, &RrightPos, NULL , PANEL_RIGHT);
                 RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
                 RAstatusBar(STATUS_FOUNDED_DIRS);
                 if (Rnr == 0)
                 {
                   // share is empty
                   RAstatusBar(STATUS_SHARE_IS_EMPTY);
                 }
               }
               else 
               {
                 // pemision denied to share
                 RshowMessage(INFO_I_CANT_ENTER_TO_X_ACCESS_DENIED, RlistTmainRight->col4, "", NULL, DO_REDRAW_MAIN_WINDOW);
               }
               Rfree(RstrT);
             } 
       }
     }
   }
   /* 
    *  (CTRL-d, F8) - delete 
    */
   if (Rkey == 4 || Rkey == 272) 
   {
    if (!RdetectScreenToSmall()) 
    {
     if (Rselected == PANEL_RIGHT) 
     {
       if (RexistAnySelect(RlistRight)) 
       {
         if (RshowMessage(INFO_DO_YOU_WANT_DELETE, Rm178, "", NULL, DO_REDRAW_MAIN_WINDOW)) 
         {
           RlistTmainRight = RlistRight;
           while (RlistTmainRight)
           {
             if (RlistTmainRight->select) 
             { 
               RstrdupN(Rtmps, Radir);
               Rstraddtoend(&Rtmps, RlistTmainRight->col2);
               if (RlistTmainRight->col1[0] == 'F') 
               {
                 if (Rsmbc_unlink(Rtmps)) 
                   RshowMessage(INFO_PERMISSION_DENIED, Rtmps, "", NULL, DO_REDRAW_MAIN_WINDOW);
               }
               else 
               {
                 if (RremoveDirR(Rtmps)) 
                   RshowMessage(INFO_PERMISSION_DENIED, Rtmps, "", NULL, DO_REDRAW_MAIN_WINDOW);
                 RwinRedrawWithData();
               }
               Rfree(Rtmps);
               Rkey = 'r';
             }
             RlistTmainRight = RlistTmainRight->next;
           }
           RlistTmainRight = RlistRight;
         }
       } 
       else 
       {
         RgoToRecordInTTlistMaster(&RlistTmainRight, RrightFirst+RrightPos, RlistRight);
         if (!RstrCmp(RlistTmainRight->col2,"..")) 
          if (RshowMessage(INFO_DO_YOU_WANT_DELETE, RlistTmainRight->col2, "", NULL, DO_REDRAW_MAIN_WINDOW)) 
          {
            RstrdupN(Rstr, Radir);
            Rstraddtoend(&Rstr, RlistTmainRight->col2);
            if (RlistTmainRight->col1[0] == 'F') 
            {
               if (Rsmbc_unlink(Rstr)) 
                 RshowMessage(INFO_PERMISSION_DENIED, Rstr, "", NULL, DO_REDRAW_MAIN_WINDOW);
            }
            else 
            {
              if (RremoveDirR(Rstr)) 
                RshowMessage(INFO_PERMISSION_DENIED, Rstr, "", NULL, DO_REDRAW_MAIN_WINDOW);
              RwinRedrawWithData();
            }
            Rfree(Rstr);
            Rkey = 'r';
          }
       }
     } 
     else 
     {
       RlistTmainLeft = RlistLeft;
       RgoToFirstTTlist(&RlistTmainLeft);
       if (RexistAnySelect(RlistLeft)) 
       {
         if (RshowMessage(INFO_DO_YOU_WANT_DELETE, Rm178, "", NULL, DO_REDRAW_MAIN_WINDOW)) 
         {
           RgoToFirstTTlist(&RlistTmainLeft);
           while (RlistTmainLeft)
           {
             if (RlistTmainLeft->select) 
             { 
               RstrdupN(Rtmps, RadirL);
               Rstraddtoend(&Rtmps, RlistTmainLeft->col2);
               if (RlistTmainLeft->col1[0] == 'F') 
                 unlink(to_sys(Rtmps));
               else 
               {
                 RremoveDir(Rtmps);
                 RwinRedrawWithData();
               }
               Rfree(Rtmps);
               Rkey = 'r';
             }
             RlistTmainLeft = RlistTmainLeft->next;
           }
           RlistTmainLeft = RlistLeft;
         }
       } 
       else {
         RlistTmainRight = RlistRight;
         RgoToRecordInTTlistMaster(&RlistTmainLeft, RleftFirst+RleftPos, RlistLeft);
         if (!RstrCmp(RlistTmainLeft->col2, "..")) 
          if (RshowMessage(INFO_DO_YOU_WANT_DELETE, RlistTmainLeft->col2, "", NULL, DO_REDRAW_MAIN_WINDOW)) 
          {
            RstrdupN(Rtmps, RadirL);
            Rstraddtoend(&Rtmps, RlistTmainLeft->col2);
            if (RlistTmainLeft->col1[0] == 'F') 
              unlink(to_sys(Rtmps));
            else 
            { 
              RremoveDir(Rtmps);
              RwinRedrawWithData();
            }
            Rfree(Rtmps);
            Rkey = 'r';
          }
       }
     }
    }
    else 
      RwinRedrawWithData();
   }
   /* 
    *  (m) - send WinPopup message
    */
   if (Rkey == 'm') 
   {
     if (!RdetectScreenToSmall()) 
     {
       if (RremoteLevel == LEVEL_HOSTS)
       {
         RlistTmainRight = RlistRight;
         RgoToRecordInTTlistMaster(&RlistTmainRight, RrightFirst+RrightPos, RlistRight);
         RsendWinpopupInput(RlistTmainRight->col4);
       }
       else 
         if (RremoteLevel > LEVEL_HOSTS)
           RsendWinpopupInput(Rahost);
         else 
           RsendWinpopupInput("");
       Rkey = 'r';
     }
     RwinRedrawWithData();
   }
   /* 
    *  (g) - goto share, host or workgroup
    */
   if (Rkey == 'g') 
   {
     if (Rselected == PANEL_RIGHT) 
     {
       if (RsetManualyLocation(1, RgetStrVariable(V_GROUP), &Rahost, &Radir, 
                               RgetStrVariable(V_USERNAME), RgetStrVariable(V_PASSWORD)) == 1) 
       {
         if (Rstrlen(RgetStrVariable(V_GROUP)) > 0) 
           RremoteLevel = LEVEL_HOSTS;
         else 
           RremoteLevel = LEVEL_WORKGROUPS;
         if (Rstrlen(Rahost) > 0) 
         {
           RremoteLevel = LEVEL_SHARES;
           if (Rstrlen(index(Radir+6+Rstrlen(Rahost),'/')) > 1) 
             RremoteLevel = LEVEL_FILES;
         }
         Rkey = 'r';
       }
     }
   }
   /* 
    *  (c, F7) - create directory
    */
   if (Rkey == 'c' || Rkey == 271) 
   {
    if (!RdetectScreenToSmall()) 
    {
     if (Rselected == PANEL_RIGHT) 
     {
       if (RremoteLevel == LEVEL_FILES) 
       {
         RcreateDir(Rselected, Radir);
         Rkey = 'r';  
       }
     }
     else 
     {
       RcreateDir(Rselected, RadirL);
       Rkey = 'r';
     }
    }
    else RwinRedrawWithData();
   }
   /* 
    *  (f) - search files
    */
   if (Rkey == 'f') 
   {
     if (!RdetectScreenToSmall()) 
     {
       if(Rselected == PANEL_RIGHT) 
       {
         if (RfindFileSmb(Radir, &RstrTmp)) 
         {
           RrefreshR();
           RDBG("smbc: main f RstrTmp = "); RDBG(RstrTmp); RDBG("\n");
           RsetSelect(&RrightFirst, &RrightPos, RstrTmp, Rselected);
         }
         RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
       }
     } 
     else 
       RwinRedrawWithData();
   }
   /* 
    *  (e) - sort change
    */
   if (Rkey == 'e') 
   {
     if (Rselected == PANEL_RIGHT) 
     {
       RsortMethodR++;
       if (RsortMethodR > 4) 
         RsortMethodR = 1;
       RAstatusBar(STATUS_PREPARING_FIND_INDEX+RsortMethodR);
     }
     else 
     {
       RsortMethodL++;
       if (RsortMethodL > 4) 
         RsortMethodL = 1;
       RAstatusBar(STATUS_PREPARING_FIND_INDEX+RsortMethodL);
     }
     Rkey = 'r';
   }
   /* 
    *  (w) - sort reverse/unreverse
    */
   if (Rkey == 'w') 
   {
     if (Rselected == PANEL_RIGHT) 
     {
       if (RsortReverseR) 
         RsortReverseR = 0;
       else 
         RsortReverseR = 1;
       RAstatusBar2(STATUS_REVERSE_SORTING_IN_X_PANEL, Rselected);
     }
     else 
     {
       if (RsortReverseL) 
         RsortReverseL = 0;
       else 
         RsortReverseL = 1;
       RAstatusBar2(STATUS_REVERSE_SORTING_IN_X_PANEL, Rselected);
     }
     Rkey = 'r';
   }
   /* 
    *  (t) - group/ungroup files and directories
    */
   if (Rkey == 't') 
   {
     if (Rselected == PANEL_RIGHT) 
     {
       if (RgroupFilesDirectoriesR) 
         RgroupFilesDirectoriesR = 0;
       else 
         RgroupFilesDirectoriesR = 1;
       RAstatusBar2(STATUS_UNGROUP_FILES_AND_DIR_IN_X_PANEL+RgroupFilesDirectoriesR, Rselected);
     }
     else 
     {
       if (RgroupFilesDirectoriesL) 
         RgroupFilesDirectoriesL = 0;
       else 
         RgroupFilesDirectoriesL = 1;
       RAstatusBar2(STATUS_UNGROUP_FILES_AND_DIR_IN_X_PANEL+RgroupFilesDirectoriesL, Rselected);
     }
     Rkey = 'r';
   }
   /* 
    *  (b) - bookmark
    */
   if (Rkey == 'b') 
   {
     RstrT = NULL;
     RchangePos = 0;
     if (!RdetectScreenToSmall()) 
     {
       if (Rselected == PANEL_RIGHT) 
       {
         RgoToRecordInTTlistMaster(&RlistTmainRight, RrightFirst+RrightPos, RlistRight);
         if (Rbookmark(&Rselected, RgetStrVariable(V_GROUP), &Rahost, &Radir, &RadirL, 
                       &(RlistTmainRight->col4),
                       RgetStrVariable(V_USERNAME), RgetStrVariable(V_PASSWORD), &RstrT))
           RchangePos = 1;
       }
       else 
       {
         RgoToRecordInTTlistMaster(&RlistTmainLeft, RleftFirst+RleftPos, RlistLeft);
         if (Rbookmark(&Rselected, RgetStrVariable(V_GROUP), &Rahost, &Radir, &RadirL, 
                       &(RlistTmainLeft->col4),
                       RgetStrVariable(V_USERNAME), RgetStrVariable(V_PASSWORD), &RstrT))
           RchangePos = 1;
       }
       if (RchangePos) 
       {
         RDBG("smbc main_Rbookmark : 1 RremoteLevel = "); RDBGi(RremoteLevel); RDBG("\n");
         if (Rselected == PANEL_LEFT) 
         {
           Rnr = RgetFileFromDirL(RadirL);
           if (Rnr != -1) 
           {
             RinitList(Rnr,0,0);
             RsetSelect(&RleftFirst, &RleftPos, RstrT , PANEL_LEFT);
           }
         }
         else 
         {
           if (Rstrlen(RgetStrVariable(V_GROUP)) > 0) 
             RremoteLevel = LEVEL_HOSTS;
           else 
             RremoteLevel = LEVEL_WORKGROUPS;
           if (Rstrlen(Rahost) > 0) 
           {
             RremoteLevel = LEVEL_SHARES;
             if (Rstrlen(Radir) > 0) 
               RremoteLevel = LEVEL_FILES;
           }

           switch (RremoteLevel)
           {
             case LEVEL_WORKGROUPS: Rnr = RgetMasterList(1);
                                    break;
             case LEVEL_HOSTS:      Rnr = RgetHostsFromGroup(RgetStrVariable(V_GROUP), 1);
                                    break;
             case LEVEL_SHARES:     Rnr = RgetSharesFromHost(Rahost, 1);
                                    break;
             case LEVEL_FILES:      Rnr = RgetFileFromDirR(Radir, 1);
                                    break;
             default:               Rnr = -1;
           }

           if (Rnr != -1) 
           {
             RinitList(Rnr, RremoteLevel, 1);
             RsetSelect(&RrightFirst, &RrightPos, RstrT , PANEL_RIGHT);
           }

         }
         Rfree(RstrT);
       }
     }
     RwinRedrawWithData();
   }
   /* 
    *  (r) - refresh
    */
   if (Rkey == 'r') 
     RrefreshR();
   /* 
    *  (a, insert) - select/unselect file or directory
    */
   if ((Rkey == 331) || (Rkey == 'a')) 
   {
     if (Rselected == PANEL_RIGHT) 
     {
       RgoToRecordInTTlistMaster(&RlistTmainRight, RrightFirst+RrightPos, RlistRight);
       if (!RstrCmp(RlistTmainRight->col2, "..")) 
       {
         if (RlistTmainRight->select) 
           RlistTmainRight->select = 0;
         else 
           RlistTmainRight->select = 1;
       }
     } 
     else 
     {
       RgoToRecordInTTlistMaster(&RlistTmainLeft, RleftFirst+RleftPos, RlistLeft);
       if (!RstrCmp(RlistTmainLeft->col2, "..")) 
       {
         if (RlistTmainLeft->select) 
           RlistTmainLeft->select = 0;
         else 
           RlistTmainLeft->select = 1;
       }
     }
     Rkey = 'o';
   }
   /* 
    *  (numeric keys *, shift+8) - select/unselect all files and directories
    */
   if ((Rkey == 106) || (Rkey == 42)) 
   {
     if (Rselected == PANEL_RIGHT) 
     {
       RinverseSelectAllRecords(RlistRight);
       RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
     } 
     else 
     {
       RinverseSelectAllRecords(RlistLeft);
       RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
     }
   }
   /* 
    *  (numeric shift -) - unselect all files and directories
    */
   if (Rkey == 95) 
   {
     if (Rselected == PANEL_RIGHT) 
     {
       RunselectAllRecords(RlistRight);
       RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
     } 
     else 
     {
       RunselectAllRecords(RlistLeft);
       RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
     }
   }
   /* 
    *  (numeric shift =) - select all files and directories
    */
   if (Rkey == 43) 
   {
     if (Rselected == PANEL_RIGHT) 
     {
       RselectAllRecords(RlistRight);
       RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
     } 
     else 
     {
       RselectAllRecords(RlistLeft);
       RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
     }
   }
   /* 
    *  (o) - go down one item
    */
   if (Rkey == KEY_DOWN || Rkey == 'o') 
   {
     if (Rselected == PANEL_LEFT) 
     {
       if (RleftPos+1 < RmainWinYlist) 
       {
         if (RleftPos+1 < RreturnCountTTlist(RlistLeft)) 
           RleftPos++;
       }
       else 
       { 
         if (RleftFirst+RleftPos+1 < RreturnCountTTlist(RlistLeft)) 
           RleftFirst++; 
       }
       RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
     }
     else 
     {
       if (RrightPos+1 < RmainWinYlist)
       {
         if (RrightPos+1 < RreturnCountTTlist(RlistRight)) 
           RrightPos++;
       }
       else 
       { 
         if (RrightFirst+RrightPos+1 < RreturnCountTTlist(RlistRight)) 
           RrightFirst++; 
       }
       RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
     }
   }
   /* 
    *  (p) - go up one item
    */
   if (Rkey == KEY_UP || Rkey == 'p') 
   {
     if (Rselected == PANEL_LEFT) 
     {
       if (RleftPos > 0) 
         RleftPos--;
       else 
       { 
         if (RleftFirst+RleftPos > 0) 
           RleftFirst--; 
       }
       RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
     }
     else 
     {
       if (RrightPos > 0) 
         RrightPos--;
       else 
       { 
         if (RrightFirst+RrightPos > 0) 
           RrightFirst--; 
       }
       RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
     }
   }
   /* 
    *  (u, END) - go at the bottom of the list
    */
   if (Rkey == KEY_END || Rkey == 'u') 
   {
     if (Rselected == PANEL_LEFT) 
     {
       if (RreturnCountTTlist(RlistLeft)+1 > RmainWinYlist)
       { 
         RleftFirst = RreturnCountTTlist(RlistLeft)-RmainWinYlist;
         RleftPos = RmainWinYlist-1; 
       }
       else 
         RleftPos = RreturnCountTTlist(RlistLeft)-1;
       RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
     }
     else 
     {
       if (RreturnCountTTlist(RlistRight)+1 > RmainWinYlist)
       { 
         RrightFirst = RreturnCountTTlist(RlistRight)-RmainWinYlist; 
         RrightPos = RmainWinYlist-1; 
       }
       else 
         RrightPos = RreturnCountTTlist(RlistRight)-1;
       RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
     }
   }
   /* 
    *  (u, HOME) - go on the top of the list
    */
   if (Rkey == KEY_HOME || Rkey == 'y') 
   {
     if (Rselected == PANEL_LEFT) 
     {
        if ((RleftFirst != 0) || (RleftPos != 0)) 
        {
          RleftFirst = 0;
          RleftPos = 0;
          RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
        }
     }
     else 
     {
       if ((RrightFirst != 0) || (RrightPos != 0)) 
       {
         RrightFirst = 0;
         RrightPos = 0;
         RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
       }
     }
   }
   /* 
    *  (l, PAGE_UP) - go on the top one screenn of the list
    */
   if (Rkey == KEY_PPAGE || Rkey == 'l') 
   {
     if (Rselected == PANEL_LEFT) 
     {
       if (RleftFirst-RmainWinYlist >= 0) 
         RleftFirst = RleftFirst-RmainWinYlist;
       else 
       { 
         RleftFirst = 0; 
         RleftPos = 0; 
       }
       RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
     }
     else 
     {
       if (RrightFirst-RmainWinYlist >= 0) 
         RrightFirst = RrightFirst-RmainWinYlist;
       else 
       { 
         RrightFirst = 0; 
         RrightPos = 0; 
       }
       RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
     }
   }
   /* 
    *  (k, PAGE_DOWN) - go at the bottom one screen of the list
    */
   if (Rkey == KEY_NPAGE || Rkey == 'k') 
   {
     if (Rselected == PANEL_LEFT) 
     {
       if (RleftFirst+RleftPos+1 < RreturnCountTTlist(RlistLeft)) 
       { 
         if (RreturnCountTTlist(RlistLeft) < RmainWinYlist) 
           RleftPos = RreturnCountTTlist(RlistLeft)-1;
         else 
         { 
           if (RleftFirst+RmainWinYlist+RmainWinYlist > RreturnCountTTlist(RlistLeft)) 
           { 
             RleftFirst = RreturnCountTTlist(RlistLeft)-RmainWinYlist;
             RleftPos = RmainWinYlist-1;
           }
           else 
             RleftFirst = RleftFirst+RmainWinYlist;
         }
       }
       RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
     }
     else 
     {
       if (RrightFirst+RrightPos+1 < RreturnCountTTlist(RlistRight)) 
       { 
         if (RreturnCountTTlist(RlistRight) < RmainWinYlist) 
           RrightPos = RreturnCountTTlist(RlistRight)-1;
         else 
         { 
           if (RrightFirst+RmainWinYlist+RmainWinYlist > RreturnCountTTlist(RlistRight)) 
           { 
             RrightFirst = RreturnCountTTlist(RlistRight)-RmainWinYlist;
             RrightPos = RmainWinYlist-1;
           }
           else 
             RrightFirst = RrightFirst+RmainWinYlist;
         }
       }
       RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
     }
   }
   /* 
    *  (i, TAB) - change active list
    */
   if (Rkey == 9 || Rkey == 'i') 
   {
     if (Rselected == PANEL_RIGHT) 
       Rselected = PANEL_LEFT;
     else 
       Rselected = PANEL_RIGHT;
     RwriteMenuTxt(RleftFirst, LEVEL_LOCAL, PANEL_LEFT, RleftPos, RlistLeft);
     RwriteMenuTxt(RrightFirst, RremoteLevel, PANEL_RIGHT, RrightPos, RlistRight);
   }
   /* 
    *  (d, F5) - download/upload
    */
   if (Rkey == 'd' || Rkey == 269) 
   {
    if (!RdetectScreenToSmall()) 
    { 
     Rwhat = 0;
     if (Rselected == PANEL_RIGHT) 
     {
       if (RexistAnySelect(RlistTmainRight))
       {
         if (RshowMessage(INFO_DO_YOU_WANT_COPY_X_TO_Y, Rm178, RadirL, NULL, DO_REDRAW_MAIN_WINDOW)) 
         {
           RlistTmainRight = RlistRight;
           while (RlistTmainRight)
           {
             if (RlistTmainRight->select) 
               RcreateFileFromDirR(RadirL, Radir, RlistTmainRight->col2, 
                                   RgetStrVariable(V_GROUP), Rahost, RlistTmainRight->col1[0]);
             RlistTmainRight = RlistTmainRight->next;
           }
           RlistTmainRight = RlistRight;
           RwinRedrawWithData();
           nodelay(RwinMain, 1);
           RactiveWindow = RwinMain;
         }
       } 
       else 
       {
         if (RremoteLevel == LEVEL_WORKGROUPS)
           RAstatusBar2(STATUS_WARNING_CANT_COPY_X_WORKGROUP, Rselected);
         else
         if (RremoteLevel == LEVEL_HOSTS)
           RAstatusBar2(STATUS_WARNING_CANT_COPY_X_HOST, Rselected);
         else
         if (RremoteLevel == LEVEL_SHARES)
           RAstatusBar(STATUS_WARNING_CANT_COPY_FROM_SHARE);
         else
         if (RremoteLevel == LEVEL_FILES)
         {
           RgoToRecordInTTlistMaster(&RlistTmainRight, RrightFirst+RrightPos, RlistRight);
           if (RstrCmp(RlistTmainRight->col2, "..") == 0) 
           {
             if (RshowMessage(INFO_DO_YOU_WANT_COPY_X_TO_Y, RlistTmainRight->col2, RadirL, NULL, DO_REDRAW_MAIN_WINDOW)) 
             {
               RcreateFileFromDirR(RadirL, Radir, RlistTmainRight->col2, 
                                   RgetStrVariable(V_GROUP), Rahost, (RlistTmainRight->col1)[0]);
               RwinRedrawWithData();
               nodelay(RwinMain, 1);
               RactiveWindow = RwinMain;
             }
           }
         }
       }
     }
     else 
     {
       if (RexistAnySelect(RlistLeft)) 
       {
         RstrT = RreturnFullSmbPath(FULLPATH_WITHOUT_SMB, RgetStrVariable(V_GROUP), Rahost, Radir, "");
         if (RshowMessage(INFO_DO_YOU_WANT_COPY_X_TO_Y, Rm178, RstrT, NULL, DO_REDRAW_MAIN_WINDOW)) 
         {
           RgoToFirstTTlist(&RlistTmainLeft);
           while (RlistTmainLeft)
           {
             if (RlistTmainLeft->select) 
               RcreateFileFromDirL(RadirL, Radir, RlistTmainLeft->col2, 
                                   RgetStrVariable(V_GROUP), Rahost, (RlistTmainLeft->col1)[0]);
             RlistTmainLeft = RlistTmainLeft->next;
           }
           RlistTmainLeft = RlistLeft;
           RwinRedrawWithData();
           nodelay(RwinMain,1);
           RactiveWindow = RwinMain;
         }
         Rfree(RstrT);
       }
       else 
       {
         if (RremoteLevel == LEVEL_WORKGROUPS)
           RAstatusBar(STATUS_WARNING_CANT_COPY_FROM_NETWORK);
         else
         if (RremoteLevel == LEVEL_HOSTS)
           RAstatusBar2(STATUS_WARNING_CANT_COPY_X_WORKGROUP, Rselected);
         else
         if (RremoteLevel == LEVEL_SHARES)
           RAstatusBar2(STATUS_WARNING_CANT_COPY_X_HOST, Rselected);
         else
         if (RremoteLevel == LEVEL_FILES)
         {
           RgoToRecordInTTlistMaster(&RlistTmainLeft, RleftFirst+RleftPos, RlistLeft);
           if (RstrCmp(RlistTmainLeft->col2,"..") == 0) 
           {
             RstrT = RreturnFullSmbPath(FULLPATH_WITHOUT_SMB, RgetStrVariable(V_GROUP),
                                        Rahost, Radir, "");
             if (RshowMessage(INFO_DO_YOU_WANT_COPY_X_TO_Y, RlistTmainLeft->col2, RstrT, NULL, DO_REDRAW_MAIN_WINDOW)) 
             {
               RcreateFileFromDirL(RadirL, Radir, RlistTmainLeft->col2, 
                                   RgetStrVariable(V_GROUP), Rahost, (RlistTmainLeft->col1)[0]);
               RwinRedrawWithData();
               nodelay(RwinMain,1);
               RactiveWindow = RwinMain;
             }
             Rfree(RstrT);
           }
         }
      }
     }
    } 
    else 
      RwinRedrawWithData();
   } 
   /* 
    *  (s) - show download/upload list
    */
   if (Rkey == 's') 
   {
     sleep(0);
     if (!RdetectScreenToSmall()) 
     {
       if (!RgetdownloadListCount(RdownloadList)) 
         RshowMessage(INFO_DOWNLOAD_LIST_IS_EMPTY, "", "", NULL, DO_REDRAW_MAIN_WINDOW);
       else 
         RmanageList();
       nodelay(RwinMain, 1);
       RactiveWindow = RwinMain;
     }
     else
       RwinRedrawWithData();
     alarm(RCalarm);
   }
   /* 
    *  (q) - quit
    */
   if (Rkey == 'q') 
   {
     alarm(0);
     if (Raskquit?RshowMessage(INFO_DO_YOU_WANT_QUIT_FROM_SMBC, "a", NULL, NULL, DO_REDRAW_MAIN_WINDOW):1) 
     {
       break;
     }
   }
   /* 
    *  (h) - help
    */
   if (Rkey == 'h') 
   {
     if (!RdetectScreenToSmall()) 
       RshowMessage(INFO_MINI_HELP, "", "", NULL, DO_REDRAW_MAIN_WINDOW);
     RwinRedrawWithData();
   }
   if (RindexTime != -1) 
     RcheckNewIndexFile(RfileNameMessage);
   
   RdoWindowResize();
   RtryDownloadUpload();
   Rkey = 0;
 };
 RDBG("----stop 1----\n");
 RfreeMasterList(); 
 RfreeSharesList(); 
 RfreeHostsList(); 
 RfreeFilesList(&RfilesR); 
 RfreeFilesList(&RfilesL); 
 Rfree(RfileNameForFindList);
 Rfree(RfileNameForFindListPrim);
 Rfree(RfileNameMessage);
 Rfree(RfileNameMessagePrim);
 Rfree(RsmbcLockFile);
 RDBG("----stop 2----\n");
 RsigTerm();
 RfreeDebug();
 return(0);
}
