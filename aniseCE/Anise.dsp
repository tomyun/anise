# Microsoft Developer Studio Project File - Name="Anise" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Anise - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Anise.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Anise.mak" CFG="Anise - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Anise - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Anise - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Anise - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /Zm200 /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msvcrt.lib msvcprt.lib winmm.lib SDL.lib SDLmain.lib /nologo /subsystem:console /machine:I386 /nodefaultlib

!ELSEIF  "$(CFG)" == "Anise - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /Zm200 /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msvcrtd.lib winmm.lib msvcprtd.lib SDL.lib SDLmain.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Anise - Win32 Release"
# Name "Anise - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "op"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\op_animate.cpp
# End Source File
# Begin Source File

SOURCE=.\op_call.cpp
# End Source File
# Begin Source File

SOURCE=.\op_callprocedure.cpp
# End Source File
# Begin Source File

SOURCE=.\op_clearscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\op_defineprocedure.cpp
# End Source File
# Begin Source File

SOURCE=.\op_delay.cpp
# End Source File
# Begin Source File

SOURCE=.\op_displaynumber.cpp
# End Source File
# Begin Source File

SOURCE=.\op_if.cpp
# End Source File
# Begin Source File

SOURCE=.\op_makearray.cpp
# End Source File
# Begin Source File

SOURCE=.\op_makebytearray.cpp
# End Source File
# Begin Source File

SOURCE=.\op_null.cpp
# End Source File
# Begin Source File

SOURCE=.\op_saveconstant.cpp
# End Source File
# Begin Source File

SOURCE=.\op_saveexpression.cpp
# End Source File
# Begin Source File

SOURCE=.\op_setbasevariable.cpp
# End Source File
# Begin Source File

SOURCE=.\op_setcolor.cpp
# End Source File
# Begin Source File

SOURCE=.\op_setdialoguecolor.cpp
# End Source File
# Begin Source File

SOURCE=.\op_utility.cpp
# End Source File
# Begin Source File

SOURCE=.\op_wait.cpp
# End Source File
# End Group
# Begin Group "op4"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\op4_blitdirect.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_blitmasked.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_blitswapped.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_break.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_callscript.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_changeslot.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_checkclick.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_continue.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_displayselection.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_drawinversebox.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_drawsolidbox.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_dummy.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_field.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_initializeselection.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_jumpscript.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_loadfile.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_loadimage.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_manipulateflag.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_palette.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_sound.cpp
# End Source File
# Begin Source File

SOURCE=.\op4_while.cpp
# End Source File
# End Group
# Begin Group "oput"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\oput_dummy.cpp
# End Source File
# Begin Source File

SOURCE=.\oput_overlapscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\oput_sprayscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\oput_swapscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\oput_unpackaniheader.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\animation.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogue.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogue_gamebox.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogue_jis.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogue_jishan.cpp
# End Source File
# Begin Source File

SOURCE=.\engine.cpp
# End Source File
# Begin Source File

SOURCE=.\field.cpp
# End Source File
# Begin Source File

SOURCE=.\field_move.cpp
# End Source File
# Begin Source File

SOURCE=.\field_path.cpp
# End Source File
# Begin Source File

SOURCE=.\file.cpp
# End Source File
# Begin Source File

SOURCE=.\image.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\memory.cpp
# End Source File
# Begin Source File

SOURCE=.\memory_block.cpp
# End Source File
# Begin Source File

SOURCE=.\memory_segment.cpp
# End Source File
# Begin Source File

SOURCE=.\opl3.cpp
# End Source File
# Begin Source File

SOURCE=.\option.cpp
# End Source File
# Begin Source File

SOURCE=.\script.cpp
# End Source File
# Begin Source File

SOURCE=.\script_opre.cpp
# End Source File
# Begin Source File

SOURCE=.\script_parameter.cpp
# End Source File
# Begin Source File

SOURCE=.\script_stack.cpp
# End Source File
# Begin Source File

SOURCE=.\script_value.cpp
# End Source File
# Begin Source File

SOURCE=.\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\video.cpp
# End Source File
# Begin Source File

SOURCE=.\ymf262.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\animation.h
# End Source File
# Begin Source File

SOURCE=.\anise.h
# End Source File
# Begin Source File

SOURCE=.\begin_code.h
# End Source File
# Begin Source File

SOURCE=.\close_code.h
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\dialogue.h
# End Source File
# Begin Source File

SOURCE=.\engine.h
# End Source File
# Begin Source File

SOURCE=.\field.h
# End Source File
# Begin Source File

SOURCE=.\file.h
# End Source File
# Begin Source File

SOURCE=.\image.h
# End Source File
# Begin Source File

SOURCE=.\input.h
# End Source File
# Begin Source File

SOURCE=.\memory.h
# End Source File
# Begin Source File

SOURCE=.\memory_block.h
# End Source File
# Begin Source File

SOURCE=.\memory_index.h
# End Source File
# Begin Source File

SOURCE=.\memory_segment.h
# End Source File
# Begin Source File

SOURCE=.\opl3.h
# End Source File
# Begin Source File

SOURCE=.\option.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\script.h
# End Source File
# Begin Source File

SOURCE=.\script_code.h
# End Source File
# Begin Source File

SOURCE=.\script_parameter.h
# End Source File
# Begin Source File

SOURCE=.\script_stack.h
# End Source File
# Begin Source File

SOURCE=.\sound.h
# End Source File
# Begin Source File

SOURCE=.\timer.h
# End Source File
# Begin Source File

SOURCE=.\video.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\anise.rc
# End Source File
# End Group
# End Target
# End Project
