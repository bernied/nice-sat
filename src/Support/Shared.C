#include "Support/Shared.H"

#include <cassert>
#include <cstdlib>

char* vnsprintf(const char* format, va_list args)
{
    static FILE* dummy = fopen("/dev/null", "wb");
    unsigned     chars_written;
    char*        ret;
    va_list      args_copy;

#ifdef __va_copy
    __va_copy (args_copy, args);
#else
    args_copy = args;
#endif
    chars_written = vfprintf(dummy, format, args);
    ret = (char*) malloc(sizeof(char) * (chars_written + 1));
    ret[chars_written] = 255;
    args = args_copy;
    vsprintf(ret, format, args);
    assert(ret[chars_written] == 0);
    return ret;
}

char* nsprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char* ret = vnsprintf(format, args);
    va_end(args);
    return ret;
}

int shellf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  char* cmd = vnsprintf(format, args);
  va_end(args);

  int ans = system(cmd);
  free(cmd);
  return ans;
}

void errorf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  char* text = vnsprintf(format, args);
  va_end(args);


  fprintf(stderr, "Error: %s\n", text);
  free(text);
  exit(1);
}

void reportf(const char* format, ...) {
  static bool col0 = true;
  
  va_list args;
  va_start(args, format);
  char* text = vnsprintf(format, args);
  va_end(args);
  
  for(char* p = text; *p != 0; p++){
    if (col0)
      putchar('c'), putchar(' ');
    
    putchar(*p);
    col0 = (*p == '\n' || *p == '\r');
  }
  fflush(stdout);
  free(text);
}
