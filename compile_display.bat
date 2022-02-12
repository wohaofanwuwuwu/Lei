@echo off
mkdir client >NUL 2>NUL
pushd client
@set LIBS=User32.lib Gdi32.lib ws2_32.lib
cl -nologo -Zi -EHsc ..\player.cpp  /link %LIBS%
popd