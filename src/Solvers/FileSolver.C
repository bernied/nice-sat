#include "Solvers/FileSolver.H"

#include <fcntl.h>

#include "ADTs/CnfExp.H"
#include "Support/Shared.H"

#include <unistd.h>

#if _POSIX_MAPPED_FILES == 0
#error "This program requires mmap.  This can be corrected by providing appropriate alternatives in src/Solver/FileSolver.{C,H}"
#endif

namespace nicesat {
  FileSolver::FileSolver(const string& filename)
    : _filename(filename), _file(0), _bPos(0), _bOff(0), _varCount(0), _clauseCount(0) {
    _file = open(_filename.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    _pgsz = sysconf(_SC_PAGE_SIZE);
    
    if (lseek(_file, (_bOff + 1) * _pgsz, SEEK_SET) == -1)
      errorf("%s: Error calling lseek to stretch the file.\n", strerror(errno));
    if (write(_file, "", 1) == -1)
      errorf("%s: Error writing last byte to the file.\n", strerror(errno));
    
    _buff = (char*) mmap(NULL, _pgsz, PROT_WRITE, MAP_SHARED, _file, 0);
    if (_buff == MAP_FAILED) errorf("%s\n", strerror(errno));
    
    _bOff = 0; _bPos = 0;
    
    for (int i = 0; i < 53; i++) _buff[_bPos++] = ' ';
    _buff[_bPos++] = '\n';
  }
    

  FileSolver::~FileSolver() {
    if (ftruncate(_file, _bOff * _pgsz + _bPos) == -1)
      errorf("%s: Error calling ftruncate() to shorten the file.\n", strerror(errno));
    
    if (_bOff != 0) {
      if (munmap(_buff, _pgsz) != 0) errorf("%s\n", strerror(errno));
      _buff = (char*) mmap(NULL, _pgsz, PROT_WRITE, MAP_SHARED , _file, 0);
      if (_buff == MAP_FAILED) errorf("%s\n", strerror(errno));
    }

    _bPos = 0;
    _putc('p'); _putc(' '); _putc('c'); _putc('n'); _putc('f'); _putc(' ');
    _putint(_varCount);
    _putc(' ');
    _putint(_clauseCount);

    if (munmap(_buff, _pgsz) != 0) errorf("%s\n", strerror(errno));
    close(_file);
  }

  void FileSolver::add(const CnfExp& exp) {
    if (exp.alwaysFalse()) {
      Literal l = getNewVar();  add(l); add(-l); return;
    } else if (exp.alwaysTrue()) {
      return;
    }
    
    _clauseCount += exp.clauseSize();
    
    const LitVector& singletons = exp.singletons();
    for (int i = 0; i < singletons.size(); i++) {
      _putint(singletons[i]); _putc(' '); _putc('0'); _putc('\n');
    }

    CnfExp::Clause* iter = exp.head();
    while (iter) {
      const LitVector& clause = iter->_litVec;
      for (int i = 0; i < clause.size(); i++) {
        _putint(clause[i]); _putc(' ');
      }
      _putc('0'); _putc('\n');
      
      iter = iter->_next;
    }
  }

  void FileSolver::constrain(Literal lit, const CnfExp& exp) {
    if (exp.alwaysFalse()) {
      add(-lit);
      return;
    } else if (exp.alwaysTrue()) {
      return;
    }
    
    _clauseCount += exp.clauseSize();
    
    const LitVector& singletons = exp.singletons();
    for (int i = 0; i < singletons.size(); i++) {
      _putint(-lit);
      _putc(' ');
      _putint(singletons[i]);
      _putc(' '); _putc('0'); _putc('\n');
    }

    CnfExp::Clause* iter = exp.head();
    while (iter) {
      const LitVector& clause = iter->_litVec;
      _putint(-lit);
      _putc(' ');
      
      for (int i = 0; i < clause.size(); i++) {
        _putint(clause[i]); _putc(' ');
      }
      _putc('0'); _putc('\n');
      
      iter = iter->_next;
    }
  }
}
