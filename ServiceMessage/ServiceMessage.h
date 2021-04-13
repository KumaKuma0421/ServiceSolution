//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_SYSTEM                  0x0
#define FACILITY_RUNTIME                 0x1
#define FACILITY_API                     0x2
#define FACILITY_IO                      0x3
#define FACILITY_TRACE                   0xF


//
// Define the severity codes
//
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_INFORMATION      0x1
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: CATEGORY_SEVICE_CORE
//
// MessageText:
//
// ServiceCore
//
#define CATEGORY_SEVICE_CORE             ((WORD)0x00000001L)

//
// MessageId: CATEGORY_SERVICE_MANAGER
//
// MessageText:
//
// ServiceManager
//
#define CATEGORY_SERVICE_MANAGER         ((WORD)0x00000002L)

//
// MessageId: CATEGORY_SERVICE_CONTROL
//
// MessageText:
//
// SerivceControl
//
#define CATEGORY_SERVICE_CONTROL         ((WORD)0x00000003L)

//
// MessageId: CATEGORY_SERVICE
//
// MessageText:
//
// Serivce
//
#define CATEGORY_SERVICE                 ((WORD)0x00000004L)


// --------------
// MESSAGE DEFINE
// --------------


// --------------
// SUCCESS SYSTEM
// --------------

//
// MessageId: SVC_SUCCESS_SYSTEM
//
// MessageText:
//
// System succeed.%r
// %1 %2 %3
//
#define SVC_SUCCESS_SYSTEM               ((DWORD)0x00000000L)


// ---------------
// SUCCESS RUNTIME
// ---------------

//
// MessageId: SVC_SUCCESS_RUNTIME
//
// MessageText:
//
// Runtime succeed.%r
// %1 %2 %3
//
#define SVC_SUCCESS_RUNTIME              ((DWORD)0x00010000L)


// -----------
// SUCCESS API
// -----------

//
// MessageId: SVC_SUCCESS_API
//
// MessageText:
//
// API succeed.%r
// %1 %2 %3
//
#define SVC_SUCCESS_API                  ((DWORD)0x00020000L)


// ----------
// SUCCESS IO
// ----------

//
// MessageId: SVC_SUCCESS_IO
//
// MessageText:
//
// IO succeed.%r
// %1 %2 %3
//
#define SVC_SUCCESS_IO                   ((DWORD)0x00030000L)


// ------------------
// INFORMATION SYSTEM
// ------------------

//
// MessageId: SVC_INFO_SYSTEM
//
// MessageText:
//
// System information.%r
// %1 %2 %3
//
#define SVC_INFO_SYSTEM                  ((DWORD)0x40000000L)


// -------------------
// INFORMATION RUNTIME
// -------------------

//
// MessageId: SVC_INFO_RUNTIME
//
// MessageText:
//
// Runtime information.%r
// %1 %2 %3
//
#define SVC_INFO_RUNTIME                 ((DWORD)0x40010000L)


// ---------------
// INFORMATION API
// ---------------

//
// MessageId: SVC_INFO_API
//
// MessageText:
//
// API information.%r
// %1 %2 %3
//
#define SVC_INFO_API                     ((DWORD)0x40020000L)


// --------------
// INFORMATION IO
// --------------

//
// MessageId: SVC_INFO_IO
//
// MessageText:
//
// IO information.%r
// %1 %2 %3
//
#define SVC_INFO_IO                      ((DWORD)0x40030000L)


// -----------------
// INFORMATION TRACE
// -----------------

//
// MessageId: SVC_INFO_TRACE
//
// MessageText:
//
// Trace information.%r
// %1 %2 %3
//
#define SVC_INFO_TRACE                   ((DWORD)0x400FFFFFL)


// --------------
// WARNING SYSTEM
// --------------

//
// MessageId: SVC_WARNING_SYSTEM
//
// MessageText:
//
// System warning occurred.%r
// %1 %2 %3
//
#define SVC_WARNING_SYSTEM               ((DWORD)0x80000000L)


// ---------------
// WARNING RUNTIME
// ---------------

//
// MessageId: SVC_WARNING_RUNTIME
//
// MessageText:
//
// Runtime warning occurred.%r
// %1 %2 %3
//
#define SVC_WARNING_RUNTIME              ((DWORD)0x80010000L)


// -----------
// WARNING API
// -----------

//
// MessageId: SVC_WARNING_API
//
// MessageText:
//
// API warning occurred.%r
// %1 %2 %3
//
#define SVC_WARNING_API                  ((DWORD)0x80020000L)


// ----------
// WARNING IO
// ----------

//
// MessageId: SVC_WARNING_IO
//
// MessageText:
//
// IO warning occurred.%r
// %1 %2 %3
//
#define SVC_WARNING_IO                   ((DWORD)0x80030000L)


// ------------
// ERROR SYSTEM
// ------------

//
// MessageId: SVC_ERROR_SYSTEM
//
// MessageText:
//
// System error occurred.%r
// %1 %2 %3
//
#define SVC_ERROR_SYSTEM                 ((DWORD)0xC0000000L)


// -------------
// ERROR RUNTIME
// -------------

//
// MessageId: SVC_ERROR_RUNTIME
//
// MessageText:
//
// Runtime error occurred.%r
// %1 %2 %3
//
#define SVC_ERROR_RUNTIME                ((DWORD)0xC0010000L)


// ---------
// ERROR API
// ---------

//
// MessageId: SVC_ERROR_API
//
// MessageText:
//
// API error occurred.%r
// %1 %2 %3
//
#define SVC_ERROR_API                    ((DWORD)0xC0020000L)


// --------
// ERROR IO
// --------

//
// MessageId: SVC_ERROR_IO
//
// MessageText:
//
// IO error occurred.%r
// %1 %2 %3
//
#define SVC_ERROR_IO                     ((DWORD)0xC0030000L)

