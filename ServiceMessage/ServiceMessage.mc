SeverityNames=(
    Success=0x0:STATUS_SEVERITY_SUCCESS
    Information=0x1:STATUS_SEVERITY_INFORMATION
    Warning=0x2:STATUS_SEVERITY_WARNING
    Errors=0x3:STATUS_SEVERITY_ERROR
)

FacilityNames=(
    System=0x0:FACILITY_SYSTEM
    Runtime=0x1:FACILITY_RUNTIME
    API=0x2:FACILITY_API
    IO=0x3:FACILITY_IO
    Trace=0xF:FACILITY_TRACE
)


LanguageNames=(English=0x409:MSG00409)
LanguageNames=(Japanese=0x411:MSG00411)


MessageIdTypedef=WORD

MessageId=0x1
SymbolicName=CATEGORY_SERVICE_CORE
Language=English
ServiceCore
.

Language=Japanese
ServiceCoreクラス
.

MessageId=0x2
SymbolicName=CATEGORY_SERVICE_MANAGER
Language=English
ServiceManager
.

Language=Japanese
ServiceManagerクラス
.

MessageId=0x3
SymbolicName=CATEGORY_SERVICE_CONTROL
Language=English
SerivceControl
.

Language=Japanese
ServiceControlクラス
.

MessageId=0x4
SymbolicName=CATEGORY_SERVICE
Language=English
Serivce
.

Language=Japanese
Serviceクラス
.

;
;// --------------
;// MESSAGE DEFINE
;// --------------
;

MessageIdTypedef=DWORD

;
;// --------------
;// SUCCESS SYSTEM
;// --------------
;

MessageId=0x0000
Severity=Success
Facility=System
SymbolicName=SVC_SUCCESS_SYSTEM
Language=English
System succeed.%r
%1 %2 %3
.

Language=Japanese
システムが成功しました。%r
%1 %2 %3
.

;
;// ---------------
;// SUCCESS RUNTIME
;// ---------------
;

MessageId=0x0000
Severity=Success
Facility=Runtime
SymbolicName=SVC_SUCCESS_RUNTIME
Language=English
Runtime succeed.%r
%1 %2 %3
.

Language=Japanese
ランタイムが成功しました。%r
%1 %2 %3
.

;
;// -----------
;// SUCCESS API
;// -----------
;

MessageId=0x0000
Severity=Success
Facility=API
SymbolicName=SVC_SUCCESS_API
Language=English
API succeed.%r
%1 %2 %3
.

Language=Japanese
APIが成功しました。%r
%1 %2 %3
.

;
;// ----------
;// SUCCESS IO
;// ----------
;

MessageId=0x0000
Severity=Success
Facility=IO
SymbolicName=SVC_SUCCESS_IO
Language=English
IO succeed.%r
%1 %2 %3
.

Language=Japanese
IOが成功しました。%r
%1 %2 %3
.

;
;// ------------------
;// INFORMATION SYSTEM
;// ------------------
;

MessageId=0x0000
Severity=Information
Facility=System
SymbolicName=SVC_INFO_SYSTEM
Language=English
System information.%r
%1 %2 %3
.

Language=Japanese
システム情報です。%r
%1 %2 %3
.

;
;// -------------------
;// INFORMATION RUNTIME
;// -------------------
;

MessageId=0x0000
Severity=Information
Facility=Runtime
SymbolicName=SVC_INFO_RUNTIME
Language=English
Runtime information.%r
%1 %2 %3
.

Language=Japanese
ランタイム情報です。%r
%1 %2 %3
.

;
;// ---------------
;// INFORMATION API
;// ---------------
;

MessageId=0x0000
Severity=Information
Facility=API
SymbolicName=SVC_INFO_API
Language=English
API information.%r
%1 %2 %3
.

Language=Japanese
API情報です。%r
%1 %2 %3
.

;
;// --------------
;// INFORMATION IO
;// --------------
;

MessageId=0x0000
Severity=Information
Facility=IO
SymbolicName=SVC_INFO_IO
Language=English
IO information.%r
%1 %2 %3
.

Language=Japanese
IO情報です。%r
%1 %2 %3
.

;
;// -----------------
;// INFORMATION TRACE
;// -----------------
;

MessageId=0xFFFF
Severity=Information
Facility=Trace
SymbolicName=SVC_INFO_TRACE
Language=English
Trace information.%r
%1 %2 %3
.

Language=Japanese
トレース情報です。%r
%1 %2 %3
.

;
;// --------------
;// WARNING SYSTEM
;// --------------
;

MessageId=0x0000
Severity=Warning
Facility=System
SymbolicName=SVC_WARNING_SYSTEM
Language=English
System warning occurred.%r
%1 %2 %3
.

Language=Japanese
システム警告が発生しました。%r
%1 %2 %3
.

;
;// ---------------
;// WARNING RUNTIME
;// ---------------
;

MessageId=0x0000
Severity=Warning
Facility=Runtime
SymbolicName=SVC_WARNING_RUNTIME
Language=English
Runtime warning occurred.%r
%1 %2 %3
.

Language=Japanese
ランタイム警告が発生しました。%r
%1 %2 %3
.

;
;// -----------
;// WARNING API
;// -----------
;

MessageId=0x0000
Severity=Warning
Facility=API
SymbolicName=SVC_WARNING_API
Language=English
API warning occurred.%r
%1 %2 %3
.

Language=Japanese
API警告が発生しました。%r
%1 %2 %3
.

;
;// ----------
;// WARNING IO
;// ----------
;

MessageId=0x0000
Severity=Warning
Facility=IO
SymbolicName=SVC_WARNING_IO
Language=English
IO warning occurred.%r
%1 %2 %3
.

Language=Japanese
IO警告が発生しました。%r
%1 %2 %3
.

;
;// ------------
;// ERROR SYSTEM
;// ------------
;

MessageId=0x0000
Severity=Error
Facility=System
SymbolicName=SVC_ERROR_SYSTEM
Language=English
System error occurred.%r
%1 %2 %3
.

Language=Japanese
システムエラーが発生しました。%r
%1 %2 %3
.

;
;// -------------
;// ERROR RUNTIME
;// -------------
;

MessageId=0x0000
Severity=Error
Facility=Runtime
SymbolicName=SVC_ERROR_RUNTIME
Language=English
Runtime error occurred.%r
%1 %2 %3
.

Language=Japanese
ランタイムエラーが発生しました。%r
%1 %2 %3
.

;
;// ---------
;// ERROR API
;// ---------
;

MessageId=0x0000
Severity=Error
Facility=API
SymbolicName=SVC_ERROR_API
Language=English
API error occurred.%r
%1 %2 %3
.

Language=Japanese
APIエラーが発生しました。%r
%1 %2 %3
.

;
;// --------
;// ERROR IO
;// --------
;

MessageId=0x0000
Severity=Error
Facility=IO
SymbolicName=SVC_ERROR_IO
Language=English
IO error occurred.%r
%1 %2 %3
.

Language=Japanese
IOエラーが発生しました。%r
%1 %2 %3
.
