/**********************************************
* $Id: TimeService.cpp,v 1.1 2003/11/26 00:25:49 kowitz Exp $
*
* $Log: TimeService.cpp,v $
* Revision 1.1  2003/11/26 00:25:49  kowitz
* SmartEye DLL project for providing timing information to Smart Eye Pro
*
* Revision 1.7  2003/10/23 15:38:27  pers
* added special case for time value 0, which is converted into an empty string
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


#include "stdafx.h"
#include "TimeService.h"

#include "../clockkit.h"

#if defined(__cplusplus)
extern "C" {
#endif


BOOL APIENTRY 
DllMain( 
  HANDLE hModule, 
  DWORD  ul_reason_for_call, 
  LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}

// magic signature used to validate private data block
const long MAGIC_SIGNATURE = 0x7715ABBA;

struct TPrivateData
{
  long magic;
  TIME_ZONE_INFORMATION timeZoneInformation;
};


inline TPrivateData * 
checkHandle(TTimeServiceHandle handle)
{
  TPrivateData * pPrivateData = (TPrivateData*)handle;
  if (pPrivateData && (MAGIC_SIGNATURE == pPrivateData->magic))
  {
    return pPrivateData;
  } 
  return 0;
}


TIMESERVICE_API(startTimeService) (
  TTimeServiceHandle * handle)
{
  // default return value is empty handle
  *handle = 0;

  // allocate a block of memory from Win32
  TPrivateData * pPrivateData = (TPrivateData *)GlobalAlloc(GPTR, sizeof(TPrivateData));
  if (!pPrivateData)
  {
    // failure
    return 0;
  }

  // mark it with the magic signature
  pPrivateData->magic = MAGIC_SIGNATURE;

  // get timezone information
  DWORD stat = GetTimeZoneInformation(&pPrivateData->timeZoneInformation);
  if (stat == TIME_ZONE_ID_INVALID)
  {
    // failure
    GlobalFree(pPrivateData);
    return 0;
  }

  ckInitialize();

  // return handle
  *handle = pPrivateData;

  // success
  return 1;
}


TIMESERVICE_API(stopTimeService) (
  TTimeServiceHandle handle)
{
  // return memory block to Win32
  TPrivateData * verifiedHandle = checkHandle(handle);
  if (verifiedHandle)
  {
    verifiedHandle->magic = 0; // invalidate
    GlobalFree(handle);
  }
  
  // report success (whatever happened)
  return 1;
}


TIMESERVICE_API(getCurrentTime) (
  TTimeServiceHandle handle,
  signed __int64 offset,
  TOpaqueUserTime * currentTime)
{
  if (!currentTime)
  {
    // bad parameters, fail
    return 0;
  }
/*
  // get current UTC time
  FILETIME systemTimeAsFileTime;
  GetSystemTimeAsFileTime(&systemTimeAsFileTime);

  // adjust for offset
  *((unsigned __int64 *)currentTime) = *((unsigned __int64 *)&systemTimeAsFileTime) + offset;
*/
  const dex::timestamp_t usec = ckTimeAsValue();
  *((dex::timestamp_t *)currentTime) = usec + offset/10;
  // success
  return 1;
}


TIMESERVICE_API(timeToString) (
  TTimeServiceHandle handle,
  TOpaqueUserTime currentTime,
  unsigned long * bufferSize,
  char * textBuffer)
{
  TPrivateData * pPrivateData = checkHandle(handle);
  if (!pPrivateData || !bufferSize || !textBuffer || !*bufferSize)
  {
    // bad parameters, fail
    return 0;
  }

  const std::string foo = dex::Timestamp::timestampToString((dex::timestamp_t)currentTime);
  // utterly lazy and dangerous
  strcpy(textBuffer, foo.c_str());
  *bufferSize = strlen(textBuffer);
  return 1;
#if 0
  // the rest of this function is smarteye's default implementation.


  // default result = empty string
  *textBuffer = 0;

  // we treat 0 as a special case that just gets converted into an empty string
  if (currentTime == 0)
  {
    *bufferSize = 1;
    return 1;
  }

  // convert UTC time to local time
  const FILETIME utcFileTime = *(FILETIME *)(&currentTime);
  FILETIME localFileTime;
  BOOL stat = FileTimeToLocalFileTime(&utcFileTime, &localFileTime);
  if (stat == 0)
  {
    // failed conversion
    *bufferSize = 1;
    return 0;
  }

  // convert local time to local system time format
  SYSTEMTIME localSystemTime;
  stat = FileTimeToSystemTime(&localFileTime, &localSystemTime);
  if (stat == 0)
  {
    // failed conversion
    *bufferSize = 1;
    return 0;
  }

  // convert to text format YYYY-MM-DD hh:mm:ss.sss [TZID]
  unsigned int theoreticalSize = 
    4+1+2+1+2 +1 +2+1+2+1+2+1+3 +1 +1+wcslen(pPrivateData->timeZoneInformation.StandardName)+1 +1;
  if (theoreticalSize > *bufferSize)
  {
    // not room enough for result
    *bufferSize = 1;
    return 0;
  }
  unsigned int actualSize = wsprintf(
    textBuffer, "%04d-%02d-%02d %02d:%02d:%02d.%03d [%ls]",
    localSystemTime.wYear, localSystemTime.wMonth, localSystemTime.wDay,
    localSystemTime.wHour, localSystemTime.wMinute, localSystemTime.wSecond,
    localSystemTime.wMilliseconds,
    pPrivateData->timeZoneInformation.StandardName) + 1;
  *bufferSize = actualSize;

  // success
  return 1;
#endif
}


#if defined(__cplusplus)
} // extern "C"
#endif
