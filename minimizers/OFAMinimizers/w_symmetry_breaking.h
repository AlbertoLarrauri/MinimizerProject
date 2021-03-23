//
// Created by lazhares on 05/03/2021.
//

#ifndef PMIN_W_SYMMETRY_BREAKING_H
#define PMIN_W_SYMMETRY_BREAKING_H

#include "assumption_based.h"
namespace SBCMin::OFAMinimizers {

    class WSymmetryBreaking: public AssumptionBased {

    private:


        inline size_t setSetInToID(int set1, int set2, int input) {
            return ofa().numberOfInputs() *((upper_bound-lower_bound)*set1+ set2-lower_bound) + input;
        }

        inline size_t& usedVar (int set1, int set2, int input) {
            return used_vars[setSetInToID(set1,set2,input)];
        }

        std::vector<size_t> used_vars;

        inline void resize(int size){
            AssumptionBased::resize(size);
            used_vars.resize(size*(upper_bound-lower_bound)*ofa().numberOfInputs());
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
        void buildCoveringClauses();

        void generateVars();

        bool query(int size) override;
        void buildSymmetryBreakingClauses();



    };

}

#endif //PMIN_W_SYMMETRY_BREAKING_H
