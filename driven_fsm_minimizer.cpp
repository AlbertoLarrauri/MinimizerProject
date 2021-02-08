//
// Created by llarrauriborroto on 28/01/2021.
//

#include <deque>
#include "fsms.h"
#include <cryptominisat5/cryptominisat.h>
#include <unordered_set>
#include <numeric>
#include <algorithm>

#include "driven_fsm_minimizer.h"

using namespace machines;
namespace minimizers {
    void DrivenFSMMinimizer::buildOFSM() {
        if (driver.getSize() == 0 || driver.getSize() == 0) {
            ofsm_init = true;
            return;
        }
        unsigned inputs = driver.numberOfInputs();
        unsigned size1 = driver.getSize();
        unsigned size2 = driven.getSize();
        auto ID = [size2](unsigned state1, unsigned state2) {
            return size2 * state1 + state2;
        };
//        std::vector<bool> added(size1*size2);
//        std::vector<bool> explored(size1*size2);
//        std::vector<unsigned> state_map;
//        std::deque<unsigned> unexplored;
        std::unordered_map<unsigned, unsigned> state_map;
        std::unordered_set<unsigned> unexplored(driven.getSize() * driver.getSize());
        unsigned max_state = 0;
        unexplored.insert(0);
        state_map[ID(0, 0)] = 0;
        obs_fsm.addStates(1);
        while (!unexplored.empty()) {
            unsigned pair = *unexplored.begin();

            unsigned state12 = state_map[pair];

            unsigned state1 = pair / size2;
            unsigned state2 = pair % size2;

            unsigned unexplored_succs = 0;

            for (unsigned i = 0; i < inputs; ++i) {
                unsigned o = driver.out(state1, i);
                unsigned t = driven.out(state2, o);
                unsigned next1 = driver.succ(state1, i);
                unsigned next2 = driven.succ(state2, o);
                unsigned next12;
                unsigned next_pair = ID(next1, next2);

                if (!state_map.count(next_pair)) {
                    obs_fsm.addStates();
                    state_map[next_pair] = ++max_state;
                    next12 = max_state;
                    unexplored.insert(next_pair);
                } else next12 = state_map[next_pair];


                if (!obs_fsm.hasTransition(state_map[pair], o)) {
                    obs_fsm.setTransition(state12, o, t);
                }
                if (!obs_fsm.hasSources(next12, o, t) ||
                    obs_fsm.sources(next12, o, t).back() != state12) {
                    obs_fsm.addSucc(state12, o, next12);
                }
            }

            unexplored.erase(pair);
            obs_fsm.addStates(unexplored_succs);

        }

        ofsm_init = true;
        matrix_up_to_date = false;
        matrix_processed = false;
        return;
    }


    void DrivenFSMMinimizer::buildCMatrix() {
        if (!ofsm_init) {
            std::cout << " Observation FSM not built yet. \n";
            return;
        }

        const unsigned &size = obs_fsm.getSize();
        const unsigned &inputs = obs_fsm.numberOfInputs();
        compat_matrix.clear();
        compat_matrix.resize((size * (size + 1)) / 2);
        std::deque<std::pair<unsigned, unsigned>> modified;

        for (unsigned state1 = 1; state1 < size; ++state1) {
            for (unsigned state2 = 0; state2 < state1; ++state2) {
                size_t current_pair = toCMatrixID(state1, state2);
                if (compat_matrix[current_pair]) continue;
                for (unsigned i = 0; i < inputs; ++i) {
                    if (obs_fsm.hasTransition(state1, i) &&
                        obs_fsm.hasTransition(state2, i) &&
                            (obs_fsm.out(state1, i) != obs_fsm.out(state2, i))) {
//                            if((state1==2 && state2==3) ||(state2==2 && state1==3)){
//                            std::cout<<"BUGAZO \n";
//                            std::cout<<"Input: "<<i<<"\n";
//                            std::cout<<"Outputs: "<<obs_fsm.out(state1, i)<<", "<<obs_fsm.out(state2, i)<<"\n";
//                            }
                        compat_matrix[current_pair] = true;
                        modified.emplace_back(state1, state2);

                        while (!modified.empty()) {

                            auto mstate1 = modified.front().first;
                            auto mstate2 = modified.front().second;
                            modified.pop_front();
                            auto sources1 = obs_fsm.sourceData(mstate1);
                            auto sources2 = obs_fsm.sourceData(mstate2);

                            for (auto pair:sources1) {
                                if (sources2.count(pair.first)) {
                                    auto parents1 = pair.second;
                                    auto parents2 = sources2[pair.first];
                                    for (auto parent1:parents1) {
                                        for (auto parent2:parents2) {
//                                            if((parent1==2 && parent2==3) ||(parent2==2 && parent1==3)){
//                                                std::cout<<"BUGAZO";
//                                            }
                                            auto parent_pair = toCMatrixID(parent1, parent2);
                                            if (compat_matrix[parent_pair]) continue;
                                            compat_matrix[parent_pair] = true;
                                            modified.emplace_back(parent1, parent2);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        matrix_up_to_date = true;
        matrix_processed = false;
    }

    void DrivenFSMMinimizer::processCMatrix() {
        if (!matrix_up_to_date) {
            std::cout << "Compatibility matrix is not up-to-date. \n";
            return;
        }
        const unsigned &size = obs_fsm.getSize();

        incompatible_pairs.clear();
        big_clique.clear();

        std::vector<unsigned> incompatibility_scores(size);


        for (unsigned state1 = 1; state1 < size; ++state1) {
            for (unsigned state2 = 0; state2 < state1; ++state2) {
                if (compat_matrix[toCMatrixID(state1, state2)]) {
                    ++incompatibility_scores[state1];
                    ++incompatibility_scores[state2];
                    incompatible_pairs.push_back(size * state1 + state2);
                }
            }

        }


        if (incompatible_pairs.empty()) {
            big_clique.push_back(0);
            matrix_processed = true;
            return;
        }


        auto comparator = [incompatibility_scores](unsigned state1, unsigned state2) -> bool {
            return (incompatibility_scores[state1] < incompatibility_scores[state2]);
        };


        std::vector<unsigned> ordered_states(size);
        std::iota(ordered_states.begin(), ordered_states.end(), 0);
        std::sort(ordered_states.begin(), ordered_states.end(), comparator);
        big_clique.push_back(ordered_states[0]);

        for (unsigned stateID = 1; stateID < size; ++stateID) {
            unsigned const &state1 = ordered_states[stateID];
            bool valid = true;
            for (unsigned state2:big_clique) {
                if (!compat_matrix[toCMatrixID(state1, state2)]) {
                    valid = false;
                    break;
                }
            }
            if (valid) big_clique.push_back(state1);
        }
        matrix_processed = true;
    }


    void DrivenFSMMinimizer::printIncompatibles() {
        if (!matrix_up_to_date) {
            std::cout << "Compatibility matrix is not up-to-date. \n";
            return;
        }
        std::cout << "Pairs of incompatible states in the Observation Machine: ";
        bool first = true;
        for (unsigned state1 = 0; state1 < obs_fsm.getSize(); ++state1) {
            for (unsigned state2 = 0; state2 < state1; ++state2) {
                if (compat_matrix[toCMatrixID(state1, state2)]) {
                    if (!first) {
                        std::cout << ", ";
                    } else first = false;
                    std::cout << "( " << state1 << ", " << state2 << " )";
                }
            }
        }
        std::cout << "\n";
    }

    void DrivenFSMMinimizer::printBigClique() {
        if (!matrix_up_to_date) {
            std::cout << "Clique not computed yet. \n";
            return;
        }
        std::cout << "Clique of incompatible states: ";
        bool first = true;
        for (unsigned state:big_clique) {
            if (!first) std::cout << ", ";
            std::cout << state;
            first = false;
        }
        std::cout << "\n";
    }


    void DrivenFSMMinimizer::buildInitialCNF() {
        current_size = big_clique.size();
        const unsigned &size = obs_fsm.getSize();
        state_class_vars.resize(size * current_size);
        class_class_vars.resize(current_size * current_size * obs_fsm.numberOfInputs());
//@ Creating state-class variables

        for (unsigned Class = 0; Class < current_size; ++Class) {
            for (unsigned state = 0; state < size; ++state) {
                state_class_vars[stateClassToID(state, Class)] = max_var;
                ++max_var;
            }

            for (unsigned Class2 = 0; Class2 < current_size; ++Class2) {
                for (unsigned i = 0; i < obs_fsm.numberOfInputs(); ++i) {
                    class_class_vars[CCiToID(Class, Class2, i)] = max_var;
                    ++max_var;
                }
            }
        }
        size_vars.push_back(max_var);
        ++max_var;

        solver.new_vars(max_var);

        for (unsigned Class = 0; Class < current_size; ++Class) {
            std::vector<CMSat::Lit> clause;
            unsigned state = big_clique[Class];

            if (!state) initial_covered = true;

            size_t var = state_class_vars[stateClassToID(state, Class)];
            clause.emplace_back(var, false);
//            std::cout<<clause<<"\n";
                solver.add_clause(clause);;
        }


        if (!initial_covered) {
//            std::cout<<"Initial cover clauses: \n";
            buildInitialCoverCNF();
        }
//        Machine structure clauses
//        std::cout<<"Frame clauses: \n";
        buildFrameCNF();

//        std::cout<<"Incompatibility clauses: \n";
        for (unsigned Class = 0; Class < current_size; ++Class) {
            for (size_t pair:incompatible_pairs) {
                unsigned state1 = pair / size;
                unsigned state2 = pair % size;
                assert(compat_matrix[toCMatrixID(state1,state2)]);
                assert(state1!=state2);
                std::vector<CMSat::Lit> clause;
                clause.reserve(2);
                clause.emplace_back(state_class_vars[stateClassToID(state1, Class)], true);
                clause.emplace_back(state_class_vars[stateClassToID(state2, Class)], true);
//                std::cout<<clause<<"\n";
                solver.add_clause(clause);
            }
        }
//
//        std::cout<<"Successor clauses: \n";
        for (unsigned Class1 = 0; Class1 < current_size; ++Class1) {
            for (unsigned i = 0; i < obs_fsm.numberOfInputs(); ++i) {
                for (unsigned Class2 = 0; Class2 < current_size; ++Class2) {
                    for (unsigned state1 = 0; state1 < size; ++state1) {
                        if (!obs_fsm.hasTransition(state1, i)) continue;
                        auto succs = obs_fsm.succs(state1, i);
//                            std::cout<<"\n\n Loop beguins: \n";
                        for (unsigned state2:succs) {
//                            std::cout<<"Successor: "<<state2<<"\n";
                            std::vector<CMSat::Lit> clause;
                            clause.reserve(3);
                            clause.emplace_back(class_class_vars[CCiToID(Class1, Class2, i)], true);
                            clause.emplace_back(state_class_vars[stateClassToID(state1, Class1)], true);
                            clause.emplace_back(state_class_vars[stateClassToID(state2, Class2)], false);
//                            std::cout<<clause<<"\n";
                            solver.add_clause(clause);
                        }
                    }
                }
            }
        }
    }


    void DrivenFSMMinimizer::tryMinimize() {
        std::vector<CMSat::Lit> size_assumption;
        size_assumption.emplace_back(size_vars.back(), false);
        auto possible = solver.solve(&size_assumption);
        if (possible == CMSat::l_False) {
            std::cout << "Not able to minimize with size: " <<current_size<<".\n";
//            possible=solver.solve();
//            std::cout<<"With no size assumption: "<< possible<<" \n";
//            std::cout<<"Assumption number: "<< size_vars.back()<<" \n";
            return;
        } else {
            if (possible == CMSat::l_True) std::cout << "Minimizing with size: "<< current_size<<".\n";
            solved= true;
        }

        std::vector<CMSat::lbool> model = solver.get_model();
        const unsigned &result_size =current_size;
        const unsigned &size = obs_fsm.getSize();
        result.addStates(result_size);

        std::vector<unsigned> classIDs(result_size);

        bool initial_found = false;
        for (unsigned Class = 0; Class < result_size; ++Class) {
            if (!initial_found && model[state_class_vars[stateClassToID(0, Class)]] == CMSat::l_True) {
                initial_found = true;
                classIDs[Class] = 0;
            } else {
                if (!initial_found) {
                    classIDs[Class] = Class + 1;
                } else {
                    classIDs[Class] = Class;
                }

            }
        }

        for (unsigned Class1 = 0; Class1 < result_size; ++Class1) {
            for (unsigned i = 0; i < obs_fsm.numberOfInputs(); ++i) {
                for (unsigned Class2 = 0; Class2 < result_size; ++Class2) {
                    if (model[class_class_vars[CCiToID(Class1, Class2, i)]] == CMSat::l_True) {
                        result.succ(classIDs[Class1], i) = classIDs[Class2];
                    }
                }
                bool out_found = false;
                for (unsigned state = 0; state < size; ++state) {
                    if (model[state_class_vars[stateClassToID(state, Class1)]] == CMSat::l_True) {
                        if (obs_fsm.hasTransition(state, i)) {
                            result.out(classIDs[Class1], i) = obs_fsm.out(state, i);
                            out_found = true;
                        }
                    }
                }
                if (!out_found) result.out(classIDs[Class1], i) = 0;
            }
        }

        printResult();
    }


    void DrivenFSMMinimizer::printResult() {
        std::cout << "Minimized machine table: \n";
        result.print();
    }


    void DrivenFSMMinimizer::buildFrameCNF() {

        for (unsigned Class1 = 0; Class1 < current_size; ++Class1) {
            for (unsigned i = 0; i < obs_fsm.numberOfInputs(); ++i) {
                std::vector<CMSat::Lit> clause;
                clause.reserve(current_size+1);
                for (unsigned Class2 = 0; Class2 < current_size; ++Class2) {
//                    std::cout<<"\n\n\n";
//                    std::cout<<"States: "<<Class1<<", "<<Class2<<"\n";
//                    std::cout<<"ID: "<<elegant_pairing(Class1,Class2)<<", "<<CCiToID(Class1, Class2, i)<<","<<class_class_vars[CCiToID(Class1, Class2, i)]<< "\n";
                    clause.emplace_back(class_class_vars[CCiToID(Class1, Class2, i)], false);
                }
                clause.emplace_back(size_vars.back(), true);
//                std::cout<<clause<<"\n";
                solver.add_clause(clause);;
            }
        }
    }


    void DrivenFSMMinimizer::buildInitialCoverCNF() {
        std::vector<CMSat::Lit> clause;
        clause.reserve(current_size+1);
        for (unsigned Class = 0; Class < current_size; ++Class) {
            size_t var = state_class_vars[stateClassToID(0, Class)];
            clause.emplace_back(var, false);
        }
        if(!incremented){
        clause.emplace_back(size_vars.back(), true);
        } else{
            initial_covered= true;
        }
//  std::cout<<clause<<"\n";
        solver.add_clause(clause);
    }


    void DrivenFSMMinimizer::buildIncrementalClauses() {
        const unsigned &size = obs_fsm.getSize();
//       std::cout<<"Incompatibility clauses: \n";
        for (size_t pair:incompatible_pairs) {
            unsigned state1 = pair / size;
            unsigned state2 = pair % size;
            assert(compat_matrix[toCMatrixID(state1,state2)]);
            std::vector<CMSat::Lit> clause;
            clause.reserve(2);
            clause.emplace_back(state_class_vars[stateClassToID(state1, current_size-1)], true);
            clause.emplace_back(state_class_vars[stateClassToID(state2, current_size-1)], true);
//            std::cout<<clause<<"\n";
            solver.add_clause(clause);
        }

//        std::cout<<"Successor clauses: \n";
        for (unsigned Class = 0; Class < current_size; ++Class) {
            for (unsigned i = 0; i < obs_fsm.numberOfInputs(); ++i) {
                for (unsigned state1 = 0; state1 < size; ++state1) {
                    if (!obs_fsm.hasTransition(state1, i)) continue;

                    auto succs = obs_fsm.succs(state1, i);
                    for (unsigned state2:succs) {
                        std::vector<CMSat::Lit> clause1;
                        std::vector<CMSat::Lit> clause2;
                        clause1.reserve(3);
                        clause2.reserve(3);
                        clause1.emplace_back(class_class_vars[CCiToID(Class, current_size-1, i)], true);
                        clause1.emplace_back(state_class_vars[stateClassToID(state1, Class)], true);
                        clause1.emplace_back(state_class_vars[stateClassToID(state2, current_size-1)], false);
//                       std::cout<<clause1<<"\n";
                        solver.add_clause(clause1);
                        clause2.emplace_back(class_class_vars[CCiToID(current_size-1, Class, i)], true);
                        clause2.emplace_back(state_class_vars[stateClassToID(state1, current_size-1)], true);
                        clause2.emplace_back(state_class_vars[stateClassToID(state2, Class)], false);
//                      std::cout<<clause2<<"\n";
                        solver.add_clause(clause2);
                    }
                }
            }
        }
    }



    void DrivenFSMMinimizer::generateIncrementalVars() {
        size_t initial_vars=max_var;
        const unsigned &size = obs_fsm.getSize();
        state_class_vars.resize(size * current_size);
        class_class_vars.resize(current_size * current_size * obs_fsm.numberOfInputs());
//@ Creating state-class variables

        for (unsigned state = 0; state < size; ++state) {
            state_class_vars[stateClassToID(state, current_size-1)] = max_var;
            ++max_var;
        }
        for (unsigned i = 0; i < obs_fsm.numberOfInputs(); ++i) {

            for (unsigned Class2 = 0; Class2 < current_size-1; ++Class2) {

                class_class_vars[CCiToID(current_size-1, Class2, i)] = max_var;
                ++max_var;
                class_class_vars[CCiToID(Class2,current_size-1,i)] = max_var;
                ++max_var;
            }
            class_class_vars[CCiToID(current_size-1,current_size-1,i)] = max_var;
            ++max_var;
        }

        size_vars.push_back(max_var);
        ++max_var;
        solver.new_vars(max_var-initial_vars);
    }


    void DrivenFSMMinimizer::incrementCNF() {
        current_size++;
        incremented= true;
        generateIncrementalVars();
//        std::cout<<"Frame clauses: \n";
        buildFrameCNF();
//        std::cout<<"Initial cover clauses: \n";
        if(!initial_covered) buildInitialCoverCNF();
        buildIncrementalClauses();
    }

    void DrivenFSMMinimizer::solve() {
        solver.set_allow_otf_gauss();
        std::cout<<"Observation FSM size: "<<obs_fsm.getSize()<<". \n";
//        std::cout<<"Impostor: "<<initial_covered<<"\n";
        buildInitialCNF();
        tryMinimize();
        while (!solved){
            incrementCNF();
            tryMinimize();
        }
    }
}


