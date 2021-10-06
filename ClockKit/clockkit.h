#ifdef SWIG
%module clockkit
%{
#include "clockkit.h"
%}
%include <stdint.i>
%include <typemaps.i>
%apply int64_t& INOUT { int64_t& idx };
#endif

#include <stdint.h>  // for int64_t

extern void ckInitialize(const char*);
extern void ckTerminate();
extern int64_t ckTimeAsValue();
extern const char* ckTimeAsString();
extern bool ckInSync();
extern int64_t ckOffset();
extern void ckKill();
