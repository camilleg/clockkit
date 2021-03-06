#ifdef SWIG
%module clockkit
%{
#include "clockkit.h"
%}
#endif

#include "Timestamp.h"

extern void ckInitialize(const char*);
extern void ckTerminate();
extern dex::timestamp_t ckTimeAsValue();
extern const char* ckTimeAsString();
extern bool ckInSync();
extern int ckOffset();
extern void ckKill();
