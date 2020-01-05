#include <stdlib.h>
#include <stdio.h>
#include <string.h>

STRPTR TagToSpace(STRPTR InBuffer, STRPTR *CommandArray, STRPTR ReplKey, long *ReplLen, long *ReplPos);

STRPTR TagToSpace(STRPTR InBuffer, STRPTR *CommandArray, STRPTR ReplKey, long *ReplLen, long *ReplPos)
	{
  STRPTR  ReturnBuffer = NULL,
          LastMatchPos = NULL;
  char    CommandBuf[16];
  long    WalkCnt;


  while(InBuffer[0])
  	{
    if(InBuffer[0] == '%')
    	{
      LastMatchPos = InBuffer;

      for(WalkCnt=0;WalkCnt<9;WalkCnt++)
      	{
        CommandBuf[WalkCnt] = InBuffer[0];

        InBuffer ++;

        if( ! InBuffer[0])
        	{
          InBuffer = NULL;

          break;
        	}
      	}

      if(InBuffer)
      {
        if((char)(CommandBuf + 3)[0] == '-' && (char)(CommandBuf + 6)[0] == '-')
        {
          char  ReplTmpLen[4],
                ReplTmpPos[4];

          memset(&ReplTmpLen[0],'\0',4);
          memset(&ReplKey[0],'\0',3);
          memset(&ReplTmpPos[0],'\0',4);

          strncpy(ReplTmpLen,(CommandBuf + 1),2);
          strncpy(ReplKey,(CommandBuf + 4),2);
          strncpy(ReplTmpPos,(CommandBuf + 7),2);

          WalkCnt = 0;

          (*ReplLen) = atol(ReplTmpLen);
          (*ReplPos) = atol(ReplTmpPos);

          while(CommandArray[WalkCnt])
          {
            if( ! Stricmp(CommandArray[WalkCnt],ReplKey))
            {
              STRPTR BackupBuf;
              long   RightStrLen  = strlen(InBuffer);

              if(BackupBuf = AllocVec(RightStrLen+1,MEMF_PUBLIC|MEMF_CLEAR))
              {
                ReturnBuffer = LastMatchPos;

                strncpy(BackupBuf,InBuffer,RightStrLen);
                memset(LastMatchPos,' ',(*ReplLen));
                LastMatchPos += (*ReplLen);
                LastMatchPos[0] = '\0';
                strcat(LastMatchPos,BackupBuf);


                FreeVec(BackupBuf);
              }

              break;
            }

            WalkCnt ++;
          }
        }
      }
    }

    if( ! InBuffer || ReturnBuffer)
    {
      break;
    }

    InBuffer ++;
  }

  return(ReturnBuffer);
}

