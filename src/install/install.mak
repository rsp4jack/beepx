# Microsoft Developer Studio Generated NMAKE File, Based on install.dsp
!IF "$(CFG)" == ""
CFG=install - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. install - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "install - Win32 Release" && "$(CFG)" != "install - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "install.mak" CFG="install - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "install - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "install - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "install - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\bin\install.exe"


CLEAN :
	-@erase "$(INTDIR)\addacl.obj"
	-@erase "$(INTDIR)\beeper.obj"
	-@erase "$(INTDIR)\install.obj"
	-@erase "$(INTDIR)\installinf.obj"
	-@erase "$(INTDIR)\res.res"
	-@erase "$(INTDIR)\takeown.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "..\..\bin\install.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "C:\NTDDK\INC" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\install.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0xc07 /fo"$(INTDIR)\res.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\install.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib setupapi.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\install.pdb" /machine:I386 /nodefaultlib /out:"../../bin/install.exe" /opt:NOWIN98 
LINK32_OBJS= \
	"$(INTDIR)\addacl.obj" \
	"$(INTDIR)\beeper.obj" \
	"$(INTDIR)\install.obj" \
	"$(INTDIR)\installinf.obj" \
	"$(INTDIR)\takeown.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\res.res"

"..\..\bin\install.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "install - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\install.exe"


CLEAN :
	-@erase "$(INTDIR)\addacl.obj"
	-@erase "$(INTDIR)\beeper.obj"
	-@erase "$(INTDIR)\install.obj"
	-@erase "$(INTDIR)\installinf.obj"
	-@erase "$(INTDIR)\res.res"
	-@erase "$(INTDIR)\takeown.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(OUTDIR)\install.exe"
	-@erase "$(OUTDIR)\install.ilk"
	-@erase "$(OUTDIR)\install.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "C:\NTDDK\INC" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\install.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0xc07 /fo"$(INTDIR)\res.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\install.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib setupapi.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\install.pdb" /debug /machine:I386 /out:"$(OUTDIR)\install.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\addacl.obj" \
	"$(INTDIR)\beeper.obj" \
	"$(INTDIR)\install.obj" \
	"$(INTDIR)\installinf.obj" \
	"$(INTDIR)\takeown.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\res.res"

"$(OUTDIR)\install.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("install.dep")
!INCLUDE "install.dep"
!ELSE 
!MESSAGE Warning: cannot find "install.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "install - Win32 Release" || "$(CFG)" == "install - Win32 Debug"
SOURCE=.\addacl.c

"$(INTDIR)\addacl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\spkrfix\beeper.c

"$(INTDIR)\beeper.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\install.c

"$(INTDIR)\install.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\installinf.c

"$(INTDIR)\installinf.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\takeown.c

"$(INTDIR)\takeown.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\version.c

"$(INTDIR)\version.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\res.rc

"$(INTDIR)\res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

