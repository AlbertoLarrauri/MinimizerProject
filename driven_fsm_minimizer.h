//
// Created by llarrauriborroto on 28/01/2021.
//

#ifndef PMIN_DRIVEN_FSM_MINIMIZER_H
#define PMIN_DRIVEN_FSM_MINIMIZER_H

#include "fsms.h"
#include<cryptominisat5/cryptominisat.h>

using namespace machines;
namespace minimizers{

    OFSM &buildOFSM(DFSM& driver, DFSM& driven);

    class OFSMMinimizer{
        private:
            OFSM obs;
            CMSat::SATSolver solver;
            std::vector<int> state_class_vars;
            std::vector<int> class_class_vars;
            std::vector<int> size_vars;

    };


}


#endif //PMIN_DRIVEN_FSM_MINIMIZER_H
