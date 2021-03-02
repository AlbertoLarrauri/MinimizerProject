//
// Created by lazhares on 25/02/2021.
//

#include "basic_non_incremental.h"

using namespace SBCMin::OFACNFBuilders;

BasicNonIncremental::BasicNonIncremental(OFA &_ofa, CompatMatrix &_compat_matrix) :
        ofa(_ofa),
        compat_matrix(_compat_matrix),
        partial_solution(compat_matrix.getClique()) {
    solver = std::make_unique<CMSat::SATSolver>();
    solver->set_single_run();
    cover_size=partial_solution.size();
//    max_var=0;

}


inline size_t BasicNonIncremental::stateClassToID(int state, int Class) {
    return Class * ofa.getSize() + state;
}

inline size_t BasicNonIncremental::elegant_pairing(int
                                                x, int
                                                y) {
    return (x >= y ? (x * x) + x + y : (y * y) + x);
}

inline size_t BasicNonIncremental::CCiToID(int
                                        Class1, int
                                        Class2, int
                                        input) {
    return (ofa.numberOfInputs() * elegant_pairing(Class1, Class2)) + input;
}


inline size_t &BasicNonIncremental::stateClassVar(int state, int var) {
    return state_class_vars.at(stateClassToID(state, var));
}

inline size_t &BasicNonIncremental::classClassVar(int Class1, int Class2, int input) {
    return class_class_vars.at(CCiToID(Class1, Class2, input));
}

void BasicNonIncremental::init() {

    current_size = partial_solution.size();
    const int &size = ofa.getSize();
    state_class_vars.resize(size * current_size);
    class_class_vars.resize(current_size * current_size * ofa.numberOfInputs());


/// Creating state-class variables
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


    solver->new_vars(max_var);



    /// Encode partial solution

    for (int Class = 0; Class < partial_solution.size(); ++Class) {
        std::vector<CMSat::Lit> clause;
        int state = partial_solution[Class];

        if (!state) initial_covered = true;

        size_t var = stateClassVar(state, Class);
        clause.emplace_back(var, false);
        solver->add_clause(clause);
    }

    /// If partial solution does not contain initial state we have to add "cover clauses"

    if (!initial_covered) {

        buildCoverClauses();
    }

    //// Build frame clauses;

    buildFrameClauses();

    /// Build compatibility clauses;

    buildCompatibilityClauses();


    /// Build successor clauses

    buildSuccessorClauses();

}


void BasicNonIncremental::buildFrameClauses() {
    for (int Class1 = 0; Class1 < current_size; ++Class1) {
        for (int i = 0; i < ofa.numberOfInputs(); ++i) {
            std::vector<CMSat::Lit> clause;
            clause.reserve(current_size);
            for (int Class2 = 0; Class2 < current_size; ++Class2) {
                clause.emplace_back(classClassVar(Class1, Class2, i), false);
            }
            solver->add_clause(clause);
        }
    }
}


void BasicNonIncremental::buildCoverClauses() {
    std::vector<CMSat::Lit> clause;
    clause.reserve(cover_size);
    for (int Class = 0; Class < cover_size; ++Class) {
        size_t var = stateClassVar(0, Class);
        clause.emplace_back(var, false);
    }
    if (!incremented) {
       ++cover_size;
    }
    solver->add_clause(clause);
}



void BasicNonIncremental::step() {
    current_size++;
    incremented = true;
    solver=std::make_unique<CMSat::SATSolver>();
    solver->set_single_run();
    generateIncrementalVars();

    for (int Class = 0; Class < partial_solution.size(); ++Class) {
        std::vector<CMSat::Lit> clause;
        int state = partial_solution[Class];

        if (!state) initial_covered = true;

        size_t var = stateClassVar(state, Class);
        clause.emplace_back(var, false);
        solver->add_clause(clause);
    }
    if (!initial_covered) buildCoverClauses();


    buildFrameClauses();
    buildCompatibilityClauses();
    buildSuccessorClauses();


}


bool BasicNonIncremental::trySolve() {
    std::cout << "\n Number of variables: " << max_var << ".\n";
    auto possible = solver->solve();
    if (possible == CMSat::l_False) {
        std::cout << "Not able to minimize with dfsm_size: " << current_size << ".\n";
        return false;
    } else if (possible == CMSat::l_True) {
        std::cout << "Minimizing with dfsm_size: " << current_size << ".\n";

        return true;

    }

    std::cout << "Undefined result";
    return false;
}

void BasicNonIncremental::computeSolution() {
    result = std::make_unique<DFSM>(ofa.numberOfInputs(), ofa.numberOfOutputs());

    std::vector<CMSat::lbool> model = solver->get_model();
    const int &result_size = current_size;
    const int &size = ofa.getSize();

    result->addStates(result_size);

    std::vector<int> classIDs(result_size);

    bool initial_found = false;
    for (int Class = 0; Class < result_size; ++Class) {
        if (!initial_found && model[stateClassVar(0, Class)] == CMSat::l_True) {
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
                if (model[classClassVar(Class1, Class2, i)] == CMSat::l_True) {
                    result->setSucc(classIDs[Class1], i, classIDs[Class2]);
                }
            }
            bool out_found = false;
            for (int state = 0; state < size; ++state) {
                if (model[stateClassVar(state, Class1)] == CMSat::l_True) {
                    if (ofa.hasTransition(state, i)) {
                        result->setOut(classIDs[Class1], i, ofa.getOut(state, i));
                        out_found = true;
                    }
                }
            }
            if (!out_found) result->setOut(classIDs[Class1], i, 0);
        }
    }
}

void BasicNonIncremental::generateIncrementalVars() {
    const int &size = ofa.getSize();
    state_class_vars.resize(size * current_size);
    class_class_vars.resize(current_size * current_size * ofa.numberOfInputs());

    for (int state = 0; state < size; ++state) {
        stateClassVar(state, current_size - 1) = max_var;
        ++max_var;
    }
    for (int i = 0; i < ofa.numberOfInputs(); ++i) {

        for (int Class2 = 0; Class2 < current_size - 1; ++Class2) {

            classClassVar(current_size - 1, Class2, i) = max_var;
            ++max_var;
            classClassVar(Class2, current_size - 1, i) = max_var;
            ++max_var;
        }
        classClassVar(current_size - 1, current_size - 1, i) = max_var;
        ++max_var;
    }
    solver->new_vars(max_var);
}


void BasicNonIncremental::buildCompatibilityClauses() {

    for (int Class = 0; Class < current_size; ++Class) {
        for (auto pair:compat_matrix.getPairs()) {
            int state1 = pair.first;
            int state2 = pair.second;
            std::vector<CMSat::Lit> clause;
            clause.reserve(2);
            clause.emplace_back(stateClassVar(state1, Class), true);
            clause.emplace_back(stateClassVar(state2, Class), true);

            solver->add_clause(clause);
        }
    }
}


void BasicNonIncremental::buildSuccessorClauses() {
    for (int Class1 = 0; Class1 < current_size; ++Class1) {
        for (int i = 0; i < ofa.numberOfInputs(); ++i) {
            for (int Class2 = 0; Class2 < current_size; ++Class2) {
                for (int state1 = 0; state1 < ofa.getSize(); ++state1) {
                    if (!ofa.hasTransition(state1, i)) continue;
                    auto succs = ofa.getSuccs(state1, i);

                    for (int state2:succs) {

                        std::vector<CMSat::Lit> clause;
                        clause.reserve(3);
                        clause.emplace_back(classClassVar(Class1, Class2, i), true);
                        clause.emplace_back(stateClassVar(state1, Class1), true);
                        clause.emplace_back(stateClassVar(state2, Class2), false);

                        solver->add_clause(clause);
                    }
                }
            }
        }
    }
}