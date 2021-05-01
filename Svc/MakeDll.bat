@ECHO OFF

SET MC_EXE="C:\Program Files (x86)\Windows Kits\10\bin\10.0.19041.0\x86\mc.exe"
SET RC_EXE="C:\Program Files (x86)\Windows Kits\10\bin\10.0.19041.0\x86\rc.exe"
SET LINK_EXE="c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.28.29910\bin\Hostx64\x86\link.exe"
SET MODULE_NAME="Sample"
SET OutDir="C:\Users\User01\source\repos\ServiceSolution\x64\Debug"

if exist MSG00409.bin (
  DEL MSG00409.bin
)

if exist %MODULE_NAME%.h (
  DEL %MODULE_NAME%.h
)

if exist %MODULE_NAME%.rc (
  DEL %MODULE_NAME%.rc
)

if exist %MODULE_NAME%.res (
  DEL %MODULE_NAME%.res
)

%MC_EXE% -U %MODULE_NAME%.mc
%RC_EXE% -r  %MODULE_NAME%.rc
%LINK_EXE% -dll -noentry -machine:x86 -out:%MODULE_NAME%.dll %MODULE_NAME%.res

XCOPY "%MODULE_NAME%.dll" %OutDir% /Y

ECHO Done.
