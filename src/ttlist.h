/*
       ttlist.h
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.org
    home page: http://smbc.airm.net
*/
// -------------------------------------------------------------------
#define Rfree(ptr)    { free(ptr); ptr = NULL; } 

typedef 
struct Tlist {
  char *col1;
  char *col2;
  char *col3;
  char *col4;
  unsigned char select;
  int size;
  struct Tlist *next;
  struct Tlist *previous;
} TTlist;


void RgoToFirstTTlist(TTlist **Rlist);
TTlist *RreturnFirstTTlist(TTlist *Rlist);

void RgoToLastTTlist(TTlist **Rlist);
TTlist *RreturnLastTTlist(TTlist *Rlist);

void RgoToRecordInTTlist(TTlist **Rlist, int Ri);
void RgoToRecordInTTlistMaster(TTlist **Rlist, int Ri, TTlist *RlistMaster);

void RunselectAllRecords(TTlist *Rlist);
void RselectAllRecords(TTlist *Rlist);
void RinverseSelectAllRecords(TTlist *Rlist);
unsigned char RexistAnySelect(TTlist *Rlist);
void RrotateTwoRecords(TTlist *Rone, TTlist *Rtwo);

void RfreeAllInTTlist(TTlist **RlistI);
void RcreateNextOrNewTTlistAndAddData(TTlist **RlistI,
                                      char *col1I, char *col2I, char *col3I, char *col4I,
                                      int sizeI,
                                      unsigned char selectI);

int RreturnCountTTlist(TTlist *RlistI);
