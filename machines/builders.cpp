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
    std::vector<int > unexplored;
    unexplored.reserve(size1*size2);
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
                    obs_fsm.getSources(next12, o, t).back() != state12) {
                obs_fsm.addSucc(state12, o, next12);
            }
        }
    }

    return obs_fsm;
}


DFSM SBCMin::buildCascadeDFSM(const DFSM &driver, const DFSM &driven) {
    DFSM result(driver.numberOfInputs(),driven.numberOfOutputs());
    if (driver.getSize() == 0 || driven.getSize() == 0) {
        return result;
    }

    int inputs = driver.numberOfInputs();
    int size1 = driver.getSize();
    int size2 = driven.getSize();
    auto ID = [size2](int state1, int state2) {
        return size2 * state1 + state2;
    };

    std::unordered_map<int , int > state_map;
    std::vector<int > unexplored;
    unexplored.reserve(size1*size2);
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

OFA SBCMin::buildHeuristicOFA(const DFSM &driver, const DFSM &driven) {
    OFA result(driven.numberOfInputs(),driven.numberOfOutputs());
    if (driver.getSize() == 0 || driver.getSize() == 0) {
        return result;
    }

    int inputs1 = driver.numberOfInputs();
    int inputs2 = driven.numberOfInputs();
    int size1 = driver.getSize();
    int size2 = driven.getSize();
    auto productID = [size2](int state1, int state2) {
        return size2 * state1 + state2;
    };
    auto transID = [inputs2](int state2, int input) {
        return inputs2 * state2 + input;
    };

    std::unordered_set<int> visited_pairs;
    std::unordered_set<int> transition_set;
    std::unordered_map<int, int> state_map;



    std::vector<int > unexplored;
    unexplored.reserve(size1*size2);

    state_map[0]=0;
    int max_state=0;

    unexplored.push_back(0);
    visited_pairs.insert(productID(0, 0));
    result.addStates();

    while (!unexplored.empty()) {
        int pair = unexplored.back();
        unexplored.pop_back();

        int state1 = pair / size2;
        int state2 = pair % size2;

        for (int i = 0; i < inputs1; ++i) {



            int o = driver.getOut(state1, i);
            int t = driven.getOut(state2, o);


            int next1 = driver.getSucc(state1, i);
            int next2 = driven.getSucc(state2, o);

            if(!state_map.count(next2)){
                state_map[next2]=++max_state;
                result.addStates();
            }

            int transition=transID(state2,o);

            if(!transition_set.count(transition)){
                int ofa_state=state_map[state2];
                int ofa_next=state_map[next2];
                assert(!result.hasTransition(ofa_state,o));
                result.setTransition(ofa_state,o,t);
                result.addSucc(ofa_state,o,ofa_next);
                transition_set.insert(transition);
            }


            int next_pair = productID(next1, next2);

            if (!visited_pairs.count(next_pair)) {
                visited_pairs.insert(next_pair);
                unexplored.push_back(next_pair);
            }
        }
    }


    return result;
}

DFSM SBCMin::makeRandomDFSM(int in_size, int out_size, int size, int padding, bool rand) {
    DFSM A(in_size, out_size);
    A.addStates(size);
    std::minstd_rand0 defEngine(time(nullptr));
    std::uniform_int_distribution<int> output_generator(0, A.numberOfOutputs() - padding-1);
    std::uniform_int_distribution<int> padding_generator(0, padding-1);
    std::uniform_int_distribution<int> succ_generator(0, size - 1);
    for (int state = 0; state < size; ++state) {
        int pad=padding_generator(defEngine);
        for (int i = 0; i < A.numberOfInputs(); ++i) {
            int o=rand?output_generator(defEngine)+ pad : output_generator(defEngine);
            A.setTrans(state, i, o,succ_generator(defEngine));
        }
    }
    return A;
}



