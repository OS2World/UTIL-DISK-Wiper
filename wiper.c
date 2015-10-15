/************************************************************************
   WIPER 1.0 (C)1991 Turgut Kalfaoglu <TURGUT@TREARN.BITNET>
                     1378 Sok, 8/10, Alsancak, Izmir,Turkey

   This program wipes away a directory and its subdirectories by
   removing its files, and the directories themselves.

   This file can be compiled either for OS/2 or DOS. Use /D option of MSC
   to define the OS2 variable, like:  /DSO2 to compile it for OS/2

   High Performance File System - friendly version.

   It returns an error code (>0) if it fails, or 0 if everything goes
   well.

   Compiled with maximum optimization and minimized codesize under MSC 6

   USER-SUPPORTED Software - if you find it of use, send $10 to the above
   address. Thank you for your support!

************************************************************************/

#ifdef OS2
#define INCL_DOS
#include <os2.h>
#else
#include <dos.h>
#endif
#include <errno.h>
#include <direct.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <string.h>
#include <malloc.h>

/* Let's please _fastcall: */
void showhelp();
int yallah(char *); /* in case you wonder, 'yallah' means 'go for it' in Turkish :) */
void fatal(char *,char *);

main(argc,argv)
int argc;
char *argv[];
{       char a;
        if (argc<2) showhelp();
        if (strcmp(argv[1],"\\")==0) {
           printf("Warning! This will remove all files from drive!\n");
           printf("Hit any key to perform this function!\n");
           a = getch(); }
        exit(yallah(argv[1]));
}

void showhelp() {
    printf("WIPER 1.0 (C)1991 Turgut Kalfaoglu,1378 Sok 8/10,Alsancak,Izmir,Turkey\n\n");
    printf("This program enables you to remove a whole branch of your diskette or hard disk\n");
    printf("Simply give the subdirectory you want removed, and WIPER will remove\n");
    printf("all subdirectories of that directory, along with all files contained\n");
    printf("within those directories.\n");
    printf("Example:\n");
    printf("To remove C:\\WINDOWS along with its subdirectories from your hard disk:\n");
    printf("1) Make the C: drive default by typing:\n");
    printf("    C:      <ENTER>\n");
    printf("2) Change to a directory 'above' the want you removed:\n");
    printf("    CD \\    <ENTER>\n");
    printf("3) Enter the command to remove the directory:\n");
    printf("    WIPER WINDOWS\n\n\n");
    printf("This program is user-supported. If you find it useful, please send $10\n");
    fatal("to the above address. %s!","Thank you");
}

int yallah(char *path)
{
#ifdef OS2
       FILEFINDBUF find;
       unsigned srch_cnt=1;
#define name achName
#define attrib attrFile
#undef  _A_SUBDIR
#define _A_SUBDIR 16
#else
       struct find_t find;
#endif

    char *dirn[100];
    char dira[100];
    unsigned handle=0xffff,d;
    int i=0;

    /* Let's see if they are desired entries: */
    if (strcmp(path,".")==0) return 0; /* naw.. */
    if (strcmp(path,"..")==0) return 0;

    /* Try changing to that directory, fail if we can't */
    if (chdir(path) == -1) fatal("Error changing path to %s",path);

    /* Load up all the files, and directory names */
#ifdef OS2

   /* 'handle' is like a reference mark for this query, 55 means 'match
   all files, regardless of attributes', srch_cnt tells how many items it
   can return - for compatibility with the rest of the code, I set it to 1.
   Making it return everything would have been a bit more efficient.
   Finally, 0L is a reserved field, it needs to be a long integer.. */

   if (!DosFindFirst("*.*",&handle,55,&find,sizeof(find),&srch_cnt,0L)) {

#else
    if (!_dos_findfirst("*.*",0xffff,&find)) {
#endif
    /* Filename can be as long as 260 characters for HPFS (defined in BSEDOS.H)
       We don't care - it's all virtual to us :)  */

       if ((dirn[i] = malloc(strlen(find.name)+1))==NULL)
          fatal("Error allocating memory for %s",find.name);

       strcpy(dirn[i],find.name); /* Store name and attributes of 'zis' file */
       dira[i++] = find.attrib; }

    /* Keep at it until we read the whole directory */
    while (1) {
#ifdef OS2
      /* Find the next occurrance, using the reference 'handle' we are given: */

      if (DosFindNext(handle,&find,sizeof(find),&srch_cnt)) break;
#else
      if (_dos_findnext(&find)) break;
#endif
      if ((dirn[i] = malloc(strlen(find.name)+1))==NULL)
         fatal("Error allocating memory for %s",find.name);
      strcpy(dirn[i],find.name);
      dira[i] = find.attrib;
      i++;
    }
    i--;

    while (i > -1) {
        if (dira[i] & _A_SUBDIR)
           d=yallah(dirn[i]);
        else
           if (remove(dirn[i])) fatal("Cannot remove file %s. Is it read-only?",dirn[i]);
        i--;
    }

    /* We are done here, go up a level */
    if (chdir("..") == -1) fatal("cannot change path to '%s'","..");

    /* remove the directory that we just got out of */
    if (rmdir(path) != 0)  fatal("Just cannot remove %s directory",path);

    return 0; /* normal out */
}

/* Something went wrong, display and quit */
void fatal(char *blurb,char *string)
{
        printf(blurb,string);
        printf("\n");
        exit(1);
}

