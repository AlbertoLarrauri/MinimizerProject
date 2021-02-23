//
// Created by lazhares on 18/02/2021.
//

#include <unordered_set>
#include "machine_builders.h"


machines::OFSM machines::builders::buildOFSM(const DFSM& driver, const DFSM& driven){
    OFSM obs_fsm(driven.numberOfInputs(),driven.numberOfOutputs());
    if (driver.getSize() == 0 || driver.getSize() == 0) {
        return obs_fsm;
    }

    int inputs = driver.numberOfInputs();
    int size1 = driver.getSize();
    int size2 = driven.getSize();
    auto ID = [size2](int state1, int state2) {
        return size2 * state1 + state2;
    };

    std::unordered_map<int , int > state_map;
    std::unordered_set<int > unexplored(driven.getSize() * driver.getSize());
    int max_state = 0;
    unexplored.insert(0);
    state_map[ID(0, 0)] = 0;
    obs_fsm.addStates(1);
    while (!unexplored.empty()) {
        int pair = *unexplored.begin();

        int state12 = state_map[pair];

        int state1 = pair / size2;
        int state2 = pair % size2;

        int unexplored_succs = 0;

        for (int i = 0; i < inputs; ++i) {
            int o = driver.getOut(state1, i);
            int t = driven.getOut(state2, o);
            int next1 = driver.getSucc(state1, i);
            int next2 = driven.getSucc(state2, o);
            int next12;
            int next_pair = ID(next1, next2);

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

    return obs_fsm;
}

