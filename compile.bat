@echo off

rem set COMPILATION_FLAGS=/std:c11 /O1 /GL /favor:blend /utf-8 /validate-charset /EHsc
rem set WARNING_FLAGS=/W3 /sdl
rem set SUBSYSTEM_FLAGS=/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup

set "raylib=C:\Users\Nathan\Documents\GitHub\subsynth\raylib-5.0\src"
set "raygui=C:\Users\Nathan\Documents\GitHub\subsynth\raygui-4.0"
set "SOURCE=C:\Users\Nathan\Documents\GitHub\subsynth\src\main.c"
set "RAYLIB_OBJ=C:\Users\Nathan\Documents\GitHub\subsynth\target\raylib"
set "ROOT_DIR=%CD%"
set "GAME_NAME=subsynth.exe"

:READ_ARGS
REM For the ! variable notation
setlocal EnableDelayedExpansion
REM For shifting, which the command line argument parsing needs
setlocal EnableExtensions

IF NOT EXIST !RAYLIB_OBJ!\ (
  mkdir "C:\Users\Nathan\Documents\GitHub\subsynth\target\raylib"
  cd !raylib_obj!
  REM raylib source folder
  set "RAYLIB_DEFINES=/D_DEFAULT_SOURCE /DPLATFORM_DESKTOP /DGRAPHICS_API_OPENGL_33"
  set RAYLIB_C_FILES="!raylib!\rcore.c" "!raylib!\rshapes.c" "!raylib!\rtextures.c" "!raylib!\rtext.c" "!raylib!\rmodels.c" "!raylib!\utils.c" "!raylib!\raudio.c" "!raylib!\rglfw.c"
  set RAYLIB_INCLUDE_FLAGS=/I"!raylib!" /I"!raylib!\external\glfw\include"

  cl.exe /nologo /w /c !RAYLIB_DEFINES! !RAYLIB_INCLUDE_FLAGS! !COMPILATION_FLAGS! !RAYLIB_C_FILES! || exit /B
)

cd "!ROOT_DIR!\target\debug"

cl.exe /nologo /std:c11 /Od /Zi /utf-8 /validate-charset /EHsc /Fe"!GAME_NAME!" /I"!raylib!" /I"!raygui!" "!SOURCE!" /link kernel32.lib user32.lib shell32.lib winmm.lib gdi32.lib opengl32.lib "!RAYLIB_OBJ!\*.obj"

del *.obj

!GAME_NAME!

cd "!ROOT_DIR!"
