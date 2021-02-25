//
// Created by llarrauriborroto on 23/02/2021.
//

#include "cnf_builders.h"
#include "compat_matrix.h"

using namespace SBCMin;
using namespace OFA_CNFBuilders;

DFSM CNFBuilder::solve() {
    init();
    while (!trySolve()) {
        step();
    }
    return getSolution();
}


BasicIncremental::BasicIncremental(OFA &_ofa, CompatMatrix &_compat_matrix) :
        ofa(_ofa),
        compat_matrix(_compat_matrix) {
    solver = std::make_unique<CMSat::SATSolver>();
}


inline size_t BasicIncremental::stateClassToID(int state, int Class) {
    return Class * ofa.getSize() + state;
}

inline size_t BasicIncremental::elegant_pairing(int
                                                x, int
                                                y) {
    return (x >= y ? (x * x) + x + y : (y * y) + x);
}

inline size_t BasicIncremental::CCiToID(int
                                        Class1, int
                                        Class2, int
                                        input) {
    return (ofa.numberOfInputs() * elegant_pairing(Class1, Class2)) + input;
}


inline size_t &BasicIncremental::stateClassVar(int state, int var) {
    return state_class_vars[stateClassToID(state, var)];
}

inline size_t &BasicIncremental::classClassVar(int Class1, int Class2, int input) {
    return class_class_vars[CCiToID(Class1, Class2, input)];
}


void BasicIncremental::init() {
    std::vector<int> &big_clique = compat_matrix.getClique();

    current_size = big_clique.size();
    const int &size = ofa.getSize();
    state_class_vars.resize(size * current_size);
    class_class_vars.resize(current_size * current_size * ofa.numberOfInputs());


//// Creating state-class variables

    for (int Class = 0; Class < current_size; ++Class) {
        for (int state = 0; state < size; ++state) {
            stateClassVar(state, Class) = max_var;
            ++max_var;
        }

        for (int Class2 = 0; Class2 < current_size; ++Class2) {
            for (int i = 0; i < ofa.numberOfInputs(); ++i) {
                classClassVar(Class, Class2, i) = max_var;
                ++max_var;
            }
        }
    }
    size_vars.push_back(max_var);
    ++max_var;

    solver->new_vars(max_var);

    for (int Class = 0; Class < current_size; ++Class) {
        std::vector<CMSat::Lit> clause;
        int state = big_clique[Class];

        if (!state) initial_covered = true;

        size_t var = state_class_vars[stateClassToID(state, Class)];
        clause.emplace_back(var, false);
        solver->add_clause(clause);
    }


    if (!initial_covered) {

        buildCoverClauses();
    }

    buildFrameCNF();


    for (int Class = 0; Class < current_size; ++Class) {
        for (auto pair:compat_matrix.getPairs()) {
            int state1 = pair.first;
            int state2 = pair.second;
            assert(compat_matrix.areIncompatible(state1, state2));
            assert(state1 != state2);
            std::vector<CMSat::Lit> clause;
            clause.reserve(2);
            clause.emplace_back(state_class_vars[stateClassToID(state1, Class)], true);
            clause.emplace_back(state_class_vars[stateClassToID(state2, Class)], true);

            solver->add_clause(clause);
        }
    }

    for (int Class1 = 0; Class1 < current_size; ++Class1) {
        for (int i = 0; i < ofa.numberOfInputs(); ++i) {
            for (int Class2 = 0; Class2 < current_size; ++Class2) {
                for (int state1 = 0; state1 < size; ++state1) {
                    if (!ofa.hasTransition(state1, i)) continue;
                    auto succs = ofa.succs(state1, i);

                    for (int state2:succs) {

                        std::vector<CMSat::Lit> clause;
                        clause.reserve(3);
                        clause.emplace_back(class_class_vars[CCiToID(Class1, Class2, i)], true);
                        clause.emplace_back(state_class_vars[stateClassToID(state1, Class1)], true);
                        clause.emplace_back(state_class_vars[stateClassToID(state2, Class2)], false);

                        solver->add_clause(clause);
                    }
                }
            }
        }
    }
}


void BasicIncremental::buildFrameCNF() {
    for (int Class1 = 0; Class1 < current_size; ++Class1) {
        for (int i = 0; i < ofa.numberOfInputs(); ++i) {
            std::vector<CMSat::Lit> clause;
            clause.reserve(current_size + 1);
            for (int Class2 = 0; Class2 < current_size; ++Class2) {
                clause.emplace_back(class_class_vars[CCiToID(Class1, Class2, i)], false);
            }
            clause.emplace_back(size_vars.back(), true);
            solver->add_clause(clause);
        }
    }
}


void BasicIncremental::buildCoverClauses() {
    std::vector<CMSat::Lit> clause;
    clause.reserve(current_size + 1);
    for (int Class = 0; Class < current_size; ++Class) {
        size_t var = state_class_vars[stateClassToID(0, Class)];
        clause.emplace_back(var, false);
    }
    if (!incremented) {
        clause.emplace_back(size_vars.back(), true);
    } else {
        initial_covered = true;
    }
//  std::cout<<clause<<"\n";
    solver->add_clause(clause);
}


void BasicIncremental::generateIncrementalVars() {
    size_t initial_vars = max_var;
    const int &size = ofa.getSize();
    state_class_vars.resize(size * current_size);
    class_class_vars.resize(current_size * current_size * ofa.numberOfInputs());
//@ Creating state-class variables

    for (int state = 0; state < size; ++state) {
        state_class_vars[stateClassToID(state, current_size - 1)] = max_var;
        ++max_var;
    }
    for (int i = 0; i < ofa.numberOfInputs(); ++i) {

        for (int Class2 = 0; Class2 < current_size - 1; ++Class2) {

            class_class_vars[CCiToID(current_size - 1, Class2, i)] = max_var;
            ++max_var;
            class_class_vars[CCiToID(Class2, current_size - 1, i)] = max_var;
            ++max_var;
        }
        class_class_vars[CCiToID(current_size - 1, current_size - 1, i)] = max_var;
        ++max_var;
    }

    size_vars.push_back(max_var);
    ++max_var;
    solver->new_vars(max_var - initial_vars);
}

void BasicIncremental::buildIncrementalClauses() {
    const int &size = ofa.getSize();
//       std::cout<<"Incompatibility clauses: \n";
    for (auto pair:compat_matrix.getPairs()) {
        int state1 = pair.first;
        int state2 = pair.second;
        assert(compat_matrix.areIncompatible(state1, state2));
        std::vector<CMSat::Lit> clause;
        clause.reserve(2);
        clause.emplace_back(state_class_vars[stateClassToID(state1, current_size - 1)], true);
        clause.emplace_back(state_class_vars[stateClassToID(state2, current_size - 1)], true);
//            std::cout<<clause<<"\n";
        solver->add_clause(clause);
    }

//        std::cout<<"Successor clauses: \n";
    for (int Class = 0; Class < current_size; ++Class) {
        for (int i = 0; i < ofa.numberOfInputs(); ++i) {
            for (int state1 = 0; state1 < size; ++state1) {
                if (!ofa.hasTransition(state1, i)) continue;

                auto succs = ofa.succs(state1, i);
                for (int state2:succs) {
                    std::vector<CMSat::Lit> clause1;
                    std::vector<CMSat::Lit> clause2;
                    clause1.reserve(3);
                    clause2.reserve(3);
                    clause1.emplace_back(class_class_vars[CCiToID(Class, current_size - 1, i)], true);
                    clause1.emplace_back(state_class_vars[stateClassToID(state1, Class)], true);
                    clause1.emplace_back(state_class_vars[stateClassToID(state2, current_size - 1)], false);
                    solver->add_clause(clause1);
                    clause2.emplace_back(class_class_vars[CCiToID(current_size - 1, Class, i)], true);
                    clause2.emplace_back(state_class_vars[stateClassToID(state1, current_size - 1)], true);
                    clause2.emplace_back(state_class_vars[stateClassToID(state2, Class)], false);

                    solver->add_clause(clause2);
                }
            }
        }
    }
}

void BasicIncremental::step() {
    current_size++;
    incremented = true;
    generateIncrementalVars();
    buildFrameCNF();
    if (!initial_covered) buildCoverClauses();
    buildIncrementalClauses();
}

DFSM BasicIncremental::getSolution() {
    DFSM result(ofa.numberOfInputs(),ofa.numberOfOutputs());

    std::vector<CMSat::lbool> model = solver->get_model();
    const int &result_size = current_size;
    const int &size = ofa.getSize();

    result.addStates(result_size);

    std::vector<int> classIDs(result_size);

    bool initial_found = false;
    for (int Class = 0; Class < result_size; ++Class) {
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

    for (int Class1 = 0; Class1 < result_size; ++Class1) {
        for (int i = 0; i < ofa.numberOfInputs(); ++i) {
            for (int Class2 = 0; Class2 < result_size; ++Class2) {
                if (model[class_class_vars[CCiToID(Class1, Class2, i)]] == CMSat::l_True) {
                    result.setSucc(classIDs[Class1], i, classIDs[Class2]);
                }
            }
            bool out_found = false;
            for (int state = 0; state < size; ++state) {
                if (model[state_class_vars[stateClassToID(state, Class1)]] == CMSat::l_True) {
                    if (ofa.hasTransition(state, i)) {
                        result.setOut(classIDs[Class1], i, ofa->out(state, i));
                        out_found = true;
                    }
                }
            }
            if (!out_found) result.setOut(classIDs[Class1], i, 0);
        }
    }
    return result;
}


bool BasicIncremental::trySolve() {
    std::vector<CMSat::Lit> size_assumption;
    size_assumption.emplace_back(size_vars.back(), false);
    std::cout << "\n Number of variables: " << max_var << ".\n";
    auto possible = solver->solve(&size_assumption);
    if (possible == CMSat::l_False) {
        std::cout << "Not able to minimize with size: " << current_size << ".\n";
        return false;
    } else if (possible == CMSat::l_True) {
        std::cout << "Minimizing with size: " << current_size << ".\n";

        return true;

    }

    std::cout << "Undefined result";
    return false;
}
