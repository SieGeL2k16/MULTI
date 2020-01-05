// PROTOTYPES FOR MULTI-TOP

extern void		wb(char *);										// Beendet Tool mit Fehlermeldung
extern BOOL		ReadConfig(void);							// Configuration aus PRGDIR: lesen
extern BOOL		ReadAllDatas(void);						// Liest user.data und sortiert aus
extern void		free_elements(struct Liste *);
extern void  	FileCenter(char *,BPTR);			// Zentriert String nach BPTR
extern void		CreateUserList(void);					// Erzeugt UserListe
extern void		CreateBull(long);							// Erzeugt Bulletins
extern void		myquicksort(long,long,long);
extern VOID __stdargs MySPrintf(STRPTR buffer, char *formatString,...);


// extern STRPTR FormatNumber(ULONG Number,char InStr[]);

/*
 *  Prototype for Strider's TagToSpace() function
 */

extern STRPTR TagToSpace(STRPTR InBuffer, STRPTR *CommandArray, STRPTR ReplKey, long *ReplLen, long *ReplPos);
