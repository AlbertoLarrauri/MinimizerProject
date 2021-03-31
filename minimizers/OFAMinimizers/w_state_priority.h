//
// Created by llarrauriborroto on 23/03/2021.
//

#ifndef PMIN_W_STATE_PRIORITY_H
#define PMIN_W_STATE_PRIORITY_H

#include "assumption_based.h"


namespace SBCMin::OFAMinimizers {

    class WStatePriority : public AssumptionBased {

    private:




        inline size_t stateSetToID(int state, int set) {
            return upper_bound*state+ set;
        }

        inline size_t &usedVar(int state, int set) {
            return used_vars[stateSetToID(state, set)];
        }

        std::vector<size_t> used_vars;

        inline void resize(int size) override{
            AssumptionBased::resize(size);
            used_vars.resize(ofa().size() * upper_bound);
        }


        void init() override;

        void buildCoveringClauses();

        void generateVars();

        bool query(int size) override;
        void buildSymmetryBreakingClauses();



    };

}




#endif //PMIN_W_STATE_PRIORITY_H
