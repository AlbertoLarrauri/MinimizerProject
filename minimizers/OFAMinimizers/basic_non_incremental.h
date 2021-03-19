//
// Created by lazhares on 25/02/2021.
//

#ifndef PMIN_BASIC_NON_INCREMENTAL_H
#define PMIN_BASIC_NON_INCREMENTAL_H

#include "../ofa_minimizer.h"

namespace SBCMin::OFAMinimizers {
    class BasicNonIncremental: public OFAMinimizer{
    private:


        std::vector<int> partial_solution;
        int max_var=0;
        bool initial_covered = false;
        bool incremented = false;
        int cover_size=0;



        void init() override;

        bool step();

        void generateIncrementalVars();

        void buildFrameClauses();

        void buildCoverClauses();

        void buildCompatibilityClauses();

        void buildSuccessorClauses();

        bool runImpl() override;


    public:

        BasicNonIncremental()=default;


    };


}
#endif //PMIN_BASIC_NON_INCREMENTAL_H
