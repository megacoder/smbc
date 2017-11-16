/*
       find-index-manage.h
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/

#include <curses.h>
#include "consts.h"
#include "rtypedef.h"

int RmakeCopyFileWithPID(char *Rdst, char *Rsrc);

int RindexFileUnlink(char *RpathToFile);
int RindexFileOpen(char *RpathToFile, int *RfileDesc);
int RindexFileCreate(char *RpathToFile, int *RfileDesc);
void RindexFileClose(int *RfileDesc);

int RindexFileReadOneRecord(int RfileDesc, TTfind **TfindRecord);
int RindexFileWriteOneRecord(int RfileDesc, TTfind *TfindRecord);
int RindexFileSetSeek(int RfileDesc, long int Rnr);
int RindexFileExistAnyRecords(int RfileDesc);
int RindexFileGetCount(int RfileDesc);
int RindexFileTruncate(int RfileDesc, long int Rnr);
int RindexFileReadCounter(int RfileDesc);

//-----------------------------------------------------------------

int RcreateNewRecordInIndexTable(TTindexTable **Ridx, int Rnr);
int RfreeAllRecordFromIndexTable(TTindexTable **Ridx);

int RgotoFirstIndexTable(TTindexTable **Ridx);
int RgotoNIndextTable(TTindexTable **Ridx, int Rnr);
int RgetCountRecordIndexTable(TTindexTable *Ridx);
int RgotoNextIndexTable(TTindexTable **Ridx);

//-----------------------------------------------------------------

int RloadIndexFile(int Rfd, TTfind **RidxNet, int count, WINDOW *Rwin);
void RfreeIndexFile(TTfind **RidxNet);

int RindexNetSetSeek(TTfind **RidxNet, int Rnr);
int RindexNetReadOneRecord(TTfind **RidxNet, TTfind **RidxNetDst);
int RindexNetGoToNext(TTfind **RidxNet);
void RindexFreeOneTTfind(TTfind **RidxNet);

//-----------------------------------------------------------------

int RAddHostToHostsTable(TThostsTable **RhostsTable, char *Rstr);
int RFreeHostsTable(TThostsTable **RhostsTable);
int RFindHostInToHostsTable(TThostsTable *RhostsTable, char *Rstr);

