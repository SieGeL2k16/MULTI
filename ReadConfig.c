#include <exec/execbase.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/rdargs.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/fame.h>
#include <libraries/fame.h>
#include <stdlib.h>
#include <strings.h>
#include <clib/utility_protos.h>
#include <proto/utility.h>
#include "proto.h"
#include "global_defines.h"
#include "struct_ex.h"

BOOL ReadConfig(void);

/*
 *  Reads in the whole configuration (PROGDIR:Multi-Top.cfg), parses all strings and builts the neccessary
 *  configuration out of it.
 */

BOOL ReadConfig(void)
	{
	BPTR	myconfig=NULL;
	register long	i=0;
	char	mystring[256];

	if(!(myconfig=Open("PROGDIR:Multi-Top.cfg",MODE_OLDFILE))) return(FALSE);
	while(i<MAX_ENTRIES)
		{
		if(!(FGets(myconfig,mp->pathes[i],255))) break;
  	mp->pathes[i][(strlen(mp->pathes[i])-1)]='\0';
		if(*mp->pathes[i]=='-') *mp->pathes[i]='\0';
		i++;
		};
	if(FGets(myconfig,mystring,255)) StrToLong(mystring,&mp->maxusers);
	else mp->maxusers=0L;
	if(FGets(myconfig,mystring,255))
		{
    if(!Strnicmp("SHOWSYSOP",mystring,9)) AllowSysop=TRUE;
		else AllowSysop=FALSE;
		}
	if(FGets(myconfig,mystring,255)) strcpy(extpath,mystring);						// New for V1.3
	else *extpath='\0';
	Close(myconfig);
  if(i!=MAX_ENTRIES||!mp->maxusers) return(FALSE);
  else return(TRUE);
	}
