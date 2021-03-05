////
//// Created by llarrauriborroto on 28/01/2021.
////
//
//#include <deque>
//#include "../machines/machines.h"
//#include <cryptominisat5/cryptominisat.h>
//#include <unordered_set>
//#include <numeric>
//#include <algorithm>
//
//#include "driven_fsm_minimizer.h"
//#include "../machines/builders.h"
//#include "../machines/builders.h"
//
//using namespace SBCMin;
//
//void DrivenFSMMinimizer::buildOFSM() {
//    ofa = std::make_unique<OFA>(builders::buildOFSM(driver, driven));
//}
//
//
//void DrivenFSMMinimizer::buildCMatrix() {
//    // TODO: This should be an exception
//    if (ofa == nullptr) throw "OFA has not been built yet";
//
//    const int dfsm_size = ofa->getSize();
//    const int inputs = ofa->numberOfInputs();
//    compat_matrix = std::make_unique<CompatMatrix>(dfsm_size);
//
//    std::deque<std::pair<int, int>> modified;
//
//    for (int state1 = 1; state1 < dfsm_size; ++state1) {
//        for (int state2 = 0; state2 < state1; ++state2) {
//            if (compat_matrix->areIncompatible(state1, state2)) continue;
//
//            for (int i = 0; i < inputs; ++i) {
//                if (ofa->hasTransition(state1, i) &&
//                    ofa->hasTransition(state2, i) &&
//                    (ofa->out(state1, i) != ofa->getOut(state2, i))) {
//                    compat_matrix->setIncompatible(state1, state2);
//                    modified.emplace_back(state1, state2);
//
//                    while (!modified.empty()) {
//                        auto mstate1 = modified.front().first;
//                        auto mstate2 = modified.front().second;
//                        modified.pop_front();
//                        auto &sources1 = ofa->sourceData(mstate1);
//                        auto &sources2 = ofa->sourceData(mstate2);
//                        for (auto pair:sources1) {
//                            if (sources2.count(pair.first)) {
//                                auto parents1 = pair.second;
//                                auto parents2 = sources2.at(pair.first);
//                                for (auto parent1:parents1) {
//                                    for (auto parent2:parents2) {
//                                        if (compat_matrix->areIncompatible(parent1, parent2)) continue;
//                                        compat_matrix->setIncompatible(parent1, parent2);
//                                        modified.emplace_back(parent1, parent2);
//                                    }
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    if (generate_clique) {
//        big_clique = compat_matrix->computeLargeClique();
//    }
//}
//
//
//void DrivenFSMMinimizer::printIncompatibles() {
//    if (!matrix_up_to_date) {
//        std::cout << "Compatibility matrix is not up-to-date. \n";
//        return;
//    }
//    std::cout << "Pairs of incompatible states in the Observation Machine: ";
//    bool first = true;
//    for (int state1 = 0; state1 < ofa->getSize(); ++state1) {
//        for (int state2 = 0; state2 < state1; ++state2) {
//            if (compat_matrix->areIncompatible(state1, state2)) {
//                if (!first) {
//                    std::cout << ", ";
//                } else first = false;
//                std::cout << "( " << state1 << ", " << state2 << " )";
//            }
//        }
//    }
//    std::cout << "\n";
//}
//
//void DrivenFSMMinimizer::printBigClique() {
//    if (!matrix_up_to_date) {
//        std::cout << "Clique not computed yet. \n";
//        return;
//    }
//    std::cout << "Clique of incompatible states: ";
//    bool first = true;
//    for (int state:big_clique) {
//        if (!first) std::cout << ", ";
//        std::cout << state;
//        first = false;
//    }
//    std::cout << "\n";
//}
//
//
//void DrivenFSMMinimizer::buildInitialCNF() {
//    current_size = big_clique.dfsm_size();
//    const int &dfsm_size = ofa->getSize();
//    state_set_vars.resize(dfsm_size * current_size);
//    set_set_vars.resize(current_size * current_size * ofa->numberOfInputs());
////@ Creating state-class variables
//
//    for (int Class = 0; Class < current_size; ++Class) {
//        for (int state = 0; state < dfsm_size; ++state) {
//            state_set_vars[stateSetToID(state, Class)] = number_of_vars;
//            ++number_of_vars;
//        }
//
//        for (int Class2 = 0; Class2 < current_size; ++Class2) {
//            for (int i = 0; i < ofa->numberOfInputs(); ++i) {
//                set_set_vars[setSetInToID(Class, Class2, i)] = number_of_vars;
//                ++number_of_vars;
//            }
//        }
//    }
//    size_vars.push_back(number_of_vars);
//    ++number_of_vars;
//
//    solver.new_vars(number_of_vars);
//
//    for (int Class = 0; Class < current_size; ++Class) {
//        std::vector<CMSat::Lit> clause;
//        int state = big_clique[Class];
//
//        if (!state) initial_covered = true;
//
//        size_t var = state_set_vars[stateSetToID(state, Class)];
//        clause.emplace_back(var, false);
////            std::cout<<clause<<"\n";
//        solver.add_clause(clause);;
//    }
//
//
//    if (!initial_covered) {
////            std::cout<<"Initial cover clauses: \n";
//        buildInitialCoverCNF();
//    }
////        Machine structure clauses
////        std::cout<<"Frame clauses: \n";
//    buildFrameClauses();
//
////        std::cout<<"Incompatibility clauses: \n";
//    for (int Class = 0; Class < current_size; ++Class) {
//        for (auto pair:compat_matrix->getPairs()) {
//            int state1 = pair.first;
//            int state2 = pair.second;
//            assert(compat_matrix->areIncompatible(state1, state2));
//            assert(state1 != state2);
//            std::vector<CMSat::Lit> clause;
//            clause.reserve(2);
//            clause.emplace_back(state_set_vars[stateSetToID(state1, Class)], true);
//            clause.emplace_back(state_set_vars[stateSetToID(state2, Class)], true);
////                std::cout<<clause<<"\n";
//            solver.add_clause(clause);
//        }
//    }
////
////        std::cout<<"Successor clauses: \n";
//    for (int Class1 = 0; Class1 < current_size; ++Class1) {
//        for (int i = 0; i < ofa->numberOfInputs(); ++i) {
//            for (int Class2 = 0; Class2 < current_size; ++Class2) {
//                for (int state1 = 0; state1 < dfsm_size; ++state1) {
//                    if (!ofa->hasTransition(state1, i)) continue;
//                    auto succs = ofa->getSuccs(state1, i);
////                            std::cout<<"\n\n Loop beguins: \n";
//                    for (int state2:getSuccs) {
////                            std::cout<<"Successor: "<<state2<<"\n";
//                        std::vector<CMSat::Lit> clause;
//                        clause.reserve(3);
//                        clause.emplace_back(set_set_vars[setSetInToID(Class1, Class2, i)], true);
//                        clause.emplace_back(state_set_vars[stateSetToID(state1, Class1)], true);
//                        clause.emplace_back(state_set_vars[stateSetToID(state2, Class2)], false);
////                            std::cout<<clause<<"\n";
//                        solver.add_clause(clause);
//                    }
//                }
//            }
//        }
//    }
//}
//
//
//void DrivenFSMMinimizer::tryMinimize() {
//    std::vector<CMSat::Lit> size_assumption;
//    size_assumption.emplace_back(size_vars.back(), false);
//    std::cout << "\n Number of variables: " << number_of_vars << ".\n";
//    auto possible = solver.solve(&size_assumption);
//    if (possible == CMSat::l_False) {
//        std::cout << "Not able to run with dfsm_size: " << current_size << ".\n";
////            possible=solver.run();
////            std::cout<<"With no dfsm_size assumption: "<< possible<<" \n";
////            std::cout<<"Assumption number: "<< size_vars.back()<<" \n";
//        return;
//    } else {
//        if (possible == CMSat::l_True) std::cout << "Minimizing with dfsm_size: " << current_size << ".\n";
//        solved = true;
//    }
//
//    std::vector<CMSat::lbool> model = solver.get_model();
//    const int &result_size = current_size;
//    const int &dfsm_size = ofa->getSize();
//    result_ptr = std::make_unique<DFSM>(driven.numberOfInputs(),
//                                    driven.numberOfOutputs());
//    result_ptr->addStates(result_size);
//
//    std::vector<int> classIDs(result_size);
//
//    bool initial_found = false;
//    for (int Class = 0; Class < result_size; ++Class) {
//        if (!initial_found && model[state_set_vars[stateSetToID(0, Class)]] == CMSat::l_True) {
//            initial_found = true;
//            classIDs[Class] = 0;
//        } else {
//            if (!initial_found) {
//                classIDs[Class] = Class + 1;
//            } else {
//                classIDs[Class] = Class;
//            }
//
//        }
//    }
//
//    for (int Class1 = 0; Class1 < result_size; ++Class1) {
//        for (int i = 0; i < ofa->numberOfInputs(); ++i) {
//            for (int Class2 = 0; Class2 < result_size; ++Class2) {
//                if (model[set_set_vars[setSetInToID(Class1, Class2, i)]] == CMSat::l_True) {
//                    result_ptr->setSucc(classIDs[Class1], i, classIDs[Class2]);
//                }
//            }
//            bool out_found = false;
//            for (int state = 0; state < dfsm_size; ++state) {
//                if (model[state_set_vars[stateSetToID(state, Class1)]] == CMSat::l_True) {
//                    if (ofa->hasTransition(state, i)) {
//                        result_ptr->setOut(classIDs[Class1], i, ofa->getOut(state, i));
//                        out_found = true;
//                    }
//                }
//            }
//            if (!out_found) result_ptr->setOut(classIDs[Class1], i, 0);
//        }
//    }
//
//    //printResult();
//}
//
//
//void DrivenFSMMinimizer::printResult() {
//    std::cout << "Minimized machine table: \n";
//    result_ptr->print();
//}
//
//
//void DrivenFSMMinimizer::buildFrameClauses() {
//
//    for (int Class1 = 0; Class1 < current_size; ++Class1) {
//        for (int i = 0; i < ofa->numberOfInputs(); ++i) {
//            std::vector<CMSat::Lit> clause;
//            clause.reserve(current_size + 1);
//            for (int Class2 = 0; Class2 < current_size; ++Class2) {
////                    std::cout<<"\n\n\n";
////                    std::cout<<"States: "<<Class1<<", "<<Class2<<"\n";
////                    std::cout<<"ID: "<<elegant_pairing(Class1,Class2)<<", "<<CCiToID(Class1, Class2, i)<<","<<set_set_vars[setSetInToID(Class1, Class2, i)]<< "\n";
//                clause.emplace_back(set_set_vars[setSetInToID(Class1, Class2, i)], false);
//            }
//            clause.emplace_back(size_vars.back(), true);
////                std::cout<<clause<<"\n";
//            solver.add_clause(clause);;
//        }
//    }
//}
//
//
//void DrivenFSMMinimizer::buildInitialCoverCNF() {
//    std::vector<CMSat::Lit> clause;
//    clause.reserve(current_size + 1);
//    for (int Class = 0; Class < current_size; ++Class) {
//        size_t var = state_set_vars[stateSetToID(0, Class)];
//        clause.emplace_back(var, false);
//    }
//    if (!incremented) {
//        clause.emplace_back(size_vars.back(), true);
//    } else {
//        initial_covered = true;
//    }
////  std::cout<<clause<<"\n";
//    solver.add_clause(clause);
//}
//
//
//void DrivenFSMMinimizer::buildIncrementalClauses() {
//    const int &dfsm_size = ofa->getSize();
////       std::cout<<"Incompatibility clauses: \n";
//    for (auto pair:compat_matrix->getPairs()) {
//        int state1 = pair.first;
//        int state2 = pair.second;
//        assert(compat_matrix->areIncompatible(state1, state2));
//        std::vector<CMSat::Lit> clause;
//        clause.reserve(2);
//        clause.emplace_back(state_set_vars[stateSetToID(state1, current_size - 1)], true);
//        clause.emplace_back(state_set_vars[stateSetToID(state2, current_size - 1)], true);
////            std::cout<<clause<<"\n";
//        solver.add_clause(clause);
//    }
//
////        std::cout<<"Successor clauses: \n";
//    for (int Class = 0; Class < current_size; ++Class) {
//        for (int i = 0; i < ofa->numberOfInputs(); ++i) {
//            for (int state1 = 0; state1 < dfsm_size; ++state1) {
//                if (!ofa->hasTransition(state1, i)) continue;
//
//                auto succs = ofa->getSuccs(state1, i);
//                for (int state2:getSuccs) {
//                    std::vector<CMSat::Lit> clause1;
//                    std::vector<CMSat::Lit> clause2;
//                    clause1.reserve(3);
//                    clause2.reserve(3);
//                    clause1.emplace_back(set_set_vars[setSetInToID(Class, current_size - 1, i)], true);
//                    clause1.emplace_back(state_set_vars[stateSetToID(state1, Class)], true);
//                    clause1.emplace_back(state_set_vars[stateSetToID(state2, current_size - 1)], false);
////                       std::cout<<clause1<<"\n";
//                    solver.add_clause(clause1);
//                    clause2.emplace_back(set_set_vars[setSetInToID(current_size - 1, Class, i)], true);
//                    clause2.emplace_back(state_set_vars[stateSetToID(state1, current_size - 1)], true);
//                    clause2.emplace_back(state_set_vars[stateSetToID(state2, Class)], false);
////                      std::cout<<clause2<<"\n";
//                    solver.add_clause(clause2);
//                }
//            }
//        }
//    }
//}
//
//
//void DrivenFSMMinimizer::generateIncrementalVars() {
//    size_t initial_vars = number_of_vars;
//    const int &dfsm_size = ofa->getSize();
//    state_set_vars.resize(dfsm_size * current_size);
//    set_set_vars.resize(current_size * current_size * ofa->numberOfInputs());
////@ Creating state-class variables
//
//    for (int state = 0; state < dfsm_size; ++state) {
//        state_set_vars[stateSetToID(state, current_size - 1)] = number_of_vars;
//        ++number_of_vars;
//    }
//    for (int i = 0; i < ofa->numberOfInputs(); ++i) {
//
//        for (int Class2 = 0; Class2 < current_size - 1; ++Class2) {
//
//            set_set_vars[setSetInToID(current_size - 1, Class2, i)] = number_of_vars;
//            ++number_of_vars;
//            set_set_vars[setSetInToID(Class2, current_size - 1, i)] = number_of_vars;
//            ++number_of_vars;
//        }
//        set_set_vars[setSetInToID(current_size - 1, current_size - 1, i)] = number_of_vars;
//        ++number_of_vars;
//    }
//
//    size_vars.push_back(number_of_vars);
//    ++number_of_vars;
//    solver.new_vars(number_of_vars - initial_vars);
//}
//
//
//void DrivenFSMMinimizer::incrementCNF() {
//    current_size++;
//    incremented = true;
//    generateIncrementalVars();
////        std::cout<<"Frame clauses: \n";
//    buildFrameClauses();
////        std::cout<<"Initial cover clauses: \n";
//    if (!initial_covered) buildInitialCoverCNF();
//    buildIncrementalClauses();
//}
//
//void DrivenFSMMinimizer::solve() {
////        solver.set_allow_otf_gauss();
//    std::cout << "Observation FSM dfsm_size: " << ofa->getSize() << ". \n";
////        std::cout<<"Impostor: "<<initial_covered<<"\n";
//    buildInitialCNF();
//    tryMinimize();
//    while (!solved) {
//        incrementCNF();
//        tryMinimize();
//    }
//}
//
//
//
