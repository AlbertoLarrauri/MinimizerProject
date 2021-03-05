//
// Created by llarrauriborroto on 23/02/2021.
//


#include "ofa_minimizer.h"


using namespace SBCMin;


bool OFAMinimizer::run(const OFA &_ofa, int _upper_bound) {

    ofa_ptr = &_ofa;
    buildMatrix();
    upper_bound = _upper_bound;
    lower_bound = compat_matrix().getClique().size();

    init();
    while (true) {
        std::cout<<"Trying to minimize with size: "<<current_size<<"\n";
        if (trySolve()) {
            std::cout<<"Succeeded\n";
            computeSolution();
            return true;
        }
        if (!step()){
            std::cout<<"Failed to minimize with the given bounds";
            return false;
        }
    }
}




void OFAMinimizer::computeSolution() {
    result_ptr = std::make_unique<DFSM>(ofa().numberOfInputs(), ofa().numberOfOutputs());
    std::vector<CMSat::lbool> model = solver->get_model();
    const int &result_size = current_size;
    const int &size = ofa().getSize();

    result_ptr->addStates(result_size);

    std::vector<int> classIDs(result_size);

    bool initial_found = false;
    for (int set = 0; set < result_size; ++set) {
        if (!initial_found && model[stateSetVar(0, set)] == CMSat::l_True) {
            initial_found = true;
            classIDs[set] = 0;
        } else {
            if (!initial_found) {
                classIDs[set] = set + 1;
            } else {
                classIDs[set] = set;
            }
        }

    }

    for (int set1 = 0; set1 < result_size; ++set1) {
        for (int i = 0; i < ofa().numberOfInputs(); ++i) {
            for (int set2 = 0; set2 < result_size; ++set2) {
                if (model[setSetVar(set1, set2, i)] == CMSat::l_True) {
                    result_ptr->setSucc(classIDs[set1], i, classIDs[set2]);
                }
            }
            bool out_found = false;
            for (int state = 0; state < size; ++state) {
                if (model[stateSetVar(state, set1)] == CMSat::l_True) {
                    if (ofa().hasTransition(state, i)) {
                        result_ptr->setOut(classIDs[set1], i, ofa().getOut(state, i));
                        out_found = true;
                    }
                }
            }
            if (!out_found) result_ptr->setOut(classIDs[set1], i, 0);
        }
    }
}

bool OFAMinimizer::trySolve() {
    CMSat::lbool solved = solver->solve(&assumptions);
    return (solved == CMSat::l_True);
}





