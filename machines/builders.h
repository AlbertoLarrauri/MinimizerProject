//
// Created by lazhares on 18/02/2021.
//

#ifndef PMIN_BUILDERS_H
#define PMIN_BUILDERS_H


#include <memory>
#include "machines.h"

namespace SBCMin {
    namespace builders {
        OFA buildOFSM(const DFSM &driver, const DFSM &driven);

        DFSM buildCascadeDFSM(const DFSM &driver, const DFSM &driven);


    }

}





#endif //PMIN_BUILDERS_H
