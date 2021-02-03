//
// Created by llarrauriborroto on 28/01/2021.
//

#ifndef PMIN_DRIVEN_FSM_MINIMIZER_H
#define PMIN_DRIVEN_FSM_MINIMIZER_H

#include "fsms.h"
#include<cryptominisat5/cryptominisat.h>



using namespace machines;



namespace minimizers{


    class DrivenFSMMinimizer{
        private:
            DFSM& driver;
            DFSM& driven;
            OFSM obs_fsm;
            bool ofsm_init;
            CMSat::SATSolver solver;
            std::vector<bool> compat_matrix;
            std::vector<int> state_class_vars;
            std::vector<int> class_class_vars;
            std::vector<int> size_vars;

            inline size_t toCMatrixID(unsigned state1, unsigned state2){
                if (state1>=state2){
                    return (state1*(state1+1))/2 + state2;
                }
                return (state2*(state2+1))/2 + state1;
            }



        public:
            DrivenFSMMinimizer(DFSM& _driver, DFSM& _driven):
                driver(_driver),
                driven(_driven),
                obs_fsm(driver.numberOfInputs(),
                            driven.numberOfOutputs()){
                assert(driver.numberOfOutputs()==driven.numberOfInputs());
            }

            inline OFSM& getOFSM(){
                return obs_fsm;
            }

            void buildOFSM();

            void buildCMatrix();


    };


}


#endif //PMIN_DRIVEN_FSM_MINIMIZER_H
