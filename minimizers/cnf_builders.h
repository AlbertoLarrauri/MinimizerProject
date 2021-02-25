//
// Created by llarrauriborroto on 23/02/2021.
//

#ifndef PMIN_CNF_BUILDERS_H
#define PMIN_CNF_BUILDERS_H

#include "ofa_minimizer.h"
#include "cryptominisat5/cryptominisat.h"
#include "compat_matrix.h"

namespace SBCMin::OFACNFBuilders {


    class BasicIncremental : public OFACNFBuilder {
    private:

        std::unique_ptr<CMSat::SATSolver> solver;
        std::unique_ptr<DFSM> result;

        int max_var;
        int current_size;
        bool initial_covered = false;
        bool incremented = false;

        const CompatMatrix &compat_matrix;
        const std::vector<int>& partial_solution;
        const OFA &ofa;

        std::vector<size_t> state_class_vars;
        std::vector<size_t> class_class_vars;
        std::vector<size_t> size_vars;



        size_t stateClassToID(int state, int Class);

        size_t elegant_pairing(int x, int y);

        size_t CCiToID(int Class1, int Class2, int input);

        size_t &stateClassVar(int state, int var);

        size_t &classClassVar(int Class1, int Class2, int input);


        bool trySolve() override;

        void init() override;

        void step() override;

        void computeSolution() override;

        void buildFrameClauses();

        void buildCoverClauses();

        void buildIncrementalClauses();

        void generateIncrementalVars();

    public:

        BasicIncremental(OFA &_ofa, CompatMatrix &_compat_matrix);

        inline const DFSM &getResult() override{
            try {
                if (result==nullptr) throw(0);
                return *result;
            } catch(int){
                std::cout<<"Result not obtained yet. NULLPTR returned \n";
            }
        }


    };

}


#endif //PMIN_CNF_BUILDERS_H
