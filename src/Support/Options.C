#include "Support/Options.H"
#include <stdio.h>
#include <ctype.h>

namespace nicesat {
  Options::~Options() {
    for (size_t i = 0; i < _options.size(); i++) {
      delete _options[i];
    }
  }
  
  void Options::addFlag(bool& present, const char** value, char shortOpt, const char* longOpt, const char* desc) {
    if (longOpt != NULL) {
      size_t longOptLen = strlen(longOpt);
      if (longOptLen > _longLen) _longLen = longOptLen;
    }
    
    _options.push(new Flag(present, value, shortOpt, longOpt, desc));
    if (isalnum(shortOpt)) {
      assert(_shortOpts[(int) shortOpt] == NULL);
      _shortOpts[(int) shortOpt] = _options.last();
    }
  }

  void Options::printUsage(const char* prog, const char* argDescs) const {
    printf("Usage: %s %s\n", prog, argDescs);

    char format[80];
    sprintf(format, "  %%s     %%s%%-%us     %%s\n", (unsigned int) _longLen);
    for (size_t i = 0; i < _options.size(); i++) {
      char shortDesc[3] = "  ";
      if (isalnum(_options[i]->shortOpt)) shortDesc[0] = '-', shortDesc[1] = _options[i]->shortOpt;
      char longPrefix[3] = "  ";
      if (_options[i]->longOpt != NULL) longPrefix[0] = '-', longPrefix[1] = '-';
      printf(format, shortDesc, longPrefix, _options[i]->longOpt, _options[i]->desc);
    }
  }

  bool Options::parse(int argc, const char** argv, int& lastArg) {
    for (lastArg = 1; lastArg < argc && *(argv[lastArg]) == '-'; lastArg++) {
      if (argv[lastArg][1] == '\0') {
        fprintf(stderr, "Error: Missing option after '-'\n");
        return true;
      } else if (argv[lastArg][1] == '-') {
        bool found = false;
        for (size_t i = 0; i < _options.size(); i++) {
          int optLen = strlen(_options[i]->longOpt);
          if (_options[i]->longOpt != NULL &&
              strncmp(argv[lastArg] + 2, _options[i]->longOpt, optLen) == 0) {
            _options[i]->present = true; found = true;
            
            if (_options[i]->value != NULL) {
              if (argv[lastArg][optLen + 2] == '=') {
                *_options[i]->value = (argv[lastArg] + optLen + 3);
              } else if (argv[lastArg][optLen + 2] == '\0') {
                *_options[i]->value = argv[++lastArg];
              }
            }

            break;
          }
        }
        
        if (!found) {
          fprintf(stderr, "Error: Invalid long option \"%s\"\n", argv[lastArg]);
          return true;
        }
      } else {
        for (int i = 1; argv[lastArg][i] != '\0'; i++) {
          int opt = (int) argv[lastArg][i];
          if (_shortOpts[opt] == NULL) {
            fprintf(stderr, "Error: Invalid short option '-%c'\n", argv[lastArg][i]);
            return true;
          }
          _shortOpts[opt]->present = true;
          if (_shortOpts[opt]->value != NULL) {
            if (argv[lastArg][i + 1] == '\0' && lastArg + 1 < argc && argv[lastArg + 1][0] != '-') {
              *_shortOpts[opt]->value = argv[++lastArg];
              break;            
            } else {
              *_shortOpts[opt]->value = NULL;
            }
          }
        }
      }
    }

    return false;
  }
}
