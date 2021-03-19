//
// Created by lazhares on 05/03/2021.
//

#ifndef PMIN_W_SYMMETRY_BREAKING_H
#define PMIN_W_SYMMETRY_BREAKING_H

#include "assumption_based.h"
namespace SBCMin::OFAMinimizers {

    class WSymmetryBreaking: public AssumptionBased {

    private:



        std::vector<size_t> used_vars;
        std::vector<size_t> participate_vars;

        inline size_t& usedVar (int state, int set) {
            return used_vars[stateSetVar(state,set)];
        }

        inline size_t& participateVar(int state, int set){
            return participate_vars[stateSetVar(state,set)];
        }



        inline void resize(int size){
            AssumptionBased::resize(size);
            used_vars.resize(ofa().getSize()*size);
            participate_vars.resize(ofa().getSize()*size);
        }

        inline auto next(int set, int input){
            assert(set<upper_bound && input<ofa().numberOfInputs());
            assert(set<upper_bound-1||input<ofa().numberOfInputs()-1);
            if(input+1!=ofa().numberOfInputs()){
                return std::pair(set,input+1);
            }else{
                return std::pair(set+1,0);
            }
        }

        inline auto prev(int set, int input){
            assert(set>=0 && input>=0);
            assert(set>0||input>0);

            if(input!=0){
                return std::pair(set,input-1);
            }else{
                return std::pair<int,int>(set-1,ofa().numberOfInputs()-1);
            }
        }

        void init() override;

        void generateVars() override;
        void buildSymmetryBreakingClauses();



    };

}

#endif //PMIN_W_SYMMETRY_BREAKING_H
