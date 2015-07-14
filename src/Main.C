#include "NiceSat.H"
#include "config.h"

#include <iostream>
#include <fstream>
using namespace std;

#include "Support/Options.H"
#include "NiceSat.H"
#include "Solvers/FileSolver.H"
#include "Solvers/PicoSatSolver.H"
using namespace nicesat;

#include <stdlib.h>

#include "Input/AigReader.H"

#if defined(__linux__)
static inline int memReadStat(int field)
{
    char    name[256];
    pid_t pid = getpid();
    sprintf(name, "/proc/%d/statm", pid);
    FILE*   in = fopen(name, "rb");
    if (in == NULL) return 0;
    int     value;
    for (; field >= 0; field--)
      fscanf(in, "%d", &value);
    fclose(in);
    return value;
}
static inline uint64_t memUsed() { return (uint64_t)memReadStat(0) * (uint64_t)getpagesize(); }

#elif defined(__FreeBSD__)
static inline uint64_t memUsed(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return ru.ru_maxrss*1024; }


#else
static inline uint64_t memUsed() { return 0; }
#endif

#ifndef NDEBUG
#define RETURN(n) return (n)
#else
#define RETURN(n) exit(n)
#endif

enum Filetype {
  UNRECOGNIZED              = -1,
  BINARY_AND_INVERTER_GRAPH = 0
};

bool recognizeFileType(const char* inputFile, string& filename, Filetype& type) {
  const char* dotPos   = strrchr(inputFile, '.');
  if (dotPos == NULL) errorf("Expected input name to have an extension!\n");

  filename = string(inputFile, dotPos - inputFile);
  if (strncmp(dotPos + 1, "aig", sizeof("aig")) == 0) {
    type = BINARY_AND_INVERTER_GRAPH;
  } else {
    type = UNRECOGNIZED;
  }
  
  return type != UNRECOGNIZED;
}

void postProcessGraphViz(const char* fileName, const char* name) {
  reportf("Running dot to make %s.%s.png\n", fileName, name);
  shellf("mv nicedag-%s.dot %s.%s.dot", name, fileName, name);
  int status = shellf("dot -Tpng -o%s.%s.png %s.%s.dot", fileName, name, fileName, name);
  if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
    shellf("rm %s.%s.dot", fileName, name);
  }
}

int main(int argc, const char** argv) {
  Options opt;

  bool printHelp    = false; 
  bool printVersion = false;
  bool printStats   = false;
  bool cnfOnly      = false;
  bool noMatching   = false;
  bool printGraph   = false;

  bool setSatSolver        = false;
  const char* satSolverStr = "picosat";

  opt.addFlag(printHelp,    'h', "help",        "Print this usage information");
  opt.addFlag(printVersion, 'v', "version",     "Print version information");
  opt.addFlag(printStats,   'n', "stats",       "Print some statistics about the NICE Dag");
  opt.addFlag(printGraph,   'p', "graphviz",    "Export the NICE dag using GraphViz");
  opt.addFlag(noMatching,   'm', "no-matching", "Disable matching");
  opt.addFlag(cnfOnly,      'c', "cnf-only",    "Produce CNF and stop");
  //  opt.addFlag(setSatSolver, &satSolverStr, 's', "solver", "Set which SAT solver to use (file|pico|mini|none)");
   opt.addFlag(setSatSolver, &satSolverStr, 's', "solver", "Set which SAT solver to use (file|pico|none)");

  int lastArg;
  if (opt.parse(argc, argv, lastArg)) printHelp = true;
  if (printVersion)
    {cout << PACKAGE_STRING << " built on " << __DATE__ << " at " << __TIME__ << endl;
      return(0);
    }
 

  enum SatSolver { SOLVER_NONE, SOLVER_FILE,
                   SOLVER_PICOSAT } satSolver = SOLVER_FILE;
  
  if (!printHelp && setSatSolver) {
    if (satSolverStr == NULL) {
      printHelp = true, fprintf(stderr, "Error: Need to SAT solver!\n");
    } else if (strncmp(satSolverStr, "file", 4) == 0) {
      satSolver = SOLVER_FILE;
    } else if (strncmp(satSolverStr, "none", 4) == 0) {
      satSolver = SOLVER_NONE;
    } else if (strncmp(satSolverStr, "picosat", 4) == 0) {
      satSolver = SOLVER_PICOSAT;
    } else {
      printHelp = true, fprintf(stderr, "Error: Unrecognized SAT solver \"%s\"\n", satSolverStr);
    }
  }

  const char* inputFile  = NULL;
  if (!printHelp && lastArg >= argc) {
    printHelp = true, fprintf(stderr, "Error: Need to specify input file!\n");
  }

  if (!printHelp && lastArg != argc) {
    inputFile = argv[lastArg++];
    assert(inputFile != NULL);
  }

  Filetype fileType = UNRECOGNIZED;
  string   fileName;
  if (!printHelp && !recognizeFileType(inputFile, fileName, fileType)) {
    errorf("Unrecognized input extension in filename \"%s\"\n", inputFile);
  }
  
  string outputFile;
  switch (satSolver) {
    case SOLVER_NONE:
      outputFile = "nowhere";
      break;
    case SOLVER_FILE:
      outputFile = fileName + ".cnf";
      break;
    case SOLVER_PICOSAT:
      outputFile = fileName + ".ans";
      break;
  }

  if (!printHelp && lastArg != argc) {
    outputFile = argv[lastArg++];
    if (lastArg != argc)
      printHelp = true, fprintf(stderr, "Error: Too many command line arguments!\n");
  }
 
  if (printVersion)
    cout << PACKAGE_STRING << " built on " << __DATE__ << " at " << __TIME__ << endl;
  
  if (printHelp)
    opt.printUsage(argv[0], "[options] <input> [<output>]");

  if (printHelp || printVersion) RETURN(0);

  double topTime = cpuTime();
  timeval time;;
  gettimeofday(&time, NULL);
  double realTime = time.tv_sec + (time.tv_usec/1000000.0);

  // Set up the solver interface
  reportf("Producing output in \"%s\".\n", outputFile.c_str());
  Solver* solver = NULL;
  switch (satSolver) {
    case SOLVER_NONE:
      solver = new NullSolver();
      break;
    case SOLVER_FILE:
      solver = new FileSolver(outputFile);
      break;
    case SOLVER_PICOSAT:
      solver = new PicoSatSolver();
      break;
  };
  
  NiceSat sat(*solver);
  
  sat.doMatching = !noMatching;
  sat.doGraphViz = printGraph;
  Edge dag;
  bool status_ok;
  switch (fileType) {
    case BINARY_AND_INVERTER_GRAPH: {
      sat.logStart("Reading AIG");
      AigReader reader(inputFile);
      dag = reader.read(sat,status_ok);
      if (!status_ok) 
        {unlink(outputFile.c_str());
         exit(1);
        }
      sat.logEnd();
      break;
    }
    default:
      delete solver;
      errorf("Unrecognized extension on file \"%s\"\n", inputFile);
  }
    
  if (printStats)   sat.printGraphStats(dag);
  if (printStats)   sat.printStats();

  sat.add(dag);

  if (printStats)   sat.printStats();

  topTime = cpuTime() - topTime;
  gettimeofday(&time, NULL);
  realTime = time.tv_sec + (time.tv_usec/1000000.0) - realTime;

  reportf("%-25s: %.4g s\n", "Translation CPU Time", topTime);
  reportf("%-25s: %.4g s\n", "Translation Real Time", realTime);
  uint64_t mem_used = memUsed();
  if (mem_used != 0) reportf("%-25s: %.2f MB\n", "Translation Memory Used", mem_used / 1048576.0);

  Answer ans = sat.solve();

  if (ans != Answer_Unknown)
    {ofstream out(outputFile.c_str());
    if (ans == Answer_Satisfiable) {
      out << "s SATISFIABLE\n";
      reportf("SATISFIABLE\n");
    } else {
      out << "s UNSATISFIABLE\n";
      reportf("UNSATISFIABLE\n");
    }
  } else {
    if ((satSolver == SOLVER_NONE) || (satSolver == SOLVER_FILE))
      reportf("The SAT-solving step is skipped. To invoke a SAT-solver use the '-s' option\n");
    else
     reportf("UNKNOWN\n");
  }
  
  // convert nicedag-pre-exp to filename-pre-exp.dot/png
  if (sat.doGraphViz) {
    reportf("Post-processing GraphViz files...\n");
    postProcessGraphViz(fileName.c_str(), "pre-exp");
    postProcessGraphViz(fileName.c_str(), "post-exp");
  }

  delete solver;
  RETURN(ans);
}
