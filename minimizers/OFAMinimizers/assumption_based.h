//
// Created by lazhares on 03/03/2021.
//

#ifndef PMIN_ASSUMPTION_BASED_H
#define PMIN_ASSUMPTION_BASED_H


#include "../ofa_minimizer.h"
#include "cryptominisat5/cryptominisat.h"
#include "../compat_matrix.h"

namespace SBCMin::OFAMinimizers {
    class AssumptionBased: public OFAMinimizerWCustomStrategy{


    protected:

        bool initial_in_partial_solution=false;


        inline size_t& sizeVar(int size){
            assert(size>=lower_bound && size<=upper_bound);
            return size_vars[size-lower_bound];
        }


        size_t number_of_vars = 0;
        std::vector<size_t> size_vars;

        void init() override;

//        bool step() override;

        inline void resize(int size){
            size_vars.resize(size-lower_bound+1);
            OFAMinimizer::resize(size);
        }

        void buildSizeClauses();

        virtual void generateVars();

        void buildPartialSolutionClauses();

        void buildCoveringClauses();

        void buildFrameClauses();

        void buildIncompatibilityClauses();

        void buildSuccessorClauses();

        bool query(int size) override;


    public:

        inline AssumptionBased()=default;




    };

}

#endif //PMIN_ASSUMPTION_BASED_H
