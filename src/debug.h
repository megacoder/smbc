/*
       debug.h
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/

#include "../config.h"

void RinitDebugm(char *Rstr);
void RfreeDebugm();
void RDBGm(char *Rstr);
int RDBGim(int Ri);
void RDBGcm(char Rc);

#ifdef HAVE_DEBUG

#define RinitDebug(Rstr) RinitDebugm(Rstr)
#define RfreeDebug() RfreeDebugm()
#define RDBG(Rstr) RDBGm(Rstr)
#define RDBGi(Ri) RDBGim(Ri)
#define RDBGc(Rc) RDBGcm(Rc)

#else /* not HAVE_DEBUG */

#define RinitDebug(Rstr)
#define RfreeDebug()
#define RDBG(Rstr)
#define RDBGi(Ri)
#define RDBGc(Rc)

#endif /* not HAVE_DEBUG */

