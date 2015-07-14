#ifndef NICESAT_SUPPORT_SHARED_H_
#define NICESAT_SUPPORT_SHARED_H_

#include<stdlib.h>
#include<stdio.h>
#include<stdarg.h>
#include<cassert>
#include<stdint.h>

// Some GNUC extensions:
//
#ifdef __GNUC__
#define ___noreturn    __attribute__ ((__noreturn__))
#define ___unused      __attribute__ ((__unused__))
#define ___const       __attribute__ ((__const__))
#define ___format(type,fmt,arg) __attribute__ ((__format__(type, fmt, arg) ))
#if (__GNUC__ > 2) || (__GNUC__ >= 2 && __GNUC_MINOR__ > 95)
#define ___malloc      __attribute__ ((__malloc__))
#else
#define ___malloc
#endif
#else
#define ___noreturn
#define ___unused
#define ___const
#define ___format(type,fmt,arg)
#define ___malloc
#endif

////////////////////////////////////////////////////////////////////////////////
// Some definitions of printf like functions, useful for debugging
char*  nsprintf(const char* format, ...) ___format(printf, 1, 2) ___malloc;
char* vnsprintf(const char* format, va_list args)                ___malloc;

int shellf(const char* format, ...) ___format(printf, 1, 2);

void reportf(const char* format, ...)  ___format(printf, 1, 2);
void errorf(const char* format, ...)   ___format(printf, 1, 2);

#ifndef NDEBUG
#define debugf(...) reportf(__VA_ARGS__)
#else
#define debugf(...)
#endif

////////////////////////////////////////////////////////////////////////////////
// Timing and Memory Usage (from MiniSat)
#ifdef _MSC_VER
#include <ctime>

static inline double cpuTime(void) {
    return (double)clock() / CLOCKS_PER_SEC; }
#else

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

static inline double cpuTime(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000; }
#endif

#endif//NICESAT_SUPPORT_SHARED_H_
