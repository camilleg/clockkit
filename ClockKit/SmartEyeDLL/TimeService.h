/**********************************************
* $Id: TimeService.h,v 1.1 2003/11/26 00:25:49 kowitz Exp $
*
* $Log: TimeService.h,v $
* Revision 1.1  2003/11/26 00:25:49  kowitz
* SmartEye DLL project for providing timing information to Smart Eye Pro
*
* Revision 1.6  2003/10/21 10:18:54  pers
* added some more comments
*
* Revision 1.5  2003/10/18 14:30:50  pers
* adjusted interface to better suit dynamic loading
*
* Revision 1.4  2003/10/17 13:56:13  pers
* UTC implementation complete (but untested)
*
* Revision 1.3  2003/10/17 10:57:03  pers
* added standard Smart Eye CVS headers
*
**********************************************
* (C) Copyright 2003 Smart Eye AB
**********************************************/


#ifdef BUILDING_TIMESERVICE
#define TIMESERVICE_API(NAME) int NAME
#else
#define TIMESERVICE_API(NAME) typedef int (*fptr_##NAME)
#endif


#if defined(__cplusplus)
extern "C" {
#endif

  // types
  // -----

  // opaque handle to the time service
  // it may be used to identify a client and/or refer to private data structures
  typedef void * TTimeServiceHandle;

  // user time is stored in a 64 bit opaque data structure
  typedef unsigned __int64 TOpaqueUserTime;


  // functions
  // ---------
  // all functions return 1 on success, 0 on failure

  // start and possibly synchronize the time service, allocate any resources needed
  // this function is called exactly once (per client) before any other calls are made from that client
  TIMESERVICE_API(startTimeService) (
    TTimeServiceHandle * handle);   // [out] time service handle

  // stop time service and release resources
  // this function is called exactly once (per client) after all other calls have been made
  TIMESERVICE_API(stopTimeService) (
    TTimeServiceHandle handle);     // [in] time service handle

  // get the current time in user format
  // to the result sholud be added a time offset in order to get the actual time of a past or future event
  // this function might be called from time-critical sections of the code,
  // and should avoid potentially blocking calls and return as soon as possible
  TIMESERVICE_API(getCurrentTime) (
    TTimeServiceHandle handle,      // [in] time service handle
    signed __int64 offset,          // [in] offset in 100 ns units
    TOpaqueUserTime * currentTime); // [out] current time in user format, adjusted by offset

  // convert a user format timestamp to a human readable text format
  // the text format is intended for presentation and not data logging, so a one-to-one mapping is not requiredu
  TIMESERVICE_API(timeToString) (
    TTimeServiceHandle handle,      // [in] time service handle
    TOpaqueUserTime currentTime,    // [in] time in user format
    unsigned long * bufferSize,     // [in/out] size of text buffer in bytes incl null termination (in = max, out = actual)
    char * textBuffer);             // [out] time in text format (null terminated)


#if defined(__cplusplus)
} // extern "C"
#endif
