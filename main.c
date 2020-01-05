/*****************************************************************************
 *              MULTI-TOP BULLETIN-CREATOR FOR F.A.M.E. SYSTEMS              *
 *                                                                           *
 *              wRITTEN bY SieGeL (tRSi/F-iNNOVATiON) 1995-2000              *
 *****************************************************************************/

#include <proto/fame.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <pragmas/locale_pragmas.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/rdargs.h>
#include <FAME/FAMEDoorProto.h>
#include <stdlib.h>
#include <strings.h>
#include <clib/utility_protos.h>
#include "proto.h"
#include "global_defines.h"
#include "struct.h"

extern struct	 ExecBase *SysBase;
extern char *_ProgramName;

void 	wb(char *);
void 	main(void);
void  FileCenter(char *,BPTR);
void	myquicksort(long,long,long);
VOID __stdargs MySPrintf(STRPTR buffer, char *formatString,...);

struct	FAMEUser *tempbuf=NULL;
struct	FAMEUser zwi;

LONG __stack = 32768L;

STATIC STRPTR Ver="\0$VER: Multi-Top "TOP_VER" (06.02.2000) "CPU_TYPE" by Sascha 'SieGeL' Pfalz\0";

/*
 *  Main Entry Point
 */

void main(void)
	{
 	struct	Liste *my=Fib;
	long		count=NULL;
	int			i=0;

	Fib=NULL;Fib2=NULL;
  if(SysBase->LibNode.lib_Version<39)
		{
		Printf("\n\n%s requires Kickstart V3.x to run !\n\n",FilePart(_ProgramName));
		return;
		}
	if(!(UtilityBase=OpenLibrary("utility.library",37))) { Print("\n\nUnable to open utility.library !"); SetIoErr(ERROR_INVALID_RESIDENT_LIBRARY); return;};
	if(!(FAMEBase=(struct FAMELibrary *)OpenLibrary(FAMENAME,5))) { SetIoErr(ERROR_INVALID_RESIDENT_LIBRARY); Printf("\n\nCan't open %s V5+, where's FAME ?\n\n",FAMENAME);CloseLibrary(UtilityBase);return;};
	LocaleBase=OpenLibrary("locale.library",38L);
	if (LocaleBase) myloc=OpenLocale(NULL);
	Printf("\n[32m[8CMulti-Top V%s [31m- Bulletin-Creator by SieGeL (tRSi-iNNOVATiONs)\n\nReading User.data...",TOP_VER);
	Flush(Output());
  if(!(mt_pool=CreatePool(MEMF_CLEAR|MEMF_PUBLIC,524288L,524288L))) wb("Unable to create Memory-Pool!\n\n");
	if(!(mp=AllocPooled(mt_pool,sizeof(struct MyPrefs)))) wb("Unable to allocate Prefs-Structure!\n\n");
	dosbuf1=AllocPooled(mt_pool,DOS_BUF_SIZE);
	dosbuf2=AllocPooled(mt_pool,DOS_BUF_SIZE);
	globds=AllocPooled(mt_pool,sizeof(struct DateStamp));
	globdt=AllocPooled(mt_pool,sizeof(struct DateTime));
	if(!dosbuf1||!dosbuf2) wb("Unable to allocate DOS Buffers !\n\n");
	if(ReadConfig()==FALSE) wb("\nERROR: Unable to find/open Config-File (Multi-Top.cfg) !\n");
	if(ReadAllDatas()==FALSE) { Printf("\nERROR: '%s' is not a valid FAME user.data File !\n",mp->pathes[USERDATA]);wb("");}
	Print("[32mOK ![m\n\n");
	if(*mp->pathes[ULISTER]) CreateUserList();
  if(mp->maxusers>usercounter) mp->maxusers=usercounter;
  my=Fib;
	if(!(list=AllocPooled(mt_pool,myuser*usercounter+1))) wb("No memory left for user.data Array!\n\n");
	while(count<usercounter)
		{
		CopyMemQuick(my,&list[count],myuser);
		count++;
		my=my->next;
		if(!my) break;
		}
	free_elements(Fib);Fib=NULL;
  for(i=2;i<BULL_ENTRIES;i++)
		{
		tempbuf=list;
		if(*mp->pathes[i])
			{
			Printf("Creating %s[m...",TypeOfMode[i]);
			Flush(Output());
			myquicksort(0,usercounter-1,i);
			CreateBull(i);
			};
		}
	wb("All done !\n");
	}

/*
 *  Modified Quicksort for Integer Arrays
 *
 *  V0.60: Added support for 64Bit Integer sorting
 */

void myquicksort(long start,long end,long which)
	{
	ULONG			num,num64[2];
	register 	long	i,j,s;

	i=start;
	j=end;
	s=(start+end)/2;
  switch(which)
		{
		case	UPFILES:		num=(ULONG)tempbuf[s].Uploads;
											break;
		case	DLFILES:  	num=(ULONG)tempbuf[s].Downloads;
											break;
		case	UPBYTES:		num64[HIGH]=tempbuf[s].BytesUpHi;
											num64[LOW]=tempbuf[s].BytesUpload;
											break;
		case	DLBYTES:		num64[HIGH]=tempbuf[s].BytesDownHi;
											num64[LOW]=tempbuf[s].BytesDownload;
											break;
		case	UPCPS:			num=(ULONG)tempbuf[s].HighCPSUP;
											break;
		case	DLCPS:			num=(ULONG)tempbuf[s].HighCPSDown;
											break;
		case	CALLERS:  	num=(ULONG)tempbuf[s].Calls;
											break;
		case	MESSIS:			num=(ULONG)tempbuf[s].MessageWrite;
                    	break;
		case	N_BYTES:		num=(ULONG)tempbuf[s].BytesNuked;
                    	break;
		case 	N_FILES:		num=(ULONG)tempbuf[s].FilesNuked;
											break;
		case  TOP_CHAT:		num=(ULONG)tempbuf[s].NumberOfChats;
                    	break;
		case	TOP_HACKS:  num=(long)tempbuf[s].AllHacks;
											break;
		}
	do
		{
		switch(which)
			{
			case	UPFILES:		while ((ULONG)tempbuf[i].Uploads < num) i++;
												while ((ULONG)tempbuf[j].Uploads > num) j--;
												break;
			case	DLFILES:		while ((ULONG)tempbuf[i].Downloads < num) i++;
												while ((ULONG)tempbuf[j].Downloads > num) j--;
												break;
			case	UPBYTES:		while(FAMENum64Comp(tempbuf[i].BytesUpHi,tempbuf[i].BytesUpload,num64[HIGH],num64[LOW])==-1) i++;
												while(FAMENum64Comp(tempbuf[j].BytesUpHi,tempbuf[j].BytesUpload,num64[HIGH],num64[LOW])==1) j--;
                      	break;
			case	DLBYTES:		while(FAMENum64Comp(tempbuf[i].BytesDownHi,tempbuf[i].BytesDownload,num64[HIGH],num64[LOW])==-1) i++;
												while(FAMENum64Comp(tempbuf[j].BytesDownHi,tempbuf[j].BytesDownload,num64[HIGH],num64[LOW])==1) j--;
                      	break;
			case	UPCPS:			while ((ULONG)tempbuf[i].HighCPSUP < num) i++;
												while ((ULONG)tempbuf[j].HighCPSUP > num) j--;
												break;
			case	DLCPS:			while ((ULONG)tempbuf[i].HighCPSDown < num) i++;
												while ((ULONG)tempbuf[j].HighCPSDown > num) j--;
												break;
			case	CALLERS:		while ((ULONG)tempbuf[i].Calls < num) i++;
												while ((ULONG)tempbuf[j].Calls > num) j--;
												break;
			case	MESSIS:			while ((ULONG)tempbuf[i].MessageWrite < num) i++;
												while ((ULONG)tempbuf[j].MessageWrite > num) j--;
												break;
			case	N_BYTES:		while ((ULONG)tempbuf[i].BytesNuked < num) i++;
                      	while ((ULONG)tempbuf[j].BytesNuked > num) j--;
                      	break;
			case  N_FILES:  	while ((ULONG)tempbuf[i].FilesNuked < num) i++;
                      	while ((ULONG)tempbuf[j].FilesNuked > num) j--;
                      	break;
			case	TOP_CHAT:		while ((ULONG)tempbuf[i].NumberOfChats < num) i++;
                      	while ((ULONG)tempbuf[j].NumberOfChats > num) j--;
												break;
			case	TOP_HACKS:	while ((LONG)tempbuf[i].AllHacks < num) i++;
                      	while ((LONG)tempbuf[j].AllHacks > num) j--;
												break;
			}
		if (i <= j)
			{
			zwi	=tempbuf[i];
			tempbuf[i]	=tempbuf[j];
			tempbuf[j] = zwi;
			i++;
			j--;
			}
		}
	while (i <= j);
	if (start < j) myquicksort(start, j,which);
	if (end   > i) myquicksort(i,end,which);
	}

/*
 *  Exits mULTI-tOP
 */

void wb(char *fehler)
	{
  if(*fehler) 		Print(fehler);
  if(LocaleBase)
		{
		if(myloc) CloseLocale(myloc);myloc=NULL;
		CloseLibrary(LocaleBase);LocaleBase=NULL;
		};
	if(UtilityBase) CloseLibrary(UtilityBase);UtilityBase=NULL;
	if(FAMEBase) 		CloseLibrary((struct Library *)FAMEBase);FAMEBase=NULL;
	if(mt_pool)			DeletePool(mt_pool);mt_pool=NULL;
	exit(0);
	}

void FileCenter(char *stri,BPTR wohin)
	{
	char 	r[256];

	*r='\0';
  strcpy(r,stri);
	FAMECutANSI(r,FCAF_STYLECMDS);
	FPrintf(wohin,"\r[%ldC%s\n",((78-strlen(r))/2),stri);
	}

/*
 *  RawDoFmt() Call without Assembly code
 */

VOID __stdargs MySPrintf(STRPTR buffer, char *ctl, ...)
	{
	RawDoFmt(ctl, (long *)(&ctl + 1), (void (*))"\x16\xc0\x4e\x75",buffer);
	}
