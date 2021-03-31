@ECHO OFF

SET PATH=%PATH%;"C:\Program Files (x86)\Windows Kits\10\bin\10.0.19041.0\x64"
SET PATH=%PATH%;"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.28.29910\bin\Hostx86\x86"

DEL MSG00409.bin
DEL MSG00411.bin
DEL Sample.h
DEL Sample.rc

MC.EXE Sample.mc
RC.EXE Sample.rc
LINK.EXE /DLL /NOENTRY /MACHINE:x64 Sample.res

COPY Sample.dll "C:\Users\User01\source\repos\ConsoleSolution\x64\Debug"

ECHO Done.