//
// Created by lazhares on 18/02/2021.
//

#include <unordered_set>
#include "observation_machine_builder.h"

void buildOFSM(const DFSM& driver, const DFSM& driven, OFSM& obs_fsm ){
    obs_fsm.reset(driven.numberOfInputs(),driven.numberOfOutputs());

    if (driver.getSize() == 0 || driver.getSize() == 0) {
        return;
    }

    unsigned inputs = driver.numberOfInputs();
    unsigned size1 = driver.getSize();
    unsigned size2 = driven.getSize();
    auto ID = [size2](unsigned state1, unsigned state2) {
        return size2 * state1 + state2;
    };

    std::unordered_map<unsigned, unsigned> state_map;
    std::unordered_set<unsigned> unexplored(driven.getSize() * driver.getSize());
    unsigned max_state = 0;
    unexplored.insert(0);
    state_map[ID(0, 0)] = 0;
    obs_fsm.addStates(1);
    while (!unexplored.empty()) {
        unsigned pair = *unexplored.begin();

        unsigned state12 = state_map[pair];

        unsigned state1 = pair / size2;
        unsigned state2 = pair % size2;

        unsigned unexplored_succs = 0;

        for (unsigned i = 0; i < inputs; ++i) {
            unsigned o = driver.getOut(state1, i);
            unsigned t = driven.getOut(state2, o);
            unsigned next1 = driver.getSucc(state1, i);
            unsigned next2 = driven.getSucc(state2, o);
            unsigned next12;
            unsigned next_pair = ID(next1, next2);

            if (!state_map.count(next_pair)) {
                obs_fsm.addStates();
                state_map[next_pair] = ++max_state;
                next12 = max_state;
                unexplored.insert(next_pair);
            } else next12 = state_map[next_pair];


            if (!obs_fsm.hasTransition(state_map[pair], o)) {
                obs_fsm.setTransition(state12, o, t);
            }
            if (!obs_fsm.hasSources(next12, o, t) ||
                obs_fsm.sources(next12, o, t).back() != state12) {
                obs_fsm.addSucc(state12, o, next12);
            }
        }

        unexplored.erase(pair);
        obs_fsm.addStates(unexplored_succs);

    }


    return;
}
