/*
 *  Reads user.data/user.keys
 */

#include <strings.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <dos/stdio.h>
#include <exec/exec.h>
#include <proto/exec.h>
#include <ctype.h>
#include <clib/utility_protos.h>
#include <proto/utility.h>
#include "struct_ex.h"
#include "global_defines.h"
#include "proto.h"

void 					free_elements (struct Liste *l);
struct Liste 	*quicksort (struct Liste *l);
struct Liste 	*merge (struct Liste *l1,struct Liste *l2);
int 					sorting (struct Liste *l1,struct Liste *l2);
BOOL					ReadAllDatas(void);
BOOL 					AddToList(void);

struct AllUlBytes
	{
  ULONG	BytesHi;
	ULONG BytesLo;
	}allulbytes;

struct AllDlBytes
	{
	ULONG BytesHi;
	ULONG BytesLo;
	}alldlbytes;

/*
 *  This Function reads user datas and calculates all required values
 *  V1.2: Added SetVBuf() to speed up reading of user.data
 */

BOOL ReadAllDatas(void)
	{
	char		Versionstring[66];
	BPTR  	udata=NULL;
	struct	FileInfoBlock __aligned myfib;
  BOOL		HowItWorks=FALSE;
	register LONG	maxusers,counta=NULL;

	Fib=NULL;Fib2=NULL;
	FAMEMemSet(&totals,'\0', sizeof(struct Totals));
  FAMEMemSet(&alldlbytes,'\0', sizeof(struct AllDlBytes));
	FAMEMemSet(&allulbytes,'\0', sizeof(struct AllUlBytes));
	if(!(udata=Open(mp->pathes[USERDATA],MODE_OLDFILE))) return(FALSE);
	SetVBuf(udata,NULL,BUF_FULL,4096);
	if(!(fib=AllocPooled(mt_pool,myuser))) { Close(udata);return(FALSE);};
	ExamineFH(udata,&myfib);
	if(myfib.fib_Size)
		{
		if(!(Read(udata,Versionstring,61)))
			{
			Close(udata);
			FreePooled(mt_pool,fib,myuser);
			return(FALSE);
			};
    if(!(Strnicmp("FAMEUSDA$VER:",Versionstring,13)))
			{
      maxusers=(myfib.fib_Size-61)/myuser;
  		while(counta<maxusers)
				{
				if(Read(udata,fib,myuser)<=0) break;
				if(!fib->Deleted_Or_Not)
					{
					if(fib->UserNumber==1 && AllowSysop==FALSE)
						{
						counta++;
						continue;
						}
					if(AddToList()==FALSE)
						{
						Close(udata);udata=NULL;
						FreePooled(mt_pool,fib,myuser);fib=NULL;
						return(FALSE);
						};
					FAMEAdd64(fib->BytesUpHi	, fib->BytesUpload, 	&allulbytes);
					FAMEAdd64(fib->BytesDownHi,	fib->BytesDownload, &alldlbytes);
					totals.ulfiles+=fib->Uploads;
					totals.dlfiles+=fib->Downloads;
     	    totals.nfiles+=fib->FilesNuked;
					totals.nbytes+=fib->BytesNuked;
					totals.calls+=fib->Calls;
					totals.messages+=fib->MessageWrite;
					counta++;
					}
				}
			Fib=quicksort(Fib);
			HowItWorks=TRUE;
			}
		else HowItWorks=FALSE;
		}
	Close(udata);udata=NULL;
	FreePooled(mt_pool,fib,myuser);fib=NULL;
	FAMENum64ToStr(allulbytes.BytesHi,allulbytes.BytesLo,FNSF_GROUPING|FNSF_NUMLOCALE,29,UBYTES);
	FAMENum64ToStr(alldlbytes.BytesHi,alldlbytes.BytesLo,FNSF_GROUPING|FNSF_NUMLOCALE,29,DBYTES);
  FAMENumToStr(totals.ulfiles,FNSF_GROUPING|FNSF_NUMLOCALE,29,UFILES);
	FAMENumToStr(totals.dlfiles,FNSF_GROUPING|FNSF_NUMLOCALE,29,DFILES);
	FAMENumToStr(totals.nbytes,FNSF_GROUPING|FNSF_NUMLOCALE,29,NBYTES);
	FAMENumToStr(totals.nfiles,FNSF_GROUPING|FNSF_NUMLOCALE,29,NFILES);
	return(HowItWorks);
	}

/*
 *  Spy's Listsort functions
 */

int sorting (struct Liste *l1,struct Liste *l2)
	{
	USHORT i,erg;

  if (!l1)  return(0);
  if (!l2)  return(1);
  i=0;
  erg=2;
  do
		{
		if (ToLower (l1->fib.UserName [i]) < ToLower (l2->fib.UserName [i])) erg = 1 ;
    if (ToLower (l1->fib.UserName [i]) > ToLower (l2->fib.UserName [i])) erg = 0 ;
    if (l1->fib.UserName [i] == '\0')	erg = 1 ;
    if (l2->fib.UserName [i] == '\0')	erg = 0 ;
    i++;
  	}while(erg==2);
  return (erg) ;
	}

struct Liste *merge (struct Liste *l1,struct Liste *l2)
	{
	struct Liste *l,*l_end ;

  l_end = NULL ;
  do
		{
		if (sorting (l1,l2))
    	{
			if (l_end == NULL)
      	{
				l_end = l1 ;
        l = l_end ;
      	}
      else
      	{
				l->next = l1 ;
        l = l->next ;
      	}
      l1 = l1->next ;
    	}
    else
    	{
			if (l_end == NULL)
      	{
				l_end = l2 ;
        l = l_end ;
      	}
      else
      	{
				l->next = l2 ;
        l = l->next ;
      	}
      l2 = l2->next ;
    	}
  	} while ((l1 != NULL) || (l2 != NULL)) ;
  l->next = NULL ;
  return (l_end) ;
	}

struct Liste *quicksort (struct Liste *l)
	{
	struct Liste *l_help1,*l_help2 ;
	int i,j ;
  if (!l)  return (l) ;
  if (!l->next) return (l) ;
  else
  	{
		i = 1 ;
    l_help1 = l ;
    while (l_help1)
    	{
			l_help1 = l_help1->next ;
      i++ ;
    	}
    l_help1 = l ;
    for (j = 1 ; j < ((i/2)-1) ; j++)	l_help1 = l_help1->next ;
    l_help2 = l_help1->next ;
    l_help1->next = NULL ;
    l_help1 = l ;
    l_help1 = quicksort (l_help1) ;
    l_help2 = quicksort (l_help2) ;
    return (merge (l_help1,l_help2)) ;
  	}
	}

BOOL AddToList(void)
	{
	if(!Fib)
		{
		if(!(Fib=(struct Liste *)AllocPooled(mt_pool,sizeof(struct Liste)))) return(FALSE);
		Fib2=Fib;
		}
	else
		{
		if(!(Fib2->next=(struct Liste *)AllocPooled(mt_pool,sizeof(struct Liste)))) return(FALSE);
		Fib2=Fib2->next;
		}
	Fib2->next=NULL;
  CopyMemQuick((APTR) fib,(APTR) Fib2,myuser);
  usercounter++;
	return(TRUE);
	}

void free_elements (struct Liste *l)
	{
	struct Liste *l_help1,*l_help2 ;
  l_help1 = l ;
  while(l_help1)
  	{
		l_help2 = l_help1 ;
    l_help1 = l_help1->next ;
    FreePooled(mt_pool,l_help2,sizeof(struct Liste));
  	}
	Fib=NULL;Fib2=NULL;
  }
