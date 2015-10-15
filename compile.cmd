/* This file re-compiles WIPER.C into WIPER.EXE using MSC 6.
   It also uses TAG.EXE, a public domain utility to make WIPER.EXE
   runnable in an OS/2 window.
*/

type = 'huh?'
Do while type \='DOS' & type\='OS2' & type \='OS/2'
  Say 'Enter the target either DOS or OS/2 :'
  Pull type
End
Say 'Compiling for' type'.. Please be patient'

Select
  When type == 'DOS' Then Do
     'CL /AL /FPi /G2rs /Ozax /Lr wiper.c /link /stack:30000 /exepack /packcode /non'
     If rc\=0 Then Call failed rc
  End
  When type == 'OS2' | type == 'OS/2' Then Do
     'CL /AL /FPi /G2rs /Zp /Ozax /Lp /DOS2 wiper.c /link /stack:30000'
     If rc\=0 Then Call failed rc
     'TAG /WIN wiper.exe'
     If rc\=0 Then Call failed rc
  End
  Otherwise Do
     Say 'Unknown type:' type
     Call Failed 1
  End
End
Say 'End of compilation'
Exit(0)

Failed:
Parse arg ReturnCode
  Say 'Operation failed - return code was' rc
  Exit rc

