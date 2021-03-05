//
// Created by llarrauriborroto on 23/02/2021.
//

#ifndef PMIN_BASIC_INCREMENTAL_H
#define PMIN_BASIC_INCREMENTAL_H

#include "../ofa_minimizer.h"
#include "cryptominisat5/cryptominisat.h"
#include "../compat_matrix.h"

namespace SBCMin::OFAMinimizers {


    class BasicIncremental : public OFAMinimizer {
    private:


        int max_var=0;
        bool initial_covered = false;
        bool incremented = false;

        std::vector<size_t> size_vars;
        std::vector<int> partial_solution;



        void init() override;

        bool step() override;

        void buildFrameClauses();

        void buildCoverClauses();

        void buildIncrementalClauses();

        void generateIncrementalVars();

    public:

        BasicIncremental()=default;



    };

}


#endif //PMIN_BASIC_INCREMENTAL_H
