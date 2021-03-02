//
// Created by lazhares on 26/02/2021.
//

#include <numeric>
#include "hopcroft.h"

using namespace SBCMin;

DFSM SBCMin::hopcroft(const DFSM &dfsm) {

    using std::vector;

    DFSM result(dfsm.numberOfInputs(), dfsm.numberOfOutputs());
    const int inputs = dfsm.numberOfInputs();
    const int size = dfsm.getSize();

    typedef vector<vector<int>> refinementData;


    class HopcroftPartition {
        int active_sets = 0;

        int size;

        vector<int> active_states;


        vector<int> to_set;

        vector<bool> target_flag;
        vector<int> active_targets_stack;

        vector<int> inactive_targets_stack;

    public:

        HopcroftPartition(DFSM &dfsm) {
            const int inputs = dfsm.numberOfInputs();
            const int size = dfsm.getSize();
            vector <vector<int>> initial_partition;


            initial_partition.reserve(size);

            for (int state = 0; state < size; ++state) {

                vector<int> matching_partitions(initial_partition.size());
                std::iota(matching_partitions.begin(), matching_partitions.end(), 0);

                for (int in = 0; in < inputs; ++in) {
                    const int out = dfsm.getOut(size, in);
                    int write_head = 0;

                    for (int partition:matching_partitions) {
                        int matching_state = initial_partition[partition].front();
                        if (dfsm.getOut(matching_state, in) == out) {
                            matching_partitions[write_head] = partition;
                            write_head++;
                        }
                        matching_partitions.resize(write_head);
                    }

                }

                if (matching_partitions.empty()) {
                    initial_partition.emplace_back(std::initializer_list<int>({state}));
                } else {
                    int partition = matching_partitions.front();
                    initial_partition[partition].push_back(state);
                }
            }

        }

        HopcroftPartition(int _size, const vector<vector<int>> &initial_partition) :
                size(_size),
                to_set(size) {

            active_states.reserve(size);

            for (vector<int> partition:initial_partition) {
                if (partition.size() == 1) {
                    int state = partition.front();
                    to_set[state] = -state;
                    inactive_targets_stack.push_back(state);
                } else {
                    for (int state:partition) {
                        to_set[state] = active_sets;
                        active_states.push_back(state);
                    }
                    ++active_sets;
                }
            }

            target_flag.resize(active_sets, true);
            target_flag.back() = false;

            active_targets_stack.resize(active_sets - 1);
            std::iota(active_targets_stack.begin(), active_targets_stack.end(), 0);


        }

        inline int &toSet(int state) {
            return to_set[state];
        }

        inline const vector<int> &activeStates() {
            return active_states;
        }

        void makeTarget(vector<int> &set) {
            assert(!set.empty());
            if (set.size() == 1) {
                int state = set.front();
                inactive_targets_stack.push_back(state);
            } else {
                int state = set.front();
                int id = to_set[state];
                assert(id >= 0);
                target_flag[id] = true;
                active_targets_stack.push_back(id);
            }
        }

        void refine(refinementData &data) {
            int active_sets_head = 0;
            active_states.resize(0);
            vector<bool> old_target_flag(target_flag);
            active_targets_stack.resize(0);

            /// For each active set
            for (int j = 0; j < active_sets; ++j) {
                /// We consider the "intersecting" and "non-intersecting" part
                for (int is_intersecting = 0; is_intersecting < 2; ++is_intersecting) {
                    auto set = data[2 * j + is_intersecting];
                    if (set.empty()) continue;

                    /// If the subset is not empty we check if it is a singleton
                    if (set.size() == 1) {
                        int state = set.front();
                        to_set[state] = -state;

                    } else {
                        for (int state:set) {
                            active_states.push_back(state);
                            to_set[state] = active_sets_head;
                        }
                        ++active_sets_head;
                    }

                    /// If the original set was a target:
                    if (old_target_flag[j]) {
                        makeTarget(set);
                    }

                }

                /// If the original set was not a target but has been refined:
                const bool refined = (!data[2 * j].empty()) && (!data[2 * j + 1].empty());
                if (!target_flag[j] && refined) {
                    int intersects;
                    data[2 * j].size() < data[2 * j + 1].size() ? intersects = 0 : intersects = 1;
                    auto set = data[2 * j + intersects];
                    makeTarget(set);
                }
            }
            active_sets = active_sets_head;
            target_flag.resize(active_sets);
        }


    };


    vector<vector<int>> initial_partition;


    initial_partition.reserve(size);

    for (int state = 0; state < size; ++state) {

        vector<int> matching_partitions(initial_partition.size());
        std::iota(matching_partitions.begin(), matching_partitions.end(), 0);

        for (int in = 0; in < inputs; ++in) {
            const int out = dfsm.getOut(size, in);
            int write_head = 0;

            for (int partition:matching_partitions) {
                int matching_state = initial_partition[partition].front();
                if (dfsm.getOut(matching_state, in) == out) {
                    matching_partitions[write_head] = partition;
                    write_head++;
                }
                matching_partitions.resize(write_head);
            }

        }

        if (matching_partitions.empty()) {
            initial_partition.emplace_back(std::initializer_list<int>({state}));
        } else {
            int partition = matching_partitions.front();
            initial_partition[partition].push_back(state);
        }
    }

    HopcroftPartition partition(size, initial_partition);


    while (!target_exists) {

        old_state_to_partition = state_to_partition;
        old_target_partitions = target_partitions;

        target_partitions.pop_back();

        for (int in = 0; in < inputs; ++in) {
            vector<vector<int>> new_classes(active_partitions *
            2);
            for (int state:active_states) {
                int succ = dfsm.getSucc(state, in);
                int curr_partition = state_to_partition[state];
                if (old_state_to_partition[succ] != target) {
                    new_classes[2 * curr_partition].push_back(state);
                } else {
                    new_classes[2 * curr_partition + 1].push_back(state);
                }
            }

            int active_states_head = 0;
            int active_partitions_head = 0;
            int inactive_partitions_head = inactive_partitions;

            for (int j = 0; j < 2 * active_partitions; ++j) {
                vector<int> partition = new_classes[j];
                if (partition.empty()) continue;
                if (partition.size() == 1) {
                    int state = partition.front();
                    state_to_partition[state] = --inactive_partitions_head;
                } else {
                    for (int state:partition) {
                        state_to_partition[state] = active_partitions_head;
                        active_states[active_states_head] = state;
                        ++active_states_head;
                    }
                    ++active_partitions_head;
                }

            }
            active_partitions = active_partitions_head;
            inactive_partitions = inactive_partitions_head;
            active_states.resize(active_states_head);
        }


    }


    return result;
}


HopcroftPartition::HopcroftPartition(DFSM &dfsm) {
    const int inputs = dfsm.numberOfInputs();
    const int size = dfsm.getSize();
    vector <vector<int>> initial_partition;


    initial_partition.
            reserve(size);

    for (
            int state = 0;
            state < size;
            ++state) {

        vector<int> matching_partitions(initial_partition.size());
        std::iota(matching_partitions.begin(), matching_partitions

                          .

                                  end(),

                  0);

        for (
                int in = 0;
                in < inputs;
                ++in) {
            const int out = dfsm.getOut(size, in);
            int write_head = 0;

            for (
                int partition
                    :matching_partitions) {
                int matching_state = initial_partition[partition].front();
                if (dfsm.
                        getOut(matching_state, in
                ) == out) {
                    matching_partitions[write_head] =
                            partition;
                    write_head++;
                }
                matching_partitions.
                        resize(write_head);
            }

        }

        if (matching_partitions.

                empty()

                ) {
            initial_partition.
                    emplace_back(std::initializer_list<int>({state})
            );
        } else {
            int partition = matching_partitions.front();
            initial_partition[partition].
                    push_back(state);
        }
    }

}

HopcroftPartition(int _size, const vector <vector<int>> &initial_partition) :
        size(_size),
        to_set(size) {

    active_states.reserve(size);

    for (vector<int> partition:initial_partition) {
        if (partition.size() == 1) {
            int state = partition.front();
            to_set[state] = -state;
            inactive_targets_stack.push_back(state);
        } else {
            for (int state:partition) {
                to_set[state] = active_sets;
                active_states.push_back(state);
            }
            ++active_sets;
        }
    }

    target_flag.resize(active_sets, true);
    target_flag.back() = false;

    active_targets_stack.resize(active_sets - 1);
    std::iota(active_targets_stack.begin(), active_targets_stack.end(), 0);


}

inline int &toSet(int state) {
    return to_set[state];
}

inline const vector<int> &activeStates() {
    return active_states;
}

void makeTarget(vector<int> &set) {
    assert(!set.empty());
    if (set.size() == 1) {
        int state = set.front();
        inactive_targets_stack.push_back(state);
    } else {
        int state = set.front();
        int id = to_set[state];
        assert(id >= 0);
        target_flag[id] = true;
        active_targets_stack.push_back(id);
    }
}

void refine(refinementData &data) {
    int active_sets_head = 0;
    active_states.resize(0);
    vector<bool> old_target_flag(target_flag);
    active_targets_stack.resize(0);

    /// For each active set
    for (int j = 0; j < active_sets; ++j) {
        /// We consider the "intersecting" and "non-intersecting" part
        for (int is_intersecting = 0; is_intersecting < 2; ++is_intersecting) {
            auto set = data[2 * j + is_intersecting];
            if (set.empty()) continue;

            /// If the subset is not empty we check if it is a singleton
            if (set.size() == 1) {
                int state = set.front();
                to_set[state] = -state;

            } else {
                for (int state:set) {
                    active_states.push_back(state);
                    to_set[state] = active_sets_head;
                }
                ++active_sets_head;
            }

            /// If the original set was a target:
            if (old_target_flag[j]) {
                makeTarget(set);
            }

        }

        /// If the original set was not a target but has been refined:
        const bool refined = (!data[2 * j].empty()) && (!data[2 * j + 1].empty());
        if (!target_flag[j] && refined) {
            int intersects;
            data[2 * j].size() < data[2 * j + 1].size() ? intersects = 0 : intersects = 1;
            auto set = data[2 * j + intersects];
            makeTarget(set);
        }
    }
    active_sets = active_sets_head;
    target_flag.resize(active_sets);
}


};

