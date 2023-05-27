# Microsoft Developer Studio Generated NMAKE File, Based on spkrfix.dsp
!IF "$(CFG)" == ""
CFG=spkrfix - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. spkrfix - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "spkrfix - Win32 Release" && "$(CFG)" != "spkrfix - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "spkrfix.mak" CFG="spkrfix - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "spkrfix - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "spkrfix - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "spkrfix - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\bin\spkrfix\spkrfix.dll"


CLEAN :
	-@erase "$(INTDIR)\beeper.obj"
	-@erase "$(INTDIR)\spkrfix.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vdmpriv.obj"
	-@erase "$(OUTDIR)\spkrfix.exp"
	-@erase "$(OUTDIR)\spkrfix.lib"
	-@erase "..\..\bin\spkrfix\spkrfix.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Gz /MT /W3 /O2 /I "c:\NTDDK\INC" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\spkrfix.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\spkrfix.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ntvdm.lib /nologo /entry:"VDDInitialize" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\spkrfix.pdb" /machine:I386 /out:"../../bin/spkrfix/spkrfix.dll" /implib:"$(OUTDIR)\spkrfix.lib" /libpath:"c:\NTDDK\LIBFRE\I386" /OPT:NOWIN98 
LINK32_OBJS= \
	"$(INTDIR)\beeper.obj" \
	"$(INTDIR)\spkrfix.obj" \
	"$(INTDIR)\vdmpriv.obj"

"..\..\bin\spkrfix\spkrfix.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "spkrfix - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\spkrfix.dll"


CLEAN :
	-@erase "$(INTDIR)\beeper.obj"
	-@erase "$(INTDIR)\spkrfix.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vdmpriv.obj"
	-@erase "$(OUTDIR)\spkrfix.dll"
	-@erase "$(OUTDIR)\spkrfix.exp"
	-@erase "$(OUTDIR)\spkrfix.ilk"
	-@erase "$(OUTDIR)\spkrfix.lib"
	-@erase "$(OUTDIR)\spkrfix.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Gz /MTd /W3 /Gm /GX /ZI /Od /I "c:\NTDDK\INC" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\spkrfix.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\spkrfix.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"VDDInitialize" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\spkrfix.pdb" /debug /machine:I386 /out:"$(OUTDIR)\spkrfix.dll" /implib:"$(OUTDIR)\spkrfix.lib" /pdbtype:sept /libpath:"c:\NTDDK\LIBCHK\I386" 
LINK32_OBJS= \
	"$(INTDIR)\beeper.obj" \
	"$(INTDIR)\spkrfix.obj" \
	"$(INTDIR)\vdmpriv.obj"

"$(OUTDIR)\spkrfix.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("spkrfix.dep")
!INCLUDE "spkrfix.dep"
!ELSE 
!MESSAGE Warning: cannot find "spkrfix.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "spkrfix - Win32 Release" || "$(CFG)" == "spkrfix - Win32 Debug"
SOURCE=.\beeper.c

"$(INTDIR)\beeper.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\spkrfix.c

"$(INTDIR)\spkrfix.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vdmpriv.c

"$(INTDIR)\vdmpriv.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

