/*
       windows.h
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.org
    home page: http://smbc.airm.net
*/
// -------------------------------------------------------------------
#include <curses.h>

#define Rright Rm86
#define Rleft  Rm87
#define Rfrom  Rm88
#define Rto    Rm113

#define STATUS_ADDING_FILES_TO_UPLOAD_LIST 0
#define STATUS_ENTER_WORKGROUP_NAME 2
#define STATUS_ENTER_HOST_NAME 3
#define STATUS_ENTER_DIR_NAME 4
#define STATUS_ENTER_FILE_NAME 5
#define STATUS_DEFAULT 6
#define STATUS_PREPARING_FIND_INDEX 8
#define STATUS_SORT_X_PANEL_BY_FILENAME 9
#define STATUS_SORT_X_PANEL_BY_EXTENSION 10
#define STATUS_SORT_X_PANEL_BY_SIZE 11
#define STATUS_X_PANEL_WITHOUT_SORTING 12
#define STATUS_REVERSE_SORTING_IN_X_PANEL 13
#define STATUS_UNGROUP_FILES_AND_DIR_IN_X_PANEL 14
#define STATUS_GROUP_FILES_AND_DIR_IN_X_PANEL 15
#define STATUS_WARNING_CANT_COPY_X_WORKGROUP 16
#define STATUS_WARNING_CANT_COPY_X_HOST 17
#define STATUS_WARNING_CANT_COPY_FROM_SHARE 18
#define STATUS_WARNING_CANT_COPY_FROM_NETWORK 19
#define STATUS_DEFAULT_FOR_VIEW_FILE 20
#define STATUS_INTERACTIVE_FILTERING_IS_OFF 21
#define STATUS_INTERACTIVE_FILTERING_IS_ON 22
#define STATUS_DEFAULT_FOR_BOOKMARK 23
#define STATUS_DEFAULT_FOR_BOOKMARK_MOVE 24
#define STATUS_DEFAULT_FOR_DULIST 25
#define STATUS_DEFAULT_FOR_DULIST_MOVE 26
#define STATUS_WORKGROUP_IS_EMPTY 27
#define STATUS_HOST_IS_EMPTY 28
#define STATUS_SHARE_IS_EMPTY 29
#define STATUS_DIR_IS_EMPTY 30
#define STATUS_ACCESS_DENIED 31
#define STATUS_WORKGROUPS_NOT_FOUND 32
#define STATUS_FOUNDED_WORKGROUPS 33
#define STATUS_FOUNDED_HOSTS 34
#define STATUS_FOUNDED_SHARES 35
#define STATUS_FOUNDED_DIRS 36
#define STATUS_X 100

#define STATUS_TXT_ACCESS_DENIED Rm45

/*
 * function display Rstr in to status bar
 * Rstr must be in to_scr char code
*/

void RstatusBarStr(char *Rstr, WINDOW *RwinStatus, int Rnr, int Rx);
void RstatusBar(int Rnr, WINDOW *RwinStatus, int Rx, int Rselected);

