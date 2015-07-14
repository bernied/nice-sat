#ifndef NICESAT_SUPPORT_OPTIONS_H_
#define NICESAT_SUPPORT_OPTIONS_H_

#include <cstring>
#include "ADTs/Vec.H"

namespace nicesat {
  struct Flag { bool& present; const char** value; char shortOpt; const char* longOpt; const char* desc;
    Flag(bool& _present, const char** _value, char _short, const char* _long, const char* _desc)
      : present(_present), value(_value), shortOpt(_short), longOpt(_long), desc(_desc) { present = false; }
  };

  class Options {
  private:
    Vec<Flag*> _options;
    Flag*      _shortOpts[256];
    size_t     _longLen;

  public:
    Options() : _longLen(0) { memset(_shortOpts, 0, sizeof(Flag*) * 256); }
    ~Options();

    void addFlag(bool& present, const char** value, char shortOpt, const char* longOpt, const char* desc);

    void addFlag(bool& present, char shortOpt, const char* longOpt, const char* desc) {
      addFlag(present, NULL, shortOpt, longOpt, desc);
    }
    
    void addFlag(bool& present, char shortOpt, const char* desc) {
      addFlag(present, NULL, shortOpt, NULL, desc);
    }

    void addFlag(bool& present, const char** value, char shortOpt, const char* desc) {
      addFlag(present, value, shortOpt, NULL, desc);
    }

    void addFlag(bool& present, const char** value, const char* longOpt, const char* desc) {
      addFlag(present, value, '\0', longOpt, desc);
    }

    void printUsage(const char* prog, const char* argDescs) const;
    bool parse(int argc, const char** argv, int& lastArg);
  };
};

#endif//NICESAT_SUPPORT_OPTIONS_H_
