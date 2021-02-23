//
// Created by lazhares on 18/02/2021.
//

#ifndef PMIN_MACHINE_BUILDERS_H
#define PMIN_MACHINE_BUILDERS_H


#include <memory>
#include "fsms.h"

namespace machines {
    namespace builders {
        OFSM buildOFSM(const DFSM &driver, const DFSM &driven);

        DFSM buildCascadeDFSM(const DFSM &driver, const DFSM &driven);


    }

}





#endif //PMIN_MACHINE_BUILDERS_H
