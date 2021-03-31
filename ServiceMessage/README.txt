MCファイルのビルド方法に関しては、以下を参照した。

https://www.ipentec.com/document/windows-create-message-file-dll

このサービスサンプル自体は、Microsoftの以下のサイトを参考にした。

https://docs.microsoft.com/en-us/windows/win32/services/the-complete-service-sample

Building the Service
The following procedure describes how to build the service and register the event message DLL.

To build the service and register the event message DLL

Build the message DLL from Sample.mc using the following steps:

mc -U sample.mc
rc -r sample.rc
link -dll -noentry -out:sample.dll sample.res
Build Svc.exe, SvcConfig.exe, and SvcControl.exe from Svc.cpp, SvcConfig.cpp, and SvcControl.cpp, respectively.

Create the registry key HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\EventLog\Application\SvcName and add the following registry values to this key.

TABLE 1
Value	Type	Description
EventMessageFile = dll_path	REG_SZ	The path to the resource-only DLL that contains strings that the service can write to the event log.
TypesSupported = 0x00000007	REG_DWORD	A bit mask that specifies the supported event types. The value 0x000000007 indicates that all types are supported.



------------------------------------
Windows Registry Editor Version 5.00

[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\EventLog\Application\ServiceApplication1]
"CategoryCount"=dword:00000003
"CategoryMessageFile"="C:\\Users\\User01\\source\\repos\\ConsoleSolution\\x64\\Debug\\ServiceLibrary.dll"
"EventMessageFile"="C:\\Users\\User01\\source\\repos\\ConsoleSolution\\x64\\Debug\\ServiceLibrary.dll"
"TypesSupported"=dword:00000007
