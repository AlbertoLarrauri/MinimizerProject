//
// Created by llarrauriborroto on 28/01/2021.
//

#ifndef PMIN_DRIVEN_FSM_MINIMIZER_H
#define PMIN_DRIVEN_FSM_MINIMIZER_H

#include <iostream>
#include "cryptominisat5/cryptominisat.h"




using namespace machines;



namespace minimizers{


    class DrivenFSMMinimizer{
        private:
            DFSM& driver;
            DFSM& driven;

            OFSM obs_fsm;
            DFSM result;

            bool ofsm_init;

            bool solved=false;
            bool incremented=false;

            bool matrix_up_to_date;
            std::vector<bool> compat_matrix;

            bool matrix_processed;
            std::vector<unsigned> big_clique;
            std::vector<size_t> incompatible_pairs;


            CMSat::SATSolver solver;

            bool initial_covered=false;

            size_t max_var=0;

            std::vector<size_t> state_class_vars;
            std::vector<size_t> class_class_vars;
            std::vector<size_t> size_vars;
            unsigned current_size;

            inline size_t stateClassToID(unsigned state, unsigned Class){
                return Class*obs_fsm.getSize()+state;
            }

            inline size_t elegant_pairing(unsigned x, unsigned y){
                return (x >= y ? (x * x) + x + y : (y * y) + x);
            }

            inline size_t CCiToID(unsigned Class1, unsigned Class2, unsigned input){
                return (obs_fsm.numberOfInputs()*elegant_pairing(Class1,Class2))+input;
            }


            inline size_t toCMatrixID(unsigned state1, unsigned state2){
                if (state1>=state2){
                    return (state1*(state1+1))/2 + state2;
                }
                return (state2*(state2+1))/2 + state1;
            }

            void buildFrameCNF();
            void buildInitialCoverCNF();
            void buildIncrementalClauses();
            void generateIncrementalVars();


        public:
            DrivenFSMMinimizer(DFSM& _driver, DFSM& _driven):
                driver(_driver),
                driven(_driven),
                obs_fsm(driven.numberOfInputs(),
                            driven.numberOfOutputs()),
                result(driven.numberOfInputs(),
                       driven.numberOfOutputs())
                            {
                assert(driver.numberOfOutputs()==driven.numberOfInputs());
            }

            inline OFSM& getOFSM(){
                return obs_fsm;
            }

            void buildOFSM();

            void buildCMatrix();

            void processCMatrix();

            void printIncompatibles();

            void printBigClique();

            void buildInitialCNF();

            void tryMinimize();

            void printResult();

            void incrementCNF();

            void solve();

    };


}


#endif //PMIN_DRIVEN_FSM_MINIMIZER_H
