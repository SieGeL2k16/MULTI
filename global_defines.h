/*
 *   GLOBAL DEFINES FOR MULTI-TOP
 */

#define TOP_VER "1.32"
#define Print(s) FPuts(Output(),s)

/*
 *  Defines for Config-struct members
 */

#define USERDATA 	0
#define ULISTER  	1
#define UPBYTES  	2
#define DLBYTES  	3
#define UPFILES  	4
#define DLFILES  	5
#define UPCPS    	6
#define DLCPS    	7
#define CALLERS  	8
#define MESSIS   	9
#define N_BYTES 	10
#define N_FILES 	11
#define TOP_CHAT	12
#define TOP_HACKS	13
#define BBSNAME		14
#define MAX_ENTRIES 15

/*
 *  Amount of currently supported Bulletins to create
 */

#define BULL_ENTRIES 14

/*
 *  These defines makes the usage of the 64-Bit integers a bit easier (V0.60)
 */

#define HIGH	0
#define LOW		1

/*
 *  Additional DOS Defines
 */

#define DOS_BUF_SIZE	1024
