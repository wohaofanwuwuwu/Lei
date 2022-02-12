@echo off
mkdir obj >NUL 2>NUL
pushd obj
@set LIBS=User32.lib Gdi32.lib ws2_32.lib
cl -nologo -Zi -EHsc ..\screenshot.cpp ..\interframe.cpp ..\server.cpp  /link %LIBS%
popd