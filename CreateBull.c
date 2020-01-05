/**********************************************************
 *                                                        *
 *  CreateBull.c - Contains the creation of the bulletins *
 *                                                        *
 **********************************************************/

#include <exec/execbase.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/stdio.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/locale.h>
#include <proto/utility.h>
#include <stdlib.h>
#include <strings.h>
#include <clib/utility_protos.h>
#include <ctype.h>
#include "proto.h"
#include "global_defines.h"
#include "struct_ex.h"

/*
 *  Prototypes
 */

void CreateBull(long MODE); 													// Creates standard Design bull
long CreateExtBull(long MODE);												// Uses template to create bulletin (V1.3)
void CheckCommand(char *buf);													// Checks % commands (V1.3)
void MyCopyString(char *, char *, long, long,long);   // Special copy command, inserts one string into another
void UpdateStamp(void);

STATIC 	LONG __saveds __asm FormatDateHookFunc(register __a0 struct Hook *,register __a1 UBYTE Char);
BOOL 		__regargs FormatStamp(struct DateStamp *,STRPTR,STRPTR);

/*
 *  Global variables
 */

char	Datum[LEN_DATSTRING],Zeit[LEN_DATSTRING];

STATIC char *Lines[2]="[35m-[34m==[35m-[34m=======================[35m-[34m=======================[35m-[34m=========================[35m-\n",
							 				"[35m-[34m==[35m-[34m=============================[35m-[34m=============================[35m-[34m=============[35m-\n";

STATIC STRPTR headernames[]={	"","","UL_BYTES.TXT","DL_BYTES.TXT","UL_FILES.TXT","DL_FILES.TXT","UL_CPS.TXT","DL_CPS.TXT",
															"CALLERS.TXT","MESSAGES.TXT","NUKED_BYTES.TXT","NUKED_FILES.TXT","CHATS.TXT","HACKS.TXT"};


STATIC STRPTR templates[]={	"","","UL_BYTES.DSG","DL_BYTES.DSG","UL_FILES.DSG","DL_FILES.DSG","UL_CPS.DSG","DL_CPS.DSG",
															"CALLERS.DSG","MESSAGES.DSG","NUKED_BYTES.DSG","NUKED_FILES.DSG","CHATS.DSG","HACKS.DSG"};

/*
 *  CreateBull() creates the output either with an external header or the built-in version.
 *  V1.3 adds the possibility to use external design files, in this case the function calls CreateExtBull().
 */

void CreateBull(long MODE)
	{
	BPTR	fileptr=NULL,headerfp=NULL;
	char	headerstr[256],tbuf[40];
	ULONG	sortdigit,sort_digit[2],color=NULL;
	register long	ucount, counter;

	UpdateStamp();
  if(*extpath)
		{
		if(!(CreateExtBull(MODE))) return;						// New for V1.3
		}
	if(!(fileptr=Open(mp->pathes[MODE],MODE_NEWFILE))) { Print("[33mFAIL!!![m\n\n");return;};
	SetVBuf(fileptr,NULL,BUF_FULL,4096);
	*headerstr='\0';
	MySPrintf(headerstr,"PROGDIR:HEADER/%s",headernames[MODE]);
  if(!(headerfp=Open(headerstr,MODE_OLDFILE)))
		{
		FPuts(fileptr,"\f\n");
		MySPrintf(headerstr,"[36mtOP [33m%ld [36m%s [34m- [35mmULTI-tOP [mV%s [36mbY SieGeL [m([36mtRSi[m-[36miNNOVATiONs[m)\0",mp->maxusers,TypeOfMode[MODE],TOP_VER);
		FileCenter(headerstr,fileptr);
		}
	else
		{
   	*headerstr='\0';
		SetVBuf(headerfp,dosbuf1,BUF_FULL,DOS_BUF_SIZE);
		SetVBuf(fileptr,dosbuf2,BUF_FULL,DOS_BUF_SIZE);
		while(FGets(headerfp,headerstr,254)) FPuts(fileptr,headerstr);
		Close(headerfp);headerfp=NULL;
		MySPrintf(headerstr,"[35mmULTI-tOP [mV%s [36mbY SieGeL [m([36mtRSi[m-[36miNNOVATiONs[m)\0",TOP_VER);
		FileCenter(headerstr,fileptr);
		}
	switch(MODE)
		{
		case	DLBYTES:
		case  UPBYTES:
		case	N_BYTES:	FPuts(fileptr,Lines[0]);
										break;
 		default:				FPuts(fileptr,Lines[1]);
										break;
		}
	counter=usercounter-1;
	ucount=NULL;
	while(counter>-1)
		{
		switch(MODE)
			{
			case	UPBYTES:		sort_digit[HIGH]=list[counter].BytesUpHi;
												sort_digit[LOW]=list[counter].BytesUpload;
												break;
			case	DLBYTES:		sort_digit[HIGH]=list[counter].BytesDownHi;
												sort_digit[LOW]=list[counter].BytesDownload;
												break;
      case	UPFILES:		sortdigit=list[counter].Uploads;
												break;
			case	DLFILES:		sortdigit=list[counter].Downloads;
												break;
			case  UPCPS:    	sortdigit=list[counter].HighCPSUP;
												break;
      case	DLCPS:			sortdigit=list[counter].HighCPSDown;
												break;
      case	CALLERS:		sortdigit=list[counter].Calls;
												break;
			case	MESSIS:			sortdigit=list[counter].MessageWrite;
                      	break;
			case  N_BYTES:		sortdigit=list[counter].BytesNuked;
												break;
			case  N_FILES:		sortdigit=list[counter].FilesNuked;
												break;
			case	TOP_CHAT:		sortdigit=list[counter].NumberOfChats;
												break;
			case	TOP_HACKS:  sortdigit=list[counter].AllHacks;
												break;
			}
		if(ucount)
			{
			if(ucount%2) color=0;
			else color=36;
			}
		else color=33;
		switch(MODE)
			{
			case	DLBYTES:
			case	UPBYTES:
			case	N_BYTES:	if(MODE==N_BYTES) FAMENumToStr(sortdigit,FNSF_GROUPING|FNSF_NUMLOCALE,38,tbuf);
											else FAMENum64ToStr(sort_digit[HIGH],sort_digit[LOW],FNSF_GROUPING|FNSF_NUMLOCALE,38,tbuf);
											FPrintf(fileptr,"[%ldm %2.2ld.%-23.23s %-23.23s\r[50C%27.27s\n",color,ucount+1,list[counter].UserName,list[counter].UserLocation,tbuf);
 	                    break;

   	  default:        FAMENumToStr(sortdigit,FNSF_GROUPING|FNSF_NUMLOCALE,38,tbuf);
                      FPrintf(fileptr,"[%ldm %2.2ld.%-29.29s %-29.29s\r[63C%14.14s\n",color,ucount+1,list[counter].UserName,list[counter].UserLocation,tbuf);
											break;
			}
    counter--;ucount++;
		if(ucount>=mp->maxusers) break;
		}
	switch(MODE)
		{
		case	DLBYTES:
		case  UPBYTES:  FPuts(fileptr,Lines[0]);
										break;

    default:				FPuts(fileptr,Lines[1]);
										break;
		}
	switch(MODE)
		{
		case	N_BYTES:
    case  N_FILES:  MySPrintf(headerstr,"[36mtOTAL nUKES[33m: [32m%s [36mfILES wITH [32m%s [36mbYTES\0",NFILES,NBYTES);
										FileCenter(headerstr,fileptr);
   	                break;
		default:	  		MySPrintf(headerstr,"[36mtOTAL sYSTEM-UL'S[33m: [32m%9.9s [36mfILES [34m- [32m%27.27s [36mbYTES\0",UFILES,UBYTES);
										FileCenter(headerstr,fileptr);
										MySPrintf(headerstr,"[36mtOTAL sYSTEM-DL'S[33m: [32m%9.9s [36mfILES [34m- [32m%27.27s [36mbYTES\0",DFILES,DBYTES);
										FileCenter(headerstr,fileptr);
		}
	Close(fileptr);
	Print("[32mOK ![m\n\n");
	}

/*
 *  CreateExtBull() creates a given bulletin from a template stored in 'extpath' with filename out of array templates
 *  Should anything go wrong the function clean up and return a NON-ZERO value to allow the internal engine to output
 *
 *  Design Configuration is originally taken from Bobo/Mystic, which inspired me ;)
 *
 *
 *  %<WW>-<CMD>-<PP> =>  <WW> = Width of string, <CMD> = Command string, <PP> = Place
 *
 */

long CreateExtBull(long mymode)
	{
	BPTR	tptr = NULL,
				fileptr = NULL;
	char	buf[256],readbuf[1024];

  strcpy(buf,extpath);
	buf[strlen(buf)-1]='\0';
	AddPart(buf,templates[mymode],255);
  if(!(tptr=Open(buf,MODE_OLDFILE))) return(1);
	if(!(fileptr=Open(mp->pathes[mymode],MODE_NEWFILE)))
		{ 
		Print("[33mFAIL!!![m\n\n");
		return(0);
		};
	SetVBuf(fileptr,NULL,BUF_FULL,4096);
	SetVBuf(tptr,NULL,BUF_LINE,4096);
	while(FGets(tptr,readbuf,255))
		{
    if(FAMEStrChr(readbuf,'%'))
			{
			CheckCommand(readbuf);
			}
		FPuts(fileptr,readbuf);
		}
	Close(tptr);tptr=NULL;
	Close(fileptr);fileptr=NULL;
	Print("[32mOK ![m\n\n");
	return(0);
	}

/*
 *  All currently supported commands
 */

static STRPTR CommandArray[] = {"UN",				// Username (left-aligned)
																"UL",				// Userlocation (left-aligned)
																"UB",				// User Upload Bytes
																"DB",				// User Download Bytes
                                "BU",				// System Uploads Bytes
                                "BD",				// System Downloads Bytes
																"FU",       // System Files Uploaded
                                "FD",				// System Files Downloaded
                                "SC",				// System Calls
																"TU",				// Total Users in BBS
																"$$",				// Program Name and Version
																"UF",				// User Upload Files
																"DF",       // User Download Files
																"UC",				// Upload CPS
																"DC",       // Download CPS
																"CL",				// Callers
                                "MW",				// Messages written
																"NB",				// Nuked Bytes
																"NF",       // Nuked Files
																"CH",				// Chats
																"HA",				// Hacks
																"LD",				// Last Update -> Date
																"LT",				// Last Update -> Time
																"TM",       // Total Messages
																"Y2",       // Y2K compatible Date (YYYY) - New for V1.32
																"SN",       // System Name (BBSName) - New for V1.32
																	0};
// Supported command-defines

#define CMD_USERNAME					0
#define CMD_USERLOCATION			1
#define CMD_UPLOADBYTES				2
#define CMD_DOWNLOADBYTES			3
#define	CMD_SYSTEMULBYTES			4
#define CMD_SYSTEMDLBYTES			5
#define CMD_SYSTEMULFILES			6
#define CMD_SYSTEMDLFILES			7
#define CMD_SYSTEMTOTALCALLS	8
#define CMD_SYSTEMTOTALUSERS	9
#define CMD_PROGRAMVERSION		10
#define CMD_UPLOADFILES				11
#define CMD_DOWNLOADFILES			12
#define CMD_UPLOADCPS					13
#define CMD_DOWNLOADCPS				14
#define CMD_CALLERS						15
#define CMD_MESSAGESWRITE			16
#define CMD_NUKEDBYTES				17
#define CMD_NUKEDFILES				18
#define CMD_CHATS							19
#define CMD_HACKS							20
#define CMD_LASTDATE          21
#define CMD_LASTTIME					22
#define CMD_TOTALMESSAGES			23
#define CMD_LASTDATEY2K				24
#define CMD_SYSTEMNAME        25

/*
 *  Parses a given line, extracts the command and fill it with the according values
 *  Uses TagToSpace() written by David 'Strider' Wettig
 */

void CheckCommand(char *buf)
	{
  char  	ReplKey[4],
  				digitbuf[256];					// Used to convert integers in locale-formated strings
  long  	ReplLen,ReplPos,lv;
	long		MyRealPos=NULL,					// Contains the Position in reverse order (memory list)
					MyStart=NULL,						// Contains position inside buf to start override
					MyAlign=0;							// Indicates left- or right aligned output
  STRPTR 	ReturnPtr;

  while(ReturnPtr=TagToSpace(buf, CommandArray, ReplKey, &ReplLen, &ReplPos))
		{
    MyRealPos = (usercounter - ReplPos);							// Reverse order
		MyStart = strlen(buf) - strlen(ReturnPtr);    		// Start of Dance
		lv=0;
		while(*CommandArray[lv])													// Get numeric value of found Command
			{
			if(!(Stricmp(CommandArray[lv],ReplKey)))
				{
				break;
				}
			else
				{
				lv++;
        }
			}
		if(islower(*ReplKey))                             // Check for alignment
			{
			MyAlign=1;
      }
		else
			{
			MyAlign=0;
			}
		switch(lv)																				// Copy values into buf according to command
			{
			case	CMD_USERNAME:      		MyCopyString(buf,list[MyRealPos].UserName,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_USERLOCATION:  		MyCopyString(buf,list[MyRealPos].UserLocation,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_UPLOADBYTES:  		FAMENum64ToStr(list[MyRealPos].BytesUpHi,list[MyRealPos].BytesUpload,FNSF_GROUPING|FNSF_NUMLOCALE,255,digitbuf);
																	MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_DOWNLOADBYTES:  	FAMENum64ToStr(list[MyRealPos].BytesDownHi,list[MyRealPos].BytesDownload,FNSF_GROUPING|FNSF_NUMLOCALE,255,digitbuf);
																	MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_UPLOADFILES:			FAMENumToStr(list[MyRealPos].Uploads,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
																	MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_DOWNLOADFILES:		FAMENumToStr(list[MyRealPos].Downloads,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
																	MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_SYSTEMULBYTES:		MyCopyString(buf,UBYTES,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_SYSTEMDLBYTES:		MyCopyString(buf,DBYTES,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_SYSTEMULFILES:		MyCopyString(buf,UFILES,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_SYSTEMDLFILES:		MyCopyString(buf,DFILES,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_SYSTEMTOTALCALLS: FAMENumToStr(totals.calls,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
                                  MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_SYSTEMTOTALUSERS: FAMENumToStr(usercounter,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
                                  MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_PROGRAMVERSION:		MySPrintf(digitbuf,"mULTI-tOP v%s by Sascha 'SieGeL' Pfalz",TOP_VER);
																	MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_UPLOADCPS:				FAMENumToStr(list[MyRealPos].HighCPSUP,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
                                  MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_DOWNLOADCPS:			FAMENumToStr(list[MyRealPos].HighCPSDown,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
                                  MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_CALLERS:					FAMENumToStr(list[MyRealPos].Calls,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
                                  MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_MESSAGESWRITE:		FAMENumToStr(list[MyRealPos].MessageWrite,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
                                  MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_NUKEDBYTES:				FAMENumToStr(list[MyRealPos].BytesNuked,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
                                  MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_NUKEDFILES:				FAMENumToStr(list[MyRealPos].FilesNuked,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
                                  MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_CHATS:						FAMENumToStr(list[MyRealPos].NumberOfChats,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
                                  MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_HACKS:						FAMENumToStr(list[MyRealPos].AllHacks,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
                                  MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_LASTDATE:					UpdateStamp();
																	MyCopyString(buf,Datum,ReplLen,MyStart,MyAlign);
                                  break;

			case	CMD_LASTTIME:					UpdateStamp();
																	MyCopyString(buf,Zeit,ReplLen,MyStart,MyAlign);
                                  break;

			case	CMD_TOTALMESSAGES:		FAMENumToStr(totals.messages,FNSF_GROUPING|FNSF_NUMLOCALE,29,digitbuf);
                                  MyCopyString(buf,digitbuf,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_LASTDATEY2K:			FormatStamp(NULL,Datum,Zeit);
																	MyCopyString(buf,Datum,ReplLen,MyStart,MyAlign);
																	break;

			case	CMD_SYSTEMNAME:				MyCopyString(buf,mp->pathes[BBSNAME],ReplLen,MyStart,MyAlign);
																	break;
			}
 		}
	}

/*
 *  MyCopyString() inserts "string" into "buffer" starting at "start" with a length of "length"
 *  If align is set to 1, the string will be inserted right-aligned, else left-aligned
 */

void MyCopyString(char *buffer, char *string, long length, long start, long align)
	{
	long 	startpoint=start,
        lv=NULL,
				mylength = length;

	if(align==1)
		{
    startpoint = startpoint + (mylength - strlen(string));
		}
	lv=0;
	while(lv < mylength)
    {
    if(string[lv] == '\0') break;
		buffer[startpoint] = string[lv];
		lv++;
    startpoint++;
		}
	}

void UpdateStamp(void)
	{
	FAMEMemSet(globds,'\0',sizeof(struct DateStamp));
	globds=DateStamp(globds);
	globdt->dat_Stamp=*globds;
	globdt->dat_Format=FORMAT_DOS;
	globdt->dat_StrTime=Zeit;
	globdt->dat_StrDate=Datum;
	DateToStr(globdt);
	}

/*
 *  Hook for locale.library/FormatDate() - Used in FormatStamp()
 */

STATIC LONG __saveds __asm FormatDateHookFunc(register __a0 struct Hook *Hook,register __a1 UBYTE Char)
	{
	STRPTR String=Hook->h_Data;
	*String++=Char;
	Hook->h_Data=String;
	return(TRUE);
	}

/*
 *  Function to convert Date & Time to the user's locale setting. Could be
 *  also called without Stamp to get current time & date.
 */

BOOL __regargs FormatStamp(struct DateStamp *Stamp,STRPTR DateBuffer,STRPTR TimeBuffer)
	{
	struct DateStamp Now;
	struct Hook LocalHook={{NULL}, (HOOKFUNC)FormatDateHookFunc};

	if(!Stamp) DateStamp(Stamp = &Now);
	if(myloc)
		{
		if(DateBuffer)
			{
			LocalHook.h_Data=DateBuffer;
			FormatDate(myloc,"%m-%d-%Y",Stamp,&LocalHook);
			}
		if(TimeBuffer)
			{
			LocalHook.h_Data=TimeBuffer;
			FormatDate(myloc,"%m-%d-%Y",Stamp,&LocalHook);
			}
		}
	return(TRUE);
	}
