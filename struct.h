/*
 *  Global defines for mULTI-tOP
 */

#include "global_Defines.h"

/*
 *  Memory Pool Pointer
 */

APTR mt_pool=NULL,dosbuf1=NULL,dosbuf2=NULL;

/*
 *  System structs used
 */

struct  FAMELibrary 	*FAMEBase=NULL;
struct 	Library 			*UtilityBase=NULL;
struct	DateStamp			*globds=NULL;
struct	DateTime			*globdt=NULL;
struct 	Library				*LocaleBase=NULL;
struct	Locale				*myloc=NULL;

/*
 *  Preferences Struct
 */

struct MyPrefs
	{
	char  pathes[MAX_ENTRIES][108];
	long  maxusers;
	};

struct MyPrefs *mp=NULL;

struct Totals
	{
	ULONG ulfiles,
				dlfiles,
				nbytes,
				nfiles,
				calls,
				messages;
	}totals;

struct Liste
	{
	struct FAMEUser fib;
	struct Liste *next;
	}*Fib,*Fib2;

struct FAMEUser *fib;

struct FAMEUser *list=NULL;                 // Fuer die Zahlen-Sortierungen
long	myuser=sizeof(struct FAMEUser);
long	usercounter=NULL;

char	*TypeOfMode[BULL_ENTRIES]="uSER.dATA",
																"uSER-lIST",
																"uPLOADER [m([34mbYTES[m)",
																"dOWNLOADER [m([34mbYTES[m)",
																"uPLOADER [m([34mfILES[m)",
																"dOWNLOADER [m([34mfILES[m)",
																"uPLOADER [m([34mCPS[m)",
																"dOWNLOADER [m([34mCPS[m)",
					                      "aCTIVE cALLERS",
																"mESSAGE wRITERS",
																"nUKED bYTES",
																"nUKED fILES",
																"sYSOP cHATTERS",
																"oVERALL hACKS";

char	UFILES[32],UBYTES[32],DFILES[32],DBYTES[32],NBYTES[32],NFILES[32];

BOOL AllowSysop=FALSE;										// V1.2 -> True shows also SysOp in Stats

char extpath[256];												// V1.3 -> Contains path to external design files or NULL (internal design)
