//
// Created by lazhares on 18/02/2021.
//

#include <unordered_set>
#include <deque>
#include "builders.h"

using namespace SBCMin;

OFA SBCMin::buildOFA(const DFSM& driver, const DFSM& driven){
    OFA obs_fsm(driven.numberOfInputs(), driven.numberOfOutputs());
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
    std::vector<int > unexplored(size1*size2);
    int max_state = 0;
    unexplored.push_back(0);
    state_map[ID(0, 0)] = 0;
    obs_fsm.addStates(1);
    while (!unexplored.empty()) {
        int pair = unexplored.back();
        unexplored.pop_back();

        int state12 = state_map[pair];

        int state1 = pair / size2;
        int state2 = pair % size2;


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
                unexplored.push_back(next_pair);
            } else next12 = state_map[next_pair];


            if (!obs_fsm.hasTransition(state_map[pair], o)) {
                obs_fsm.setTransition(state12, o, t);
            }
            if (!obs_fsm.hasSources(next12, o, t) ||
                obs_fsm.sources(next12, o, t).back() != state12) {
                obs_fsm.addSucc(state12, o, next12);
            }
        }
    }

    return obs_fsm;
}


DFSM SBCMin::buildCascadeDFSM(const DFSM &driver, const DFSM &driven) {
    DFSM result(driver.numberOfInputs(),driven.numberOfOutputs());
    if (driver.getSize() == 0 || driver.getSize() == 0) {
        return result;
    }

    int inputs = driver.numberOfInputs();
    int size1 = driver.getSize();
    int size2 = driven.getSize();
    auto ID = [size2](int state1, int state2) {
        return size2 * state1 + state2;
    };

    std::unordered_map<int , int > state_map;
    std::vector<int > unexplored(size1*size2);
    int max_state = 0;
    unexplored.push_back(0);
    state_map[ID(0, 0)] = 0;
    result.addStates();

    while (!unexplored.empty()) {
        int pair = unexplored.back();
        unexplored.pop_back();

        int state12 = state_map[pair];

        int state1 = pair / size2;
        int state2 = pair % size2;

        for (int i = 0; i < inputs; ++i) {
            int o = driver.getOut(state1, i);
            int t = driven.getOut(state2, o);
            int next1 = driver.getSucc(state1, i);
            int next2 = driven.getSucc(state2, o);
            int next12;
            int next_pair = ID(next1, next2);

            if (!state_map.count(next_pair)) {
                result.addStates();
                state_map[next_pair] = ++max_state;
                next12 = max_state;
                unexplored.push_back(next_pair);
            } else next12 = state_map[next_pair];

            result.setSucc(state12,i, next12);
            result.setOut(state12,i, t);
        }
    }


    return result;
}


