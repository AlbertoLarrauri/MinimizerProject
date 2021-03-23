//
// Created by llarrauriborroto on 23/02/2021.
//


#include "ofa_minimizer.h"


using namespace SBCMin;





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

//    std::cout<<"Compatible cover: \n";
    for (int set1 = 0; set1 < result_size; ++set1) {
        for (int i = 0; i < ofa().numberOfInputs(); ++i) {
            for (int set2 = 0; set2 < result_size; ++set2) {
                if (model[setSetVar(set1, set2, i)] == CMSat::l_True) {
                    result_ptr->setSucc(classIDs[set1], i, classIDs[set2]);
//                    std::cout<<set1<<" / "<<i <<" -> "<<set2<<"\n";
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

bool OFAMinimizer::trySolve(const std::vector<CMSat::Lit> &assumptions) {
    CMSat::lbool solved = solver->solve(&assumptions);
    return (solved == CMSat::l_True);
}


bool OFAMinimizerWCustomStrategy::runImpl() {
    auto Query=[&](int size){
        std::cout<<" Trying to minimize with size "<<size<<" ...\n";
        bool value=query(size);
        std::cout<<(value?"Succeeded ":"Failed")<<"\n";
        if(value) computeSolution();
        return value;
    };

    auto solution_size=strat(lower_bound,upper_bound,Query);
    return solution_size.has_value();

}

