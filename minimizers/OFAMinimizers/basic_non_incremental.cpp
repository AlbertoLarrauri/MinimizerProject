//
// Created by lazhares on 25/02/2021.
//

#include "basic_non_incremental.h"

using namespace SBCMin::OFAMinimizers;



void BasicNonIncremental::init() {
    solver=std::make_unique<CMSat::SATSolver>();
    partial_solution=compat_matrix().getClique();
    current_size = partial_solution.size();
    const int &size = ofa().size();
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

    buildCompatibilityClauses();


    /// Build successor clauses

    buildSuccessorClauses();

}


void BasicNonIncremental::buildFrameClauses() {
    for (int Class1 = 0; Class1 < current_size; ++Class1) {
        for (int i = 0; i < ofa().numberOfInputs(); ++i) {
            std::vector<CMSat::Lit> clause;
            clause.reserve(current_size);
            for (int Class2 = 0; Class2 < current_size; ++Class2) {
                clause.emplace_back(setSetVar(Class1, Class2, i), false);
            }
            solver->add_clause(clause);
        }
    }
}


void BasicNonIncremental::buildCoverClauses() {
    std::vector<CMSat::Lit> clause;
    clause.reserve(cover_size);
    for (int Class = 0; Class < cover_size; ++Class) {
        size_t var = stateSetVar(0, Class);
        clause.emplace_back(var, false);
    }
    if (!incremented) {
       ++cover_size;
    }
    solver->add_clause(clause);
}



bool BasicNonIncremental::step() {
    if(current_size==upper_bound) return false;
    current_size++;
    incremented = true;
    solver=std::make_unique<CMSat::SATSolver>();
    solver->set_single_run();
    generateIncrementalVars();

    for (int Class = 0; Class < partial_solution.size(); ++Class) {
        std::vector<CMSat::Lit> clause;
        int state = partial_solution[Class];

        if (!state) initial_covered = true;

        size_t var = stateSetVar(state, Class);
        clause.emplace_back(var, false);
        solver->add_clause(clause);
    }
    if (!initial_covered) buildCoverClauses();


    buildFrameClauses();
    buildCompatibilityClauses();
    buildSuccessorClauses();

    return true;
}



void BasicNonIncremental::generateIncrementalVars() {
    const int &size = ofa().size();
    resize(current_size);

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
    solver->new_vars(max_var);
}


void BasicNonIncremental::buildCompatibilityClauses() {

    for (int Class = 0; Class < current_size; ++Class) {
        for (auto [state1,state2]:compat_matrix().getPairs()) {
            std::vector<CMSat::Lit> clause;
            clause.reserve(2);
            clause.emplace_back(stateSetVar(state1, Class), true);
            clause.emplace_back(stateSetVar(state2, Class), true);

            solver->add_clause(clause);
        }
    }
}


void BasicNonIncremental::buildSuccessorClauses() {
    for (int Class1 = 0; Class1 < current_size; ++Class1) {
        for (int i = 0; i < ofa().numberOfInputs(); ++i) {
            for (int Class2 = 0; Class2 < current_size; ++Class2) {
                for (int state1 = 0; state1 < ofa().size(); ++state1) {
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
}


bool BasicNonIncremental::runImpl() {
    init();
    while (true) {
        std::cout << "Trying to minimize with size_impl: " << current_size << "\n";
        if (trySolve()) {
            std::cout << "Succeeded\n";
            computeSolution();
            return true;
        }
        if (!step()) {
            std::cout << "Failed to minimize with the given bounds";
            return false;
        }
    }

}