/*
 *  Creates the User List in alphabetical order
 */

#include <dos/dos.h>
#include <dos/stdio.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <exec/memory.h>
#include <stdlib.h>
#include <strings.h>
#include "proto.h"
#include "global_defines.h"
#include "struct_ex.h"

void CreateUserList(void)
	{
	struct	Liste *mylist;
	struct	DateTime *dt=NULL;
	struct	DateStamp *ds=NULL;
  BPTR		upath=NULL,myheader=NULL;
	char		str[256],datum[14];
	ULONG		count=1L,totalcalls=0L;
	ULONG 	UnixTimeOffset = 252482400,number;

	Printf("Creating %s[m...",TypeOfMode[ULISTER]);
	Flush(Output());
	dt=AllocPooled(mt_pool,sizeof(struct DateTime));
	ds=AllocPooled(mt_pool,sizeof(struct DateStamp));
	if(!dt||!ds) wb("ERROR!\n\nOut of memory!\n\n");
	if(!(upath=Open(mp->pathes[ULISTER],MODE_NEWFILE))) { Printf("ERROR !\n\n Unable to open %s !\n",mp->pathes[ULISTER]);return;};
	if(!(myheader=Open("PROGDIR:HEADER/USERLIST.TXT",MODE_OLDFILE)))
		{
		FPuts(upath,"\f\n");
		MySPrintf(str,"[m-+- [35muSER-LIST fROM [33m%s [m-+-",mp->pathes[BBSNAME]);
		FileCenter(str,upath);
		}
	else
		{
		*str='\0';
		SetVBuf(myheader,dosbuf1,BUF_FULL,DOS_BUF_SIZE);
		SetVBuf(upath,dosbuf2,BUF_FULL,DOS_BUF_SIZE);
		while(FGets(myheader,str,254)) FPuts(upath,str);
    Close(myheader);myheader=NULL;
		*str='\0';
		}
	FPuts(upath,"\n   [33mnR.       [36mnAME[m/[36mhANDLE            [35mlOCATION[m/[35mgROUP        [32mcALLS  [34mlAST cALL\n[m  ----^-----------------------^--------------------------^-----^-----------\n");
	mylist=Fib;
	while(mylist)
		{
		number=mylist->fib.LastTime;
		number-= UnixTimeOffset;
    dt->dat_Stamp.ds_Days=number/86400L;
    number -= dt->dat_Stamp.ds_Days*86400L;
    dt->dat_Stamp.ds_Minute=number/60L;
    dt->dat_Stamp.ds_Tick=0L;
 		dt->dat_Format=FORMAT_USA;
  	dt->dat_StrDate=datum;
  	DateToStr(dt);
		FPrintf(upath,"  [33m%4.4ld[m.[36m%-22.22s\r[31C[35m%-25.25s\r[58C[32m%5.5ld  [34m%s\n",count,mylist->fib.UserName,mylist->fib.UserLocation,mylist->fib.Calls,datum);
		totalcalls+=mylist->fib.Calls;
		mylist=mylist->next;
		count++;
    }
	FPuts(upath,"  [m----^-----------------------^--------------------------^-----^-----------\n");
	MySPrintf(str,"[36mtOTAL cALLS[33m: [32m%ld [34m- [35mmULTI-tOP [mV%s [36mby SieGeL [m([36mtRSi[m/[36mF[m-[36miNNOVATiON[m)",totalcalls,TOP_VER);
	FileCenter(str,upath);
	Close(upath);
	FreePooled(mt_pool,ds,sizeof(struct DateStamp));ds=NULL;
  FreePooled(mt_pool,dt,sizeof(struct DateTime));dt=NULL;
	Print("[32mOK ![m\n\n");
	}
