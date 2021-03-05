//
// Created by llarrauriborroto on 23/02/2021.
//

#include "basic_incremental.h"
#include "../ofa_minimizer.h"
#include "../compat_matrix.h"


using namespace SBCMin;
using namespace OFAMinimizers;



void BasicIncremental::init() {
    solver=std::make_unique<CMSat::SATSolver>();
    partial_solution=compat_matrix().getClique();
    current_size = partial_solution.size();
    const int &size = ofa().getSize();
    resize(current_size);


/// Creating state-class variables

    for (int Class = 0; Class < current_size; ++Class) {
        for (int state = 0; state < size; ++state) {
            stateSetVar(state, Class) = max_var;
            ++max_var;
        }

        for (int Class2 = 0; Class2 < current_size; ++Class2) {
            for (int i = 0; i < ofa().numberOfInputs(); ++i) {
                setSetVar(Class, Class2, i) = max_var;
                ++max_var;
            }
        }
    }

    /// Adding dfsm_size assumption

    size_vars.push_back(max_var);
    ++max_var;


    solver->new_vars(max_var);



    /// Encode partial solution

    for (int Class = 0; Class < partial_solution.size(); ++Class) {
        std::vector<CMSat::Lit> clause;
        int state = partial_solution[Class];

        if (!state) initial_covered = true;

        size_t var = stateSetVar(state, Class);
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

    for (int Class = 0; Class < current_size; ++Class) {
        for (auto pair:compat_matrix().
        getPairs()) {
            int state1 = pair.first;
            int state2 = pair.second;
            std::vector<CMSat::Lit> clause;
            clause.reserve(2);
            clause.emplace_back(stateSetVar(state1, Class), true);
            clause.emplace_back(stateSetVar(state2, Class), true);

            solver->add_clause(clause);
        }
    }

    /// Build successor clauses

    for (int Class1 = 0; Class1 < current_size; ++Class1) {
        for (int i = 0; i < ofa().numberOfInputs(); ++i) {
            for (int Class2 = 0; Class2 < current_size; ++Class2) {
                for (int state1 = 0; state1 < size; ++state1) {
                    if (!ofa().hasTransition(state1, i)) continue;
                    auto succs = ofa().getSuccs(state1, i);

                    for (int state2:succs) {

                        std::vector<CMSat::Lit> clause;
                        clause.reserve(3);
                        clause.emplace_back(setSetVar(Class1, Class2, i), true);
                        clause.emplace_back(stateSetVar(state1, Class1), true);
                        clause.emplace_back(stateSetVar(state2, Class2), false);

                        solver->add_clause(clause);
                    }
                }
            }
        }
    }

    assumptions.emplace_back(size_vars.back(), false);

}


void BasicIncremental::buildFrameClauses() {
    for (int Class1 = 0; Class1 < current_size; ++Class1) {
        for (int i = 0; i < ofa().numberOfInputs(); ++i) {
            std::vector<CMSat::Lit> clause;
            clause.reserve(current_size + 1);
            for (int Class2 = 0; Class2 < current_size; ++Class2) {
                clause.emplace_back(setSetVar(Class1, Class2, i), false);
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
        size_t var = stateSetVar(0, Class);
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
    const int &size = ofa().getSize();
    resize(current_size);
//@ Creating state-class variables

    for (int state = 0; state < size; ++state) {
        stateSetVar(state, current_size - 1) = max_var;
        ++max_var;
    }
    for (int i = 0; i < ofa().numberOfInputs(); ++i) {

        for (int Class2 = 0; Class2 < current_size - 1; ++Class2) {

            setSetVar(current_size - 1, Class2, i) = max_var;
            ++max_var;
            setSetVar(Class2, current_size - 1, i) = max_var;
            ++max_var;
        }
        setSetVar(current_size - 1, current_size - 1, i) = max_var;
        ++max_var;
    }

    size_vars.push_back(max_var);
    ++max_var;
    solver->new_vars(max_var - initial_vars);
}

void BasicIncremental::buildIncrementalClauses() {
    const int &size = ofa().getSize();
//       std::cout<<"Incompatibility clauses: \n";
    for (auto pair:compat_matrix().
    getPairs()) {
        int state1 = pair.first;
        int state2 = pair.second;
//        assert(compat_matrix().areIncompatible(state1, state2));
        std::vector<CMSat::Lit> clause;
        clause.reserve(2);
        clause.emplace_back(stateSetVar(state1, current_size - 1), true);
        clause.emplace_back(stateSetVar(state2, current_size - 1), true);
//            std::cout<<clause<<"\n";
        solver->add_clause(clause);
    }

//        std::cout<<"Successor clauses: \n";
    for (int Class = 0; Class < current_size; ++Class) {
        for (int i = 0; i < ofa().numberOfInputs(); ++i) {
            for (int state1 = 0; state1 < size; ++state1) {
                if (!ofa().hasTransition(state1, i)) continue;

                auto succs = ofa().getSuccs(state1, i);
                for (int state2:succs) {
                    std::vector<CMSat::Lit> clause1;
                    std::vector<CMSat::Lit> clause2;
                    clause1.reserve(3);
                    clause2.reserve(3);
                    clause1.emplace_back(setSetVar(Class, current_size - 1, i), true);
                    clause1.emplace_back(stateSetVar(state1, Class), true);
                    clause1.emplace_back(stateSetVar(state2, current_size - 1), false);
                    solver->add_clause(clause1);
                    clause2.emplace_back(setSetVar(current_size - 1, Class, i), true);
                    clause2.emplace_back(stateSetVar(state1, current_size - 1), true);
                    clause2.emplace_back(stateSetVar(state2, Class), false);

                    solver->add_clause(clause2);
                }
            }
        }
    }
}

bool BasicIncremental::step() {
    if(current_size==upper_bound) return false;
    current_size++;
    incremented = true;
    generateIncrementalVars();
    buildFrameClauses();
    if (!initial_covered) buildCoverClauses();
    buildIncrementalClauses();
    assumptions.resize(0);
    assumptions.emplace_back(size_vars.back(), false);
    return true;
}




