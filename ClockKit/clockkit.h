#ifdef SWIG
%module clockkit
%{
#include "clockkit.h"
%}
#endif

#include "Common.h"

extern void ckInitialize();
   extern dex::timestamp_t ckTimeAsValue();
extern const char* ckTimeAsString();
extern bool ckInSync();
extern int ckOffset();
