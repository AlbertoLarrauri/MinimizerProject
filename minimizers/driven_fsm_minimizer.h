////
//// Created by llarrauriborroto on 28/01/2021.
////
//
//#ifndef PMIN_DRIVEN_FSM_MINIMIZER_H
//#define PMIN_DRIVEN_FSM_MINIMIZER_H
//
//#include "hopcroft.h"
//#include <iostream>
//#include <memory>
//#include "cryptominisat5/cryptominisat.h"
//#include "../machines/builders.h"
//#include "compat_matrix.h"
//
//namespace SBCMin {
//
//    class DrivenFSMMinimizer {
//    private:
//
//        //DATA STRUCTURES
//
//        const DFSM &driver;
//        const DFSM &driven;
//
//        std::unique_ptr<OFA> ofa;
//
//        std::unique_ptr<DFSM> result_ptr;
//
//        std::unique_ptr<CompatMatrix> compat_matrix;
//        std::vector<int> big_clique;
//
//        CMSat::SATSolver solver;
//
//        //FLAGS
//
//        bool generate_clique = true;
//
//
//        bool solved = false;
//        bool incremented = false;
//        bool matrix_up_to_date;
//
//
//        bool initial_covered = false;
//
//        size_t number_of_vars = 0;
//
//        std::vector<size_t> state_set_vars;
//        std::vector<size_t> set_set_vars;
//        std::vector<size_t> size_vars;
//        int current_size;
//
//        inline size_t stateSetToID(int state, int Class) {
//            return Class * ofa->getSize() + state;
//        }
//
//        inline size_t elegant_pairing(int
//                                      x, int
//                                      y) {
//            return (x >= y ? (x * x) + x + y : (y * y) + x);
//        }
//
//        inline size_t setSetInToID(int
//                              Class1, int
//                              Class2, int
//                              input) {
//            return (ofa->numberOfInputs() * elegant_pairing(Class1, Class2)) + input;
//        }
//
//
//        inline size_t toCMatrixID(int
//                                  state1, int
//                                  state2) {
//            if (state1 >= state2) {
//                return (state1 * (state1 + 1)) / 2 + state2;
//            }
//            return (state2 * (state2 + 1)) / 2 + state1;
//        }
//
//        void buildFrameClauses();
//
//        void buildInitialCoverCNF();
//
//        void buildIncrementalClauses();
//
//        void generateIncrementalVars();
//
//
//    public:
//
//
//
//        DrivenFSMMinimizer(const DFSM &_driver, const DFSM &_driven) :
//                driver(_driver),
//                driven(_driven),
//                result_ptr() {
//
//            assert(driver.numberOfOutputs() == driven.numberOfInputs());
//        }
//
//        inline const OFA &getOFSM() {
//            return *ofa;
//        }
//
//        void buildOFSM();
//
//        void buildCMatrix();
//
//
//        void printIncompatibles();
//
//        void printBigClique();
//
//        void buildInitialCNF();
//
//        void tryMinimize();
//
//        void printResult();
//
//        void incrementCNF();
//
//        void solve();
//
//    };
//}
//
//#endif //PMIN_DRIVEN_FSM_MINIMIZER_H
