/*
       rtypedef.h
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/
// -------------------------------------------------------------------

#include "consts.h"

#define RMAXDIRLEN 1000
#define TdownloadsizeDisplayRLen 6
#define TdownloadsizeDisplayDownR 6
#define TdownloadpercentLen 5 
#define TfullfilenameLen 2000
#define EXTLEN 11

typedef struct Tdownload {
  char *username;
  char *password;
  char *groupnameR;
  char *hostname;
  char *pathRLbase;
  char *pathR;
  char *pathL;
  char *fullfilename;
  char *filename;
  unsigned int sizeR;
  char *sizeDisplayR;
  char *sizeDisplayDownR;
  char *percent;
  char active;
  char upload; 
  time_t delayRetry;
  time_t nextRetry;
  struct Tdownload *next;
  struct Tdownload *previous;
} TTdownload;

typedef struct Tfind {
  char *group;
  char *host;
  char *dir;
  char *file;
  char *access_denied;
  struct Tfind *next;
  struct Tfind *previous;
} TTfind;

typedef struct TindexTable {
  int number;
  struct TindexTable *next;
  struct TindexTable *previous;
} TTindexTable;

typedef struct ThostsTable {
  char *name;
  struct ThostsTable *next;
} TThostsTable;

typedef
struct Tgroups {
  char *groupname;
  char *mastername;
  char *masterip;
  struct Tgroups *next;
  struct Tgroups *previous;
} TTgroups;

typedef 
struct Thosts {
  char *hostname;
  char *comment;
  char *ip;
  struct Thosts *next;
  struct Thosts *previous;
} TThosts;

typedef 
struct Tshares {
  char *sharename;
  char *comment;
  struct Tshares *next;
  struct Tshares *previous;
} TTshares;

typedef 
struct Tfiles {
  char *filename;
  int type;     
  char *sizeShow;
  int size;
  struct Tfiles *next;
  struct Tfiles *previous;
} TTfiles;

typedef
struct Tcolor {
  short red;
  short green;
  short blue;
  char realnumber;
} TTcolor;

typedef
struct Text {
  char ext[EXTLEN];
  int colornr;
  struct Text *Text;
} TText;

typedef
struct Tposition {
  int Pos;
  int First;
  struct Tposition *Tposition;
} TTposition;

