#ifndef NICESAT_SOLVERS_FILE_SOLVER
#define NICESAT_SOLVERS_FILE_SOLVER

#include "Solvers/Solver.H"
#include "Support/Shared.H"

#include <string>

#include <errno.h>
#include <sys/mman.h>
#include <cstring>

#include <cassert>

using namespace std;

#define WRITE_BUFFER_SIZE 1024
#define INT_BUFFER_SIZE 12     // maximum number of digits in an integer

namespace nicesat {
  class CnfExp;
  
  class FileSolver : public Solver {
  public:
    FileSolver(const string& filename);
    ~FileSolver();

    Answer runSolver() { return Answer_Unknown; }
    Assignment deref(Literal lit) { return Assignment_Unknown; }
    Literal getNewVar() { return ++_varCount; }
    Literal getVar(Literal lit)   { assert(lit <= _varCount); return lit; }
    void reserveLit(Literal lit) {
      assert(_varCount == 0);
      _varCount = lit;
    }

    void add(const CnfExp& exp);
    void add(Literal lit) {
      ++_clauseCount;
      _putint(lit);
      _putc(' ');
      _putc('0');
      _putc('\n');
    }
    void constrain(Literal lit, const CnfExp& exp);
    
  private:
    inline void _putint(int n)  {
      if (n == 0) { _putc('0'); return; }

      static char buffer[INT_BUFFER_SIZE];
      
      if (n < 0) { _putc('-'); n *= -1; }
      
      size_t index = INT_BUFFER_SIZE - 1;
      while (n > 0) {
        buffer[index--] = '0' + (n % 10);
        n /= 10;
      }

      size_t numLen = INT_BUFFER_SIZE - index - 1;
      if (_bPos + numLen > _pgsz) {
        size_t fstChunk = _pgsz - _bPos;
        memcpy (_buff + _bPos, buffer + index + 1, fstChunk);

        if (munmap(_buff, _pgsz) != 0) errorf("%s\n", strerror(errno));
        if (lseek(_file, (++_bOff + 1) * _pgsz, SEEK_SET) == -1)
          errorf("%s: Error calling lseek to stretch the file.\n", strerror(errno));
        if (write(_file, "", 1) == -1)
          errorf("%s: Error writing last byte to the file.\n", strerror(errno));
        _buff = (char*) mmap(NULL, _pgsz, PROT_WRITE, MAP_SHARED , _file, _bOff * _pgsz);
        if (_buff == MAP_FAILED) errorf("%s\n", strerror(errno));
        _bPos = 0;

        memcpy(_buff, buffer + index + fstChunk + 1, numLen - fstChunk);
        _bPos = numLen - fstChunk;
      } else {
        memcpy(_buff + _bPos, buffer + index + 1, numLen);
        _bPos += numLen;
      }
    }

    inline void _putc(char c) {
      if (_bPos == _pgsz) {
        if (munmap(_buff, _pgsz) != 0) errorf("%s\n", strerror(errno));
        if (lseek(_file, (++_bOff + 1) * _pgsz, SEEK_SET) == -1)
          errorf("%s: Error calling lseek to stretch the file.\n", strerror(errno));
        if (write(_file, "", 1) == -1)
          errorf("%s: Error writing last byte to the file.\n", strerror(errno));
        _buff = (char*) mmap(NULL, _pgsz, PROT_WRITE, MAP_SHARED , _file, _bOff * _pgsz);
        if (_buff == MAP_FAILED) errorf("%s\n", strerror(errno));
        _bPos = 0;
      }

      _buff[_bPos++] = c;
    }

    const string&   _filename;
    int             _file;
    size_t          _pgsz;
    size_t          _bPos;
    size_t          _bOff;
    char*           _buff;

    Literal         _varCount;
    int             _clauseCount;
  };
}

#endif//NICESAT_SOLVERS_FILE_SOLVER
