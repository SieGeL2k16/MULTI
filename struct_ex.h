/*
 *  Global defines for mULTI-tOP (external)
 */

#include "global_defines.h"

/*
 *  Memory Pool Pointer
 */

extern APTR mt_pool,dosbuf1,dosbuf2;

/*
 *  System structs used
 */

extern struct FAMELibrary 	*FAMEBase;
extern struct Library 			*UtilityBase;
extern struct	Locale				*myloc;

extern struct	DateStamp			*globds;
extern struct	DateTime			*globdt;


/*
 *  Preferences Struct
 */

extern struct MyPrefs
	{
	char  pathes[MAX_ENTRIES][108];
	long  maxusers;
	};

extern struct	MyPrefs *mp;

extern struct Totals
	{
	ULONG ulfiles,
				dlfiles,
				nbytes,
				nfiles,
				calls,
				messages;
	}totals;

extern struct Liste
	{
	struct FAMEUser fib;
	struct Liste *next;
	}*Fib,*Fib2;

extern struct FAMEUser *fib;

extern struct FAMEUser *list;    // Fuer die Zahlen-Sortierungen

extern long myuser;
extern long usercounter;
extern char	*TypeOfMode[BULL_ENTRIES];

extern char	UFILES[32],UBYTES[32],DFILES[32],DBYTES[32],NBYTES[32],NFILES[32];

extern BOOL AllowSysop;										// V1.2 -> True shows also SysOp in Stats
extern char extpath[256];									// V1.3 -> Contains path to external design files or NULL (internal design)
