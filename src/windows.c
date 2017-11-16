/*
       windows.c
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
#include <stdlib.h>
#include "debug.h"
#include "static_str.h"
#include "windows.h"
#include "fnet.h"

#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

char *RstatusBarStrP = NULL;

void
RstatusBarStr(char *Rstr, WINDOW *RwinStatus, int Rx, int Rnr)
{
  RDBG("smbc : RstatusBarStr start\n");
  RstatusBarStrP = Rstrdup(Rnr == 1?Rm54:"");
  Rstraddtoend(&RstatusBarStrP, Rstr);
  if (strlen(RstatusBarStrP) > Rx)
    RstatusBarStrP[Rx-1] = 0;
  RstatusBar(STATUS_X, RwinStatus, Rx, 0);
  Rfree(RstatusBarStrP);
  RDBG("smbc : RstatusBarStr end\n");
}

void
RstatusBar(int Rnr, WINDOW *RwinStatus, int Rx, int Rselected)
{
  RDBG("windows: RstatusBar RmainWinX Rx = "); RDBGi(Rx); RDBG("\n");
  mvwhline(RwinStatus, 0, 0, ' ', Rx);
  switch(Rnr) {
    case -1: break;
    case STATUS_ADDING_FILES_TO_UPLOAD_LIST:      Rmvwprintw(RwinStatus, 0,0, Rm60); break;
    case STATUS_ENTER_WORKGROUP_NAME:             Rmvwprintw(RwinStatus, 0,0, Rm61); break;
    case STATUS_ENTER_HOST_NAME:                  Rmvwprintw(RwinStatus, 0,0, Rm62); break;
    case STATUS_ENTER_DIR_NAME:                   Rmvwprintw(RwinStatus, 0,0, Rm63); break;
    case STATUS_ENTER_FILE_NAME:                  Rmvwprintw(RwinStatus, 0,0, Rm64); break;
    case STATUS_DEFAULT:                          Rmvwprintw(RwinStatus, 0,0, Rm65); break;
    case STATUS_PREPARING_FIND_INDEX:             Rmvwprintw(RwinStatus, 0,0, Rm67); break;
    case STATUS_SORT_X_PANEL_BY_FILENAME:         mvwprintw(RwinStatus, 0,0, Rm68, Rselected?Rright:Rleft); break;
    case STATUS_SORT_X_PANEL_BY_EXTENSION:        mvwprintw(RwinStatus, 0,0, Rm69, Rselected?Rright:Rleft); break;
    case STATUS_SORT_X_PANEL_BY_SIZE:             mvwprintw(RwinStatus, 0,0, Rm70, Rselected?Rright:Rleft); break;
    case STATUS_X_PANEL_WITHOUT_SORTING:          mvwprintw(RwinStatus, 0,0, Rm71, Rselected?Rright:Rleft); break;
    case STATUS_REVERSE_SORTING_IN_X_PANEL:       mvwprintw(RwinStatus, 0,0, Rm72, Rselected?Rright:Rleft); break;
    case STATUS_UNGROUP_FILES_AND_DIR_IN_X_PANEL: mvwprintw(RwinStatus, 0,0, Rm73, Rselected?Rright:Rleft); break;
    case STATUS_GROUP_FILES_AND_DIR_IN_X_PANEL:   mvwprintw(RwinStatus, 0,0, Rm74, Rselected?Rright:Rleft); break;
    case STATUS_WARNING_CANT_COPY_X_WORKGROUP:    mvwprintw(RwinStatus, 0,0, Rm75, Rselected?Rfrom:Rto); break;
    case STATUS_WARNING_CANT_COPY_X_HOST:         mvwprintw(RwinStatus, 0,0, Rm76, Rselected?Rfrom:Rto); break;
    case STATUS_WARNING_CANT_COPY_FROM_SHARE:     Rmvwprintw(RwinStatus, 0,0, Rm77); break;
    case STATUS_WARNING_CANT_COPY_FROM_NETWORK:   Rmvwprintw(RwinStatus, 0,0, Rm78); break;
    case STATUS_DEFAULT_FOR_VIEW_FILE:            Rmvwprintw(RwinStatus, 0,0, Rm79); break;
    case STATUS_INTERACTIVE_FILTERING_IS_OFF:     Rmvwprintw(RwinStatus, 0,0, Rm80); break;
    case STATUS_INTERACTIVE_FILTERING_IS_ON:      Rmvwprintw(RwinStatus, 0,0, Rm81); break;
    case STATUS_DEFAULT_FOR_BOOKMARK:             Rmvwprintw(RwinStatus, 0,0, Rm82); break;
    case STATUS_DEFAULT_FOR_BOOKMARK_MOVE:        Rmvwprintw(RwinStatus, 0,0, Rm83); break;
    case STATUS_DEFAULT_FOR_DULIST:               Rmvwprintw(RwinStatus, 0,0, Rm84); break;
    case STATUS_DEFAULT_FOR_DULIST_MOVE:          Rmvwprintw(RwinStatus, 0,0, Rm85); break;
    case STATUS_WORKGROUP_IS_EMPTY:               Rmvwprintw(RwinStatus, 0,0, Rm22); break;
    case STATUS_HOST_IS_EMPTY:                    Rmvwprintw(RwinStatus, 0,0, Rm19); break;
    case STATUS_SHARE_IS_EMPTY:                   Rmvwprintw(RwinStatus, 0,0, Rm20); break;
    case STATUS_DIR_IS_EMPTY:                     Rmvwprintw(RwinStatus, 0,0, Rm21); break;
    case STATUS_ACCESS_DENIED:                    Rmvwprintw(RwinStatus, 0,0, Rm45); break;
    case STATUS_WORKGROUPS_NOT_FOUND:             Rmvwprintw(RwinStatus, 0,0, Rm18); break;
    case STATUS_FOUNDED_WORKGROUPS:               Rmvwprintw(RwinStatus, 0,0, Rm4); break;
    case STATUS_FOUNDED_HOSTS:                    Rmvwprintw(RwinStatus, 0,0, Rm6); break;
    case STATUS_FOUNDED_SHARES:                   Rmvwprintw(RwinStatus, 0,0, Rm8); break;
    case STATUS_FOUNDED_DIRS:                     Rmvwprintw(RwinStatus, 0,0, Rm11); break;
    case STATUS_X:                                Rmvwprintw(RwinStatus, 0,0, RstatusBarStrP); break;
  }
  wrefresh(RwinStatus);
}
