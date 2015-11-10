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
IDE_LFLAGSDOS =  -Li:\BC45\LIB
IDE_BFLAGS = 
LLATDOS_dospegdexe =  -c -Tde
RLATDOS_dospegdexe = 
BLATDOS_dospegdexe = 
CNIEAT_dospegdexe = -Ii:\BC45\INCLUDE;..\..\..\INCLUDE;..\..\..\EXAMPLES\PEGDEMO; -D
LNIEAT_dospegdexe = -x
LEAT_dospegdexe = $(LLATDOS_dospegdexe)
REAT_dospegdexe = $(RLATDOS_dospegdexe)
BEAT_dospegdexe = $(BLATDOS_dospegdexe)
    
#
# Dependency List
#
Dep_dospeg = \
   dospeg.exe

dospeg : BccDos.cfg $(Dep_dospeg)
  echo MakeNode 

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
   dospeg.obj\
   pbitmaps.obj\
   pbutton.obj\
   pcombo.obj\
   pdecwin.obj\
   pdialog.obj\
   peditbox.obj\
   pfonts.obj\
   pgroup.obj\
   picon.obj\
   plist.obj\
   pliteral.obj\
   pmenfont.obj\
   pmenu.obj\
   pmesgwin.obj\
   pmessage.obj\
   pmltbtn.obj\
   ppresent.obj\
   pprogbar.obj\
   pprogwin.obj\
   pprompt.obj\
   prect.obj\
   pscreen.obj\
   pscroll.obj\
   pslider.obj\
   pspin.obj\
   pstatbar.obj\
   pstring.obj\
   psysfont.obj\
   ptextbox.obj\
   pthing.obj\
   ptitle.obj\
   pvprompt.obj\
   pwindow.obj\
   vgascrn.obj\
   pegdemo.obj\
   monsfont.obj\
   scripfon.obj\
   demobmp.obj\
   pool.obj\
   sand.obj\

dospeg.exe : $(Dep_dospegdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_dospegdexe) $(LNIEAT_dospegdexe) +
i:\BC45\LIB\c0l.obj+
dospeg.obj+
pbitmaps.obj+
pbutton.obj+
pcombo.obj+
pdecwin.obj+
pdialog.obj+
peditbox.obj+
pfonts.obj+
pgroup.obj+
picon.obj+
plist.obj+
pliteral.obj+
pmenfont.obj+
pmenu.obj+
pmesgwin.obj+
pmessage.obj+
pmltbtn.obj+
ppresent.obj+
pprogbar.obj+
pprogwin.obj+
pprompt.obj+
prect.obj+
pscreen.obj+
pscroll.obj+
pslider.obj+
pspin.obj+
pstatbar.obj+
pstring.obj+
psysfont.obj+
ptextbox.obj+
pthing.obj+
ptitle.obj+
pvprompt.obj+
pwindow.obj+
vgascrn.obj+
pegdemo.obj+
monsfont.obj+
scripfon.obj+
demobmp.obj+
pool.obj+
sand.obj
$<,$*
i:\BC45\LIB\cl.lib

|

dospeg.obj : ..\..\..\source\dospeg.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\dospeg.cpp
|

pbitmaps.obj :  ..\..\..\source\pbitmaps.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pbitmaps.cpp
|

pbutton.obj :  ..\..\..\source\pbutton.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pbutton.cpp
|

pcombo.obj :  ..\..\..\source\pcombo.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pcombo.cpp
|

pdecwin.obj :  ..\..\..\source\pdecwin.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pdecwin.cpp
|

pdialog.obj :  ..\..\..\source\pdialog.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pdialog.cpp
|

peditbox.obj :  ..\..\..\source\peditbox.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\peditbox.cpp
|

pfonts.obj :  ..\..\..\source\pfonts.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pfonts.cpp
|

pgroup.obj :  ..\..\..\source\pgroup.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pgroup.cpp
|

picon.obj :  ..\..\..\source\picon.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\picon.cpp
|

plist.obj :  ..\..\..\source\plist.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\plist.cpp
|

pliteral.obj :  ..\..\..\source\pliteral.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pliteral.cpp
|

pmenfont.obj :  ..\..\..\source\pmenfont.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pmenfont.cpp
|

pmenu.obj :  ..\..\..\source\pmenu.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pmenu.cpp
|

pmesgwin.obj :  ..\..\..\source\pmesgwin.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pmesgwin.cpp
|

pmessage.obj :  ..\..\..\source\pmessage.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pmessage.cpp
|

pmltbtn.obj :  ..\..\..\source\pmltbtn.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pmltbtn.cpp
|

ppresent.obj :  ..\..\..\source\ppresent.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\ppresent.cpp
|

pprogbar.obj :  ..\..\..\source\pprogbar.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pprogbar.cpp
|

pprogwin.obj :  ..\..\..\source\pprogwin.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pprogwin.cpp
|

pprompt.obj :  ..\..\..\source\pprompt.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pprompt.cpp
|

prect.obj :  ..\..\..\source\prect.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\prect.cpp
|

pscreen.obj :  ..\..\..\source\pscreen.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pscreen.cpp
|

pscroll.obj :  ..\..\..\source\pscroll.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pscroll.cpp
|

pslider.obj :  ..\..\..\source\pslider.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pslider.cpp
|

pspin.obj :  ..\..\..\source\pspin.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pspin.cpp
|

pstatbar.obj :  ..\..\..\source\pstatbar.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pstatbar.cpp
|

pstring.obj :  ..\..\..\source\pstring.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pstring.cpp
|

psysfont.obj :  ..\..\..\source\psysfont.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\psysfont.cpp
|

ptextbox.obj :  ..\..\..\source\ptextbox.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\ptextbox.cpp
|

pthing.obj :  ..\..\..\source\pthing.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pthing.cpp
|

ptitle.obj :  ..\..\..\source\ptitle.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\ptitle.cpp
|

pvprompt.obj :  ..\..\..\source\pvprompt.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pvprompt.cpp
|

pwindow.obj :  ..\..\..\source\pwindow.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pwindow.cpp
|

#use this for standard VGA:

vgascrn.obj :  ..\..\..\source\vgascrn.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\vgascrn.cpp
|

#use this for 1024x768:

#svgascrn.obj :  ..\..\..\source\svgascrn.cpp
#  $(BCCDOS) -c @&&|
# $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\svgascrn.cpp
#|

#these files are only for run-time image conversion:

#pbmpconv.obj :  ..\..\..\source\pbmpconv.cpp
#  $(BCCDOS) -c @&&|
# $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pbmpconv.cpp
#|
#
#pimgconv.obj :  ..\..\..\source\pimgconv.cpp
#  $(BCCDOS) -c @&&|
# $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pimgconv.cpp
#|
#
#pgifconv.obj :  ..\..\..\source\pgifconv.cpp
#  $(BCCDOS) -c @&&|
# $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pgifconv.cpp
#|
#
#pjpgconv.obj :  ..\..\..\source\pjpgconv.cpp
#  $(BCCDOS) -c @&&|
# $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\source\pjpgconv.cpp
#|
#

#the remaining files are for the demo application:

pegdemo.obj :  ..\..\..\examples\pegdemo\pegdemo.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\examples\pegdemo\pegdemo.cpp
|

monsfont.obj :  ..\..\..\examples\pegdemo\monsfont.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\examples\pegdemo\monsfont.cpp
|

scripfon.obj :  ..\..\..\examples\pegdemo\scripfon.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\examples\pegdemo\scripfon.cpp
|

demobmp.obj :  ..\..\..\examples\pegdemo\demobmp.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\examples\pegdemo\demobmp.cpp
|

pool.obj :  ..\..\..\examples\pegdemo\pool.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\examples\pegdemo\pool.cpp
|

sand.obj :  ..\..\..\examples\pegdemo\sand.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_dospegdexe) $(CNIEAT_dospegdexe) -o$@ ..\..\..\examples\pegdemo\sand.cpp
|



