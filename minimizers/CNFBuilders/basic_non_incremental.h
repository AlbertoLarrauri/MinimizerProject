//
// Created by lazhares on 25/02/2021.
//

#ifndef PMIN_BASIC_NON_INCREMENTAL_H
#define PMIN_BASIC_NON_INCREMENTAL_H

#include "../ofa_minimizer.h"
namespace SBCMin::OFACNFBuilders {
    class BasicNonIncremental: public OFACNFBuilder{
    private:

        std::unique_ptr<CMSat::SATSolver> solver;
        std::unique_ptr<DFSM> result;

        int max_var=0;
        int current_size=0;
        bool initial_covered = false;
        bool incremented = false;
        int cover_size=0;

        const CompatMatrix &compat_matrix;
        const std::vector<int>& partial_solution;
        const OFA &ofa;

        std::vector<size_t> state_class_vars;
        std::vector<size_t> class_class_vars;



        size_t stateClassToID(int state, int Class);

        size_t elegant_pairing(int x, int y);

        size_t CCiToID(int Class1, int Class2, int input);

        size_t &stateClassVar(int state, int var);

        size_t &classClassVar(int Class1, int Class2, int input);


        bool trySolve() override;

        void init() override;

        void step() override;

        void computeSolution() override;

        void generateIncrementalVars();

        void buildFrameClauses();

        void buildCoverClauses();

        void buildCompatibilityClauses();

        void buildSuccessorClauses();


    public:

        BasicNonIncremental(OFA &_ofa, CompatMatrix &_compat_matrix);

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
#endif //PMIN_BASIC_NON_INCREMENTAL_H
