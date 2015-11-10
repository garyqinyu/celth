#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCCDOS  = Bcc +BccDos.cfg 
TLINK   = TLink
TLIB    = TLib
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LFLAGSDOS =  -Ld:\BC45\LIB
IDE_BFLAGS = 
LLATDOS_dospegdexe =  -c -Tde
RLATDOS_dospegdexe = 
BLATDOS_dospegdexe = 
CNIEAT_dospegdexe = -Id:\BC45\INCLUDE;..\INCLUDE; -D
LNIEAT_dospegdexe = -x
LEAT_dospegdexe = $(LLATDOS_dospegdexe)
REAT_dospegdexe = $(RLATDOS_dospegdexe)
BEAT_dospegdexe = $(BLATDOS_dospegdexe)
    

#
# the required PEG files are listed first, followed in this case by the 
# demo files
#
# To replace the standard VGA screen class with the 1024x768 SVGA,
# replace all occurances of 'vgascrn' with 'svgascrn'.
#
# All files listed after and including "pegdemo" are only needed for the
# demo app. When you are ready to build your own application, you can remove
# these files.
#


Dep_dospegdexe = \
   pthing.obj\
   pscreen.obj\
   prect.obj\
   pwindow.obj\
   pdialog.obj\
   pmesgwin.obj\
   pmltbtn.obj\
   pprogwin.obj\
   pdecwin.obj\
   panimwin.obj\
   pspread.obj\
   ptable.obj\
   pliteral.obj\
   pnotebk.obj\
   pbutton.obj\
   pprogbar.obj\
   pbitmaps.obj\
   ptitle.obj\
   pstatbar.obj\
   pprompt.obj\
   pvprompt.obj\
   pstring.obj\
   pmenu.obj\
   ptextbox.obj\
   peditbox.obj\
   plist.obj\
   pcombo.obj\
   picon.obj\
   pmessage.obj\
   ppresent.obj\
   pfonts.obj\
   pscroll.obj\
   pspin.obj\
   pslider.obj\
   pgroup.obj\
   dospeg.obj\
   psysfont.obj\
   pmenfont.obj\

dospeg.lib : $(Dep_dospegdexe)
        tlib dospeg @peglib.rsp /C /0,peglib.lst

pthing.obj :  ..\source\pthing.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pthing.cpp
|

pscreen.obj :  ..\source\pscreen.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pscreen.cpp
|

prect.obj :  ..\source\prect.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\prect.cpp
|

pwindow.obj :  ..\source\pwindow.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pwindow.cpp
|

pdialog.obj :  ..\source\pdialog.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pdialog.cpp
|

pmesgwin.obj :  ..\source\pmesgwin.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pmesgwin.cpp
|

pmltbtn.obj :  ..\source\pmltbtn.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pmltbtn.cpp
|

pprogwin.obj :  ..\source\pprogwin.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pprogwin.cpp
|

pdecwin.obj :  ..\source\pdecwin.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pdecwin.cpp
|

panimwin.obj :  ..\source\panimwin.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\panimwin.cpp
|

ptable.obj :  ..\source\ptable.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\ptable.cpp
|

pliteral.obj :  ..\source\pliteral.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pliteral.cpp
|

pspread.obj :  ..\source\pspread.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pspread.cpp
|

pnotebk.obj :  ..\source\pnotebk.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pnotebk.cpp
|

pbutton.obj :  ..\source\pbutton.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pbutton.cpp
|

pprogbar.obj :  ..\source\pprogbar.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pprogbar.cpp
|

pbitmaps.obj :  ..\source\pbitmaps.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pbitmaps.cpp
|

ptitle.obj :  ..\source\ptitle.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\ptitle.cpp
|

pstatbar.obj :  ..\source\pstatbar.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pstatbar.cpp
|

pprompt.obj :  ..\source\pprompt.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pprompt.cpp
|

pvprompt.obj :  ..\source\pvprompt.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pvprompt.cpp
|

pstring.obj :  ..\source\pstring.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pstring.cpp
|

pmenu.obj :  ..\source\pmenu.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pmenu.cpp
|

ptextbox.obj :  ..\source\ptextbox.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\ptextbox.cpp
|

peditbox.obj :  ..\source\peditbox.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\peditbox.cpp
|

plist.obj :  ..\source\plist.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\plist.cpp
|

pcombo.obj :  ..\source\pcombo.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pcombo.cpp
|

picon.obj :  ..\source\picon.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\picon.cpp
|

pmessage.obj :  ..\source\pmessage.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pmessage.cpp
|

ppresent.obj :  ..\source\ppresent.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\ppresent.cpp
|

pfonts.obj :  ..\source\pfonts.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pfonts.cpp
|

pscroll.obj :  ..\source\pscroll.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pscroll.cpp
|

pspin.obj :  ..\source\pspin.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pspin.cpp
|

pslider.obj :  ..\source\pslider.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pslider.cpp
|

pgroup.obj :  ..\source\pgroup.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pgroup.cpp
|

psysfont.obj :  ..\source\psysfont.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\psysfont.cpp
|

pmenfont.obj :  ..\source\pmenfont.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\pmenfont.cpp
|

dospeg.obj : ..\source\dospeg.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\source\dospeg.cpp
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=dospeg.csm
-ml
-f
| $@


