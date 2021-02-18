//
// Created by lazhares on 18/02/2021.
//

#ifndef PMIN_OBSERVATION_MACHINE_BUILDER_H
#define PMIN_OBSERVATION_MACHINE_BUILDER_H


#include <memory>
#include "fsms.h"

using namespace machines;

void buildOFSM(const DFSM& driver, const DFSM& driven, OFSM& obs_fsm);




#endif //PMIN_OBSERVATION_MACHINE_BUILDER_H
