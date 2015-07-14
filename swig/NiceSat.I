%module nicesat

%apply const std::string& {std::string* foo};

%{
#include "NiceSat.H"
#include "ADTs/Edge.H"
#include "Solvers/Solver.H"
#include "Solvers/PicoSatSolver.H"
#include "Solvers/FileSolver.H"
#include "Solvers/NullSolver.H"
#include "ADTs/Vec.H"
using namespace nicesat;
using namespace std;
%}

%rename(EdgeVec) Vec<Edge>;

typedef long int intptr_t;

class Vec<Edge> {
public:

        Vec<Edge> ();
        void push (const Edge&);

};

enum Assignment {
        Assignment_True    =  1,
        Assignment_False   = -1,
        Assignment_Unknown =  0
};

enum Answer {
        Answer_Satisfiable = 10,
        Answer_Unsatisfiable = 20,
        Answer_Unknown = 0
};

class Edge {
public:

        /*
         * exposing methods we absolutely need -- most interaction
         * is through the NiceSat class
         */

        Edge ();
        Edge(const Edge& e);
        intptr_t getVar();
        void negate();

};

virtual class Solver {

        virtual Solver();
        virtual ~Solver();

};

class NullSolver: public Solver {
public:

        NullSolver();
        ~NullSolver();

};

class PicoSatSolver: public Solver {
public:

        PicoSatSolver();
        ~PicoSatSolver();

};

class FileSolver: public Solver {
public:

        FileSolver(const std::string& filename);
        ~FileSolver();

};

static const Edge True;

static const Edge False;

class NiceSat {
public:

        bool doMatching;

        bool doGraphViz;

        NiceSat();

        NiceSat(Solver& solver);

        virtual ~NiceSat() { if (_freeSolver) delete _solver; }

        void predictNumNodes(size_t numNodes);

        Edge createVar_(Literal lit) { return Edge(_solver->getVar(lit)); }
    
        Edge createVar() { return Edge(_solver->getNewVar()); }

        Edge createAnd(Vec<Edge>& args);

        Edge createIff(Edge a, Edge b);

        Edge createIte(Edge tst, Edge tt, Edge ff);

        Edge createAnd(Edge a, Edge b);

        Edge createOr(Edge a, Edge b);

        Edge createOr(Vec<Edge>& args);

        Edge createImpl(Edge lhs, Edge rhs);

        void add (Edge f, bool removable = false);

        Answer solve() { return _solver->runSolver(); }

        Assignment deref(Edge e) const;

        void printGraphStats(Edge root);

        void printStats();

        Solver& getSolver() { return *_solver; }

        void logStart(const char* str, ...);

        void logEnd();

};
