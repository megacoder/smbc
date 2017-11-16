/*
       bookmarks.h
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
  
    last update: 2004-05-26
*/

#include "../config.h"
#include "consts.h"

#define TTBOOKMARKDIRLEN 2000
#define TTBOOKMARKFILELEN 1000

typedef struct Tbookmark { 
  char username[RMAXGHLEN];
  char password[RMAXGHLEN];
  char group[RMAXGHLEN];
  char host[RMAXGHLEN];
  char dir[TTBOOKMARKDIRLEN];
  char file[TTBOOKMARKFILELEN];
  char type;
  char del;
  struct Tbookmark *next;
  struct Tbookmark *previous;
} TTbookmark; 


void RgotoFirstBookmark(TTbookmark **Rbk);
void RgotoLastBookmark(TTbookmark **Rbk);
int RgotoXBookmark(TTbookmark **Rbk, int Rnr);
void RfreeBookmark(TTbookmark *Rbk);
int RfindInBookmark(TTbookmark **Rbk, int Riselect, char *Riagroup, char *Riahost, char *Riadir, char *Rifile, char *Riusername, char *Ripassword);
void RaddToBookmark(TTbookmark **Rbk, int Riselect, char *Riagroup, char *Riahost, char *Riadir, char *Rifile, char *Riusername, char *Ripassword);
void RopenBookmark(TTbookmark **Rbk, char *RfileName);
void RsaveBookmark(TTbookmark *Rbk, char *RfileName);
unsigned int RgetBookmarkCount(TTbookmark *Rbk);

