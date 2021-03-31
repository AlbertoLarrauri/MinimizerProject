//
// Created by lazhares on 26/02/2021.
//

#include <numeric>
#include "hopcroft.h"

using namespace SBCMin;


DFSMHopcroft::DFSMHopcroft(DFSM &_dfsm) :
        dfsm(_dfsm),
        dfsm_size(dfsm.size()),
        partition_dict(dfsm_size) {

    active_states.reserve(dfsm_size);
    is_set_tracked.reserve(dfsm_size / 2);
    tracked_sets.reserve(dfsm_size / 2);
    tracked_singletons.reserve(dfsm_size);


    const int inputs = dfsm.numberOfInputs();
    Partition initial_partition;


    initial_partition.reserve(dfsm_size);
    for (int state = 0; state < dfsm_size; ++state) {
        std::vector<int> matching_sets(initial_partition.size());
        std::iota(matching_sets.begin(), matching_sets.end(), 0);
        for (int in = 0; in < inputs; ++in) {
            const int out = dfsm.getOut(state, in);
            int write_head = 0;
            for (int setID:matching_sets) {
                int matching_state = initial_partition[setID].front();
                if (dfsm.getOut(matching_state, in) == out) {
                    matching_sets[write_head] = setID;
                    write_head++;
                }
            }
            matching_sets.resize(write_head);
        }
        if (matching_sets.empty()) {
            initial_partition.emplace_back(std::initializer_list<int>({state}));
        } else {
            int set = matching_sets.front();
            initial_partition[set].push_back(state);
        }
    }
    for (std::vector<int> set:initial_partition) {
        addSet(set, true);
    }
}


void DFSMHopcroft::refine(int target) {
    std::vector<int> old_partition_dict = partition_dict;

    int inputs = dfsm.numberOfInputs();
    for (int in = 0; in < inputs; ++in) {
        Partition refinement(2 * number_of_sets);
        for (int state:active_states) {
            const int succ = dfsm.getSucc(state, in);
            const int set = toSetOrSingleton(state);
            assert(set>=0);
            assert(set<number_of_sets);
            if (old_partition_dict[succ] == target) {
                refinement[2 * set + 1].push_back(state);
            } else {
                refinement[2 * set].push_back(state);
            }
        }

        std::vector<bool> to_track(2 * number_of_sets, false);
        for (int set = 0; set < number_of_sets; ++set) {
            bool refined = (!refinement[2 * set].empty()) && (!refinement[2 * set + 1].empty());

            if (isSetTracked(set)) {
                for (int j = 0; j < 2; ++j) {
                    if (!refinement[2 * set + j].empty()) to_track[2 * set + j] = true;
                }
            } else if (refined) {
                int j = refinement[2 * set].size() < refinement[2 * set + 1].size() ? 0 : 1;
                to_track[2 * set + j] = true;
            }
        }

        int old_number_of_sets = number_of_sets;
        clear();

        for (int setID = 0; setID < 2 * old_number_of_sets; ++setID) {
            if(!refinement[setID].empty()) addSet(refinement[setID], to_track[setID]);
        }
    }
}


void DFSMHopcroft::clear() {
    number_of_sets = 0;
    active_states.resize(0);
    is_set_tracked.resize(0);
    tracked_sets.resize(0);
}

int DFSMHopcroft::popTarget() {
    if (!tracked_singletons.empty()) {
        int target = tracked_singletons.back();
        tracked_singletons.pop_back();
        return target;
    } else {
        int target = tracked_sets.back();
        tracked_sets.pop_back();
        assert(is_set_tracked.size()>target);
        is_set_tracked[target] = false;
        return target;
    }
}


void DFSMHopcroft::addSet(std::vector<int> set, bool tracking) {
    assert(!set.empty());
    if (set.size() == 1) {
        int state = set.front();
        toSetOrSingleton(state) = -state-1;
        if (tracking) tracked_singletons.push_back(-state-1);
    } else {
        assert(is_set_tracked.size()==number_of_sets);
        for (int state:set) {
            toSetOrSingleton(state) = number_of_sets;
            active_states.push_back(state);
        }

        is_set_tracked.push_back(tracking);
        if (tracking) tracked_sets.push_back(number_of_sets);

        ++number_of_sets;
    }
}



DFSM DFSMHopcroft::extractDFSM(DFSMHopcroft &algorithm) {
    DFSM& dfsm=algorithm.dfsm;
    DFSM result(dfsm.numberOfInputs(), dfsm.numberOfOutputs());

    std::vector<int> state_to_old_state;
    state_to_old_state.reserve(dfsm.size());

    std::unordered_map<int, int> set_to_state;
    set_to_state.reserve(dfsm.size());
    std::vector<int> unexplored_old;

    int initial_set = algorithm.toSetOrSingleton(0);
    set_to_state[initial_set] = 0;

    unexplored_old.push_back(0);
    int inputs = dfsm.numberOfInputs();
    result.addStates(1);

    while (!unexplored_old.empty()) {
        int old_state = unexplored_old.back();
        int set = algorithm.toSetOrSingleton(old_state);
        int state = set_to_state[set];

        unexplored_old.pop_back();
        for (int in = 0; in < inputs; ++in) {
            int old_succ = dfsm.getSucc(old_state, in);
            int succ_set = algorithm.toSetOrSingleton(old_succ);
            int succ;
            if (set_to_state.count(succ_set)) {
                succ = set_to_state[succ_set];
            } else {
                succ = result.size();
                result.addStates();
                set_to_state[succ_set] = succ;
                unexplored_old.push_back(old_succ);
            }
            result.setSucc(state, in, succ);
            result.setOut(state, in, dfsm.getOut(old_state, in));
        }
    }
    return result;
}

DFSM DFSMHopcroft::minimize(DFSM &dfsm) {
    DFSMHopcroft algorithm(dfsm);

    while (!algorithm.finished()) {
        int target = algorithm.popTarget();
        algorithm.refine(target);
    }

    return extractDFSM(algorithm);
}


OFAHopcroft::OFAHopcroft(OFA &_ofa) :
        ofa(_ofa),
        ofa_size(ofa.size()),
        partition_dict(ofa_size) {

    active_states.reserve(ofa_size);
    is_set_tracked.reserve(ofa_size / 2);
    tracked_sets.reserve(ofa_size / 2);
    tracked_singletons.reserve(ofa_size);


    const int inputs = ofa.numberOfInputs();
    Partition initial_partition;


    initial_partition.reserve(ofa_size);
    for (int state = 0; state < ofa_size; ++state) {
        std::vector<int> matching_sets(initial_partition.size());
        std::iota(matching_sets.begin(), matching_sets.end(), 0);
        for (int in = 0; in < inputs; ++in) {
            std::optional<int> out;
            ofa.hasTransition(state, in)? out=ofa.getOut(state, in): out=std::nullopt;
            int write_head = 0;
            for (int setID:matching_sets) {
                int matching_state = initial_partition[setID].front();
                if(!ofa.hasTransition(matching_state,in) && out) continue;
                if (!ofa.hasTransition(matching_state,in) || ofa.getOut(matching_state, in) == out) {
                    matching_sets[write_head] = setID;
                    write_head++;
                }
            }
            matching_sets.resize(write_head);
        }
        if (matching_sets.empty()) {
            initial_partition.emplace_back(std::initializer_list<int>({state}));
        } else {
            int set = matching_sets.front();
            initial_partition[set].push_back(state);
        }
    }
    for (std::vector<int> set:initial_partition) {
        addSet(set, true);
    }
}


void OFAHopcroft::refine(int target) {
    std::vector<int> old_partition_dict = partition_dict;

    int inputs = ofa.numberOfInputs();
    for (int in = 0; in < inputs; ++in) {
        Partition refinement(2 * number_of_sets);
        for (int state:active_states) {
            bool intersects=false;
            if(ofa.hasTransition(state,in)){
                for(int succ:ofa.getSuccs(state,in)){
                    if (old_partition_dict[succ] == target) intersects= true;
                }
            }
            const int set = toSetOrSingleton(state);
            assert(set>=0);
            assert(set<number_of_sets);
            if (intersects) {
                refinement[2 * set + 1].push_back(state);
            } else {
                refinement[2 * set].push_back(state);
            }
        }

        std::vector<bool> to_track(2 * number_of_sets, false);
        for (int set = 0; set < number_of_sets; ++set) {
            bool refined = (!refinement[2 * set].empty()) && (!refinement[2 * set + 1].empty());

            if (isSetTracked(set)) {
                for (int j = 0; j < 2; ++j) {
                    if (!refinement[2 * set + j].empty()) to_track[2 * set + j] = true;
                }
            } else if (refined) {
                int j = refinement[2 * set].size() < refinement[2 * set + 1].size() ? 0 : 1;
                to_track[2 * set + j] = true;
            }
        }

        int old_number_of_sets = number_of_sets;
        clear();

        for (int setID = 0; setID < 2 * old_number_of_sets; ++setID) {
            if(!refinement[setID].empty()) addSet(refinement[setID], to_track[setID]);
        }
    }
}


void OFAHopcroft::clear() {
    number_of_sets = 0;
    active_states.resize(0);
    is_set_tracked.resize(0);
    tracked_sets.resize(0);
}

int OFAHopcroft::popTarget() {
    if (!tracked_singletons.empty()) {
        int target = tracked_singletons.back();
        tracked_singletons.pop_back();
        return target;
    } else {
        int target = tracked_sets.back();
        tracked_sets.pop_back();
        assert(is_set_tracked.size()>target);
        is_set_tracked[target] = false;
        return target;
    }
}


void OFAHopcroft::addSet(std::vector<int> set, bool tracking) {
    assert(!set.empty());
    if (set.size() == 1) {
        int state = set.front();
        toSetOrSingleton(state) = -state-1;
        if (tracking) tracked_singletons.push_back(-state-1);
    } else {
        assert(is_set_tracked.size()==number_of_sets);
        for (int state:set) {
            toSetOrSingleton(state) = number_of_sets;
            active_states.push_back(state);
        }

        is_set_tracked.push_back(tracking);
        if (tracking) tracked_sets.push_back(number_of_sets);

        ++number_of_sets;
    }
}



OFA OFAHopcroft::extractOFA(OFAHopcroft &algorithm) {
    OFA& ofa=algorithm.ofa;
    OFA result(ofa.numberOfInputs(), ofa.numberOfOutputs());

    std::vector<int> state_to_old_state;
    state_to_old_state.reserve(ofa.size());

    std::unordered_map<int, int> set_to_state;
    set_to_state.reserve(ofa.size());
    std::vector<int> unexplored_old;

    int initial_set = algorithm.toSetOrSingleton(0);
    set_to_state[initial_set] = 0;

    unexplored_old.push_back(0);
    int inputs = ofa.numberOfInputs();
    result.addStates(1);

    while (!unexplored_old.empty()) {
        int old_state = unexplored_old.back();
        int set = algorithm.toSetOrSingleton(old_state);
        int state = set_to_state[set];

        unexplored_old.pop_back();
        for (int in = 0; in < inputs; ++in) {
            if (!ofa.hasTransition(old_state, in)) continue;
            int out = ofa.getOut(old_state, in);
            result.setTransition(state, in, out);
            for (int old_succ:ofa.getSuccs(old_state, in)) {

                int succ_set = algorithm.toSetOrSingleton(old_succ);
                int succ;
                if (set_to_state.count(succ_set)) {
                    succ = set_to_state[succ_set];
                } else {
                    succ = result.size();
                    result.addStates();
                    set_to_state[succ_set] = succ;
                    unexplored_old.push_back(old_succ);
                }

                if (!result.hasSources(succ,in,out) || !(result.getSources(succ, in, out).back() == state)) {
                    result.addSucc(state, in, succ);

                }
            }
        }
    }
    return result;
}

OFA OFAHopcroft::minimize(OFA &ofa) {
    OFAHopcroft algorithm(ofa);

    while (!algorithm.finished()) {
        int target = algorithm.popTarget();
        algorithm.refine(target);
    }

    return extractOFA(algorithm);
}

