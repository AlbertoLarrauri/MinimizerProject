//
// Created by llarrauriborroto on 23/02/2021.
//

#ifndef PMIN_CNF_BUILDERS_H
#define PMIN_CNF_BUILDERS_H

#include "ofa_minimizer.h"
#include "cryptominisat5/cryptominisat.h"
#include "compat_matrix.h"

namespace SBCMin::OFA_CNFBuilders {


    class CNFBuilder {
    private:

        virtual bool trySolve() = 0;

        virtual void init() = 0;

        virtual void step() = 0;

        virtual DFSM getSolution() = 0;

    public:

        DFSM solve();

    };


    class BasicIncremental : public CNFBuilder {
    private:
        std::unique_ptr<CMSat::SATSolver> solver;

        int max_var;
        int current_size;
        bool initial_covered=false;
        bool incremented=false;
        CompatMatrix &compat_matrix;
        OFA &ofa;

        std::vector<size_t> state_class_vars;
        std::vector<size_t> class_class_vars;
        std::vector<size_t> size_vars;

        size_t stateClassToID(int state, int Class);

        size_t elegant_pairing(int x, int y);

        size_t CCiToID(int Class1, int Class2, int input);

        size_t& stateClassVar(int state, int var);

        size_t& classClassVar(int Class1, int Class2, int input);


        bool trySolve() override;

        void init() override;

        void step() override;

        DFSM getSolution() override;

        void buildFrameCNF();

        void buildCoverClauses();

        void buildIncrementalClauses();

        void generateIncrementalVars();

    public:

        BasicIncremental(OFA &_ofa, CompatMatrix &_compat_matrix);


    };

}


#endif //PMIN_CNF_BUILDERS_H
