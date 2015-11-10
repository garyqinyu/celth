# Microsoft Developer Studio Project File - Name="peg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=peg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "peg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "peg.mak" CFG="peg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "peg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "peg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "peg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "peg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Z7 /Od /I "E:\ST\peg182st20\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "peg - Win32 Release"
# Name "peg - Win32 Debug"
# Begin Source File

SOURCE=..\..\..\..\HDTVMenu_EMS2004\helpwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\ltoa.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\p2dpoly.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pal256.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\panimwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pbig5map.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pbitmaps.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pblight.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pbmpconv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pbutton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pcbdial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pcdial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pchart.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pclight.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pcombo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pdecbtn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pdecwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pdial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pdialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\peditbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pfbdial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pfdial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pfonts.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pgifconv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pgroup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\picon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pimgconv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pjpgconv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\plbscale.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\plight.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\plist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pliteral.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\plnchart.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\plscale.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pmenfont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pmenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pmesgwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pmessage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pmlchart.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pmlmsgwn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pmltbtn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pnotebk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\ppngconv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\ppresent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pprogbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pprogwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pprompt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pquant.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\prect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pscale.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pscroll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\psincos.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\psjmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pslider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pspin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pspread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pstatbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pstchart.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pstring.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\psysfont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\ptable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\ptextbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pthing.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\ptitle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\ptoolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\ptree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pvprompt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\pzip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\w32scrn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\winpeg.cpp
# End Source File
# End Target
# End Project
