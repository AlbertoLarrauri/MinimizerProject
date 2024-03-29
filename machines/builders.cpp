//
// Created by lazhares on 18/02/2021.
//

#include <unordered_set>
#include <deque>
#include <algorithm>
#include "builders.h"
#include "../minimizers/hopcroft.h"
#include<boost/dynamic_bitset.hpp>
#include<map>


using namespace SBCMin;

OFA SBCMin::buildOFA(const DFSM &driver, const DFSM &driven) {
    OFA obs_fsm(driven.numberOfInputs(), driven.numberOfOutputs());
    if (driver.size() == 0 || driver.size() == 0) {
        return obs_fsm;
    }

    int inputs = driver.numberOfInputs();
    int size1 = driver.size();
    int size2 = driven.size();

    std::vector<int> state_map(size1 * size2, 0);
    std::vector<bool> visited_pairs(size1 * size2, false);

    auto ID = [size2](int state1, int state2) {
        return size2 * state1 + state2;
    };

    auto setOFAState = [&](int state1, int state2, int ofa_state) {
        state_map[ID(state1, state2)] = ofa_state;
        visited_pairs[ID(state1, state2)] = true;
    };

    auto getOFAState = [&](int state1, int state2) {
        return state_map[ID(state1, state2)];
    };

    auto isPairVisited = [&](int state1, int state2) {
        return visited_pairs[ID(state1, state2)];
    };


    std::vector<std::pair<int, int>> unexplored;
    unexplored.reserve(size1 * size2);
    int max_state = 0;
    unexplored.emplace_back(0, 0);
    setOFAState(0, 0, 0);
    obs_fsm.addStates(1);
    while (!unexplored.empty()) {
        auto[state1, state2] = unexplored.back();
        unexplored.pop_back();
        int state12 = getOFAState(state1, state2);

        for (int i = 0; i < inputs; ++i) {
            int o = driver.getOut(state1, i);
            int t = driven.getOut(state2, o);
            int next1 = driver.getSucc(state1, i);
            int next2 = driven.getSucc(state2, o);
            int next12 = 0;
            if (!isPairVisited(next1, next2)) {
                obs_fsm.addStates();
                setOFAState(next1, next2, ++max_state);
                next12 = max_state;
                unexplored.emplace_back(next1, next2);
            } else next12 = getOFAState(next1, next2);


            if (!obs_fsm.hasTransition(state12, o)) {
                obs_fsm.setOut(state12, o, t);
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
    DFSM result(driver.numberOfInputs(), driven.numberOfOutputs());
    if (driver.size() == 0 || driven.size() == 0) {
        return result;
    }

    int inputs = driver.numberOfInputs();
    int size1 = driver.size();
    int size2 = driven.size();
    auto ID = [size2](int state1, int state2) {
        return size2 * state1 + state2;
    };

    std::unordered_map<int, int> state_map;
    std::vector<int> unexplored;
    unexplored.reserve(size1 * size2);
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

            result.setSucc(state12, i, next12);
            result.setOut(state12, i, t);
        }
    }


    return result;
}

int SBCMin::getStateCoverage(const DFSM &driver, const DFSM &driven) {
    if (driver.size() == 0 || driven.size() == 0) {
        return 0;
    }
    std::vector<bool> covered(driven.size(), false);
    int coverage = 0;
    int inputs = driver.numberOfInputs();
    int size1 = driver.size();
    int size2 = driven.size();
    auto ID = [size2](int state1, int state2) {
        return size2 * state1 + state2;
    };

    std::unordered_set<int> reached_pairs;
    std::vector<int> unexplored;
    unexplored.reserve(size1 * size2);
    covered[0] = true;
    ++coverage;
    unexplored.push_back(0);
    reached_pairs.insert(ID(0, 0));

    while (!unexplored.empty()) {
        int pair = unexplored.back();
        unexplored.pop_back();


        int state1 = pair / size2;
        int state2 = pair % size2;

        for (int i = 0; i < inputs; ++i) {
            int o = driver.getOut(state1, i);
            int next1 = driver.getSucc(state1, i);
            int next2 = driven.getSucc(state2, o);
            int next_pair = ID(next1, next2);

            if (!reached_pairs.count(next_pair)) {
                unexplored.push_back(next_pair);
                reached_pairs.insert(next_pair);
                if (!covered[next2]) {
                    covered[next2] = true;
                    ++coverage;
                }
            }

        }
    }


    return coverage;
}

OFA SBCMin::buildHeuristicOFA(const DFSM &driver, const DFSM &driven) {
    OFA result(driven.numberOfInputs(), driven.numberOfOutputs());
    if (driver.size() == 0 || driver.size() == 0) {
        return result;
    }

    int inputs1 = driver.numberOfInputs();
    int inputs2 = driven.numberOfInputs();
    int size1 = driver.size();
    int size2 = driven.size();
    auto productID = [size2](int state1, int state2) {
        return size2 * state1 + state2;
    };
    auto transID = [inputs2](int state2, int input) {
        return inputs2 * state2 + input;
    };

    std::unordered_set<int> visited_pairs;
    std::unordered_set<int> transition_set;
    std::unordered_map<int, int> state_map;


    std::vector<int> unexplored;
    unexplored.reserve(size1 * size2);

    state_map[0] = 0;
    int max_state = 0;

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
            if(o>=driven.numberOfInputs()) continue;
            int t = driven.getOut(state2, o);


            int next1 = driver.getSucc(state1, i);
            int next2 = driven.getSucc(state2, o);

            if (!state_map.count(next2)) {
                state_map[next2] = ++max_state;
                result.addStates();
            }

            int transition = transID(state2, o);

            if (!transition_set.count(transition)) {
                int ofa_state = state_map[state2];
                int ofa_next = state_map[next2];
                assert(!result.hasTransition(ofa_state, o));
                result.setOut(ofa_state, o, t);
                result.addSucc(ofa_state, o, ofa_next);
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


DFSM SBCMin::makeRandomDFSM(int in_size, int out_size, int size, int padding) {
    bool rand = padding ? true : false;
    DFSM A(in_size, out_size);
    A.addStates(size);
    std::minstd_rand0 defEngine(time(nullptr));
    std::uniform_int_distribution<int> output_generator(0, A.numberOfOutputs() - padding - 1);
    std::uniform_int_distribution<int> padding_generator(0, padding - 1);
    std::uniform_int_distribution<int> succ_generator(0, size - 1);
    for (int state = 0; state < size; ++state) {
        int pad = padding_generator(defEngine);
        for (int i = 0; i < A.numberOfInputs(); ++i) {
            int o = rand ? output_generator(defEngine) + pad : output_generator(defEngine);
            A.setTrans(state, i, o, succ_generator(defEngine));
        }
    }
    return A;
}

OFA SBCMin::buildOFA(const DFSM &driver, const DFSM &driven, std::vector<int> &state_map) {
    OFA obs_fsm(driven.numberOfInputs(), driven.numberOfOutputs());
    if (driver.size() == 0 || driver.size() == 0) {
        return obs_fsm;
    }

    int inputs = driver.numberOfInputs();
    int size1 = driver.size();
    int size2 = driven.size();

    state_map.resize(size1 * size2, 0);
    std::vector<bool> visited_pairs(size1 * size2, false);

    auto ID = [size2](int state1, int state2) {
        return size2 * state1 + state2;
    };

    auto setOFAState = [&](int state1, int state2, int ofa_state) {
        state_map[ID(state1, state2)] = ofa_state;
        visited_pairs[ID(state1, state2)] = true;
    };

    auto getOFAState = [&](int state1, int state2) {
        return state_map[ID(state1, state2)];
    };

    auto isPairVisited = [&](int state1, int state2) {
        return visited_pairs[ID(state1, state2)];
    };


    std::vector<std::pair<int, int>> unexplored;
    unexplored.reserve(size1 * size2);
    int max_state = 0;
    unexplored.emplace_back(0, 0);
    setOFAState(0, 0, 0);
    obs_fsm.addStates(1);
    while (!unexplored.empty()) {
        auto[state1, state2] = unexplored.back();
        unexplored.pop_back();
        int state12 = getOFAState(state1, state2);

        for (int i = 0; i < inputs; ++i) {
            int o = driver.getOut(state1, i);
            int t = driven.getOut(state2, o);
            int next1 = driver.getSucc(state1, i);
            int next2 = driven.getSucc(state2, o);
            int next12 = 0;
            if (!isPairVisited(next1, next2)) {
                obs_fsm.addStates();
                setOFAState(next1, next2, ++max_state);
                next12 = max_state;
                unexplored.emplace_back(next1, next2);
            } else next12 = getOFAState(next1, next2);


            if (!obs_fsm.hasTransition(state12, o)) {
                obs_fsm.setOut(state12, o, t);
            }
            if (!obs_fsm.hasSources(next12, o, t) ||
                obs_fsm.getSources(next12, o, t).back() != state12) {
                obs_fsm.addSucc(state12, o, next12);
            }
        }
    }

    return obs_fsm;
}



//// EEE

NFA SBCMin::makeDeterministic(const NFA &A) {

    typedef boost::dynamic_bitset<> bitset;

    NFA Result(A.numberOfInputs());

    std::vector<std::unique_ptr<bitset>> state_sets;

    struct BitsetComparator{
        bool operator()(const bitset* const& lhs, const bitset* const& rhs ) const {
            return *lhs<*rhs;
        }
    };

    std::map<bitset*, uint32_t, BitsetComparator> state_map;

    Result.addStates();

    state_sets.emplace_back(new bitset(A.size(),1));
    auto initial_set = state_sets.back().get();

    state_map.emplace(initial_set,0);

    std::vector<bitset*> stack={initial_set};

    while(!stack.empty()){
        auto source_set_ptr = stack.back();
        const auto& source_set = *source_set_ptr;
        stack.pop_back();
        auto source_state_result = state_map.at(source_set_ptr);

        for(uint32_t in=0; in<A.numberOfInputs(); ++in){
            bitset succ_set(A.size());

            auto source_state=source_set.find_first();
            while(source_state!=bitset::npos){
                for (auto succ_state: A.getSuccs(source_state, in)) {
                    succ_set[succ_state] = true;
                }
                source_state=source_set.find_next(source_state);
            }

            if(succ_set.none()) continue;

            if(state_map.count(&succ_set)){
                auto succ_state = state_map.at(&succ_set);
                Result.addSucc(source_state_result, in, succ_state);
            } else {
                state_sets.emplace_back(new bitset(std::move(succ_set)));

                auto succ_set_ptr = state_sets.back().get();
                stack.push_back(succ_set_ptr);

                auto succ_state= Result.size();
                state_map[succ_set_ptr]=succ_state;


                Result.addStates();
                Result.addSucc(source_state_result, in, succ_state);

            }
        }

    }


    return Result;
}

////

NFA SBCMin::getOutputNFA(const DFSM &H, std::optional<uint32_t> alphabet_size) {
    uint32_t number_of_outputs;
    if(!alphabet_size){
        number_of_outputs=H.numberOfOutputs();
    } else {
        number_of_outputs = *alphabet_size;
    }

    NFA A(number_of_outputs);
    A.addStates(H.size());

    for(uint32_t s=0; s<H.size(); ++s){
        for(uint32_t in=0; in<H.numberOfInputs();++in ){
            auto out= H.getOut(s,in);
            if(out>=number_of_outputs) continue;
            auto succ= H.getSucc(s,in);
            A.addSucc(s,out,succ);
        }

    }

    return A;
}

////

OFA SBCMin::buildOFA(const NFA &A, const DFSM &T) {
    assert(A.numberOfInputs() >= T.numberOfInputs());
    OFA ofa(T.numberOfInputs(), T.numberOfOutputs());

    auto pairToID = [&](uint32_t a, uint32_t s) {
        return a * T.size() + s;
    };

    ofa.addStates();
    std::unordered_map<uint32_t, uint32_t> pairs_to_states =
            { {0,0} };

    std::vector<uint32_t> stack={0};

    while(!stack.empty()){
        auto pair=stack.back();
        stack.pop_back();

        auto ofa_state= pairs_to_states.at(pair);

        uint32_t a= pair/ T.size();
        uint32_t s= pair% T.size();

        for(uint32_t in=0; in<T.numberOfInputs(); ++in){
            if(!A.hasSymbol(a,in)) continue;
            auto& A_succs=A.getSuccs(a,in);
            auto& s_succ= T.getSucc(s,in);
            auto& out= T.getOut(s,in);

            ofa.setOut(ofa_state, in, out);

            for(auto a_succ:A_succs){
                auto pair_succ=pairToID(a_succ, s_succ);
                uint32_t ofa_succ;
                if(pairs_to_states.count(pair_succ)){
                    ofa_succ=pairs_to_states.at(pair_succ);
                } else {
                    ofa_succ=ofa.size();
                    ofa.addStates();
                    pairs_to_states[pair_succ]=ofa_succ;
                    stack.push_back(pair_succ);
                }
                ofa.addSucc(ofa_state,in,ofa_succ);

            }

        }
    }


    return ofa;
}

////
OFA SBCMin::buildOFAV2(const DFSM &driver, const DFSM &driven) {
    auto A= getOutputNFA(driver, driven.numberOfInputs());
    return buildOFA(A,driven);
}

OFA SBCMin::buildOFAKimNewborn(const DFSM &driver, const DFSM &driven) {
    auto A= getOutputNFA(driver, driven.numberOfInputs());
    A= makeDeterministic(A);
    A= minimizeDFA(A);
    return buildOFA(A,driven);
}

DFSM SBCMin::makeRandomDFSM(int in_size, int out_size, int size, std::minstd_rand0 &generator) {
    DFSM A(in_size, out_size);
    A.addStates(size);
    std::uniform_int_distribution<int> output_generator(0, A.numberOfOutputs()  - 1);
    std::uniform_int_distribution<int> succ_generator(0, size - 1);
    for (int state = 0; state < size; ++state) {
        for (int i = 0; i < A.numberOfInputs(); ++i) {
            int o = output_generator(generator);
            A.setTrans(state, i, o, succ_generator(generator));
        }
    }
    return A;
}

////







DFSM ModularDFSMBuilder::getDFSM() {
    DFSM A(in_size, out_size);
    A.addStates(size);


    std::mt19937 rnd(time(nullptr));


    class TransitionTags {
        std::vector<bool> impl;
        int in_size;
        int size;

        size_t toID(int state, int in) {
            return state * in_size + in;
        }

    public:
        TransitionTags(int _size, int _in_size) :
                impl(_size * _in_size, false),
                size(_size),
                in_size(_in_size) {}

        void use(int state, int in) {
            impl[toID(state, in)] = true;
        }

        bool hasBeenUsed(int state, int in) {
            return impl[toID(state, in)];
        }

        void reset() {
            std::fill(impl.begin(), impl.end(), false);
        }
    } tags(size, in_size);

    std::vector<std::vector<int>> partition(number_of_bags);

    /// Populate partition

    {
        for (int j = 0; j < partition.size(); ++j) {
            partition[j].push_back(j);
        }

        std::uniform_int_distribution<int> partition_generator(0, number_of_bags - 1);

        for (int j = partition.size(); j < size; ++j) {
            int bag = partition_generator(rnd);
            partition[bag].push_back(j);
        }
    }

    std::vector<std::vector<int>> successors(number_of_bags);

    /// Generate tree of bags using Prufer code

    {
        bool good_tree = false;
        while (!good_tree) {
            good_tree = true;

            std::vector<int> code(number_of_bags - 2);
            std::vector<int> degrees(number_of_bags, 1);

            std::uniform_int_distribution<int> code_generator(0, number_of_bags - 1);
            for (int &i: code) {
                i = code_generator(rnd);
                ++degrees[i];
            }

            /// Populate successors


            {
                auto magicPairing = [](int bag1, int bag2) -> size_t {
                    if (bag1 >= bag2) {
                        return (bag1 * (bag1 + 1)) / 2 + bag2;
                    }
                    return (bag2 * (bag2 + 1)) / 2 + bag1;
                };


                std::vector<bool> connections((number_of_bags * (number_of_bags + 1) / 2), false);

                auto setConnection = [&connections, &magicPairing](int bag1, int bag2) -> void {
                    connections[magicPairing(bag1, bag2)] = true;
                };

                auto deleteConnection = [&connections, &magicPairing](int bag1, int bag2) -> void {
                    connections[magicPairing(bag1, bag2)] = false;
                };

                auto areConnected = [&connections, &magicPairing](int bag1, int bag2) -> bool {
                    return connections[magicPairing(bag1, bag2)];
                };

                for (int bag1: code) {
                    for (int bag2 = 0; bag2 < number_of_bags; ++bag2) {
                        if (degrees[bag2] == 1) {
                            setConnection(bag1, bag2);
                            --degrees[bag2];
                            --degrees[bag1];
                            break;
                        }
                    }
                }

                std::vector<int> unexplored;
                unexplored.reserve(number_of_bags);
                unexplored.push_back(0);

                while (!unexplored.empty()) {
                    int bag1 = unexplored.back();
                    unexplored.pop_back();
                    for (int bag2 = 0; bag2 < number_of_bags; ++bag2) {
                        if (areConnected(bag1, bag2)) {
                            successors[bag1].push_back(bag2);
                            deleteConnection(bag1, bag2);
                            unexplored.push_back(bag2);
                        }
                    }
                }
            }

            /// Check is there are enough transitions in each bag for the tree to happen

            for (int bag = 0; bag < number_of_bags; ++bag) {
                if (partition[bag].size() * in_size < successors[bag].size()) {
                    good_tree = false;
                    break;
                }
            }

            if (!good_tree) continue;

        }


    }


    for (int i = 0; i < number_of_bags; ++i) {

        int current_padding;
        {
            auto dist = std::uniform_int_distribution(0, padding - 1);
            current_padding = dist(rnd);
            int shift = std::log2(out_size / padding);

            current_padding = current_padding << shift;
        }
        auto out_dist = std::uniform_int_distribution(0, out_size / padding - 1);

        ///Set transitions realizing the tree of bags;

        std::vector<int> &bag = partition[i];


        std::vector<int> transitions(bag.size() * in_size);
        std::iota(transitions.begin(), transitions.end(), 0);
        std::shuffle(transitions.begin(), transitions.end(), rnd);
        std::vector<int> &children = successors[i];
        for (int j = 0; j < children.size(); ++j) {
            int current_transition = transitions[j];
            int state = bag[current_transition / in_size];
            int in = current_transition % in_size;
            tags.use(state, in);
            auto &next_bag = partition[children[j]];
            int next_state;
            {
                auto next_dist = std::uniform_int_distribution(0, int(next_bag.size()) - 1);
                next_state = next_dist(rnd);
            }
            A.setTrans(state, in, current_padding + out_dist(rnd), next_state);
        }


        /// Populate the rest of the DFSM
        std::vector<int> external_states;
        external_states.reserve(size);
        {
            for (auto &another_bag: partition) {
                if (&another_bag == &bag) continue;
                external_states.insert(external_states.end(), another_bag.begin(), another_bag.end());
            }
        }

        for (int state: bag) {

            std::vector<int> shuffled_bag = bag;
            std::shuffle(shuffled_bag.begin(), shuffled_bag.end(), rnd);

            auto external_edge_dist = std::uniform_real_distribution(0.0, 1.0);

            int degree = std::min(3, int(bag.size()));

            for (int in = 0; in < in_size; ++in) {
                if (tags.hasBeenUsed(state, in)) continue;
                tags.use(state, in);

                int next;
                if (external_edge_dist(rnd) < extra_edge_probability) {
                    auto next_dist = std::uniform_int_distribution(0, int(external_states.size()) - 1);
                    next = external_states[next_dist(rnd)];
                } else {
                    auto next_dist = std::uniform_int_distribution(0, degree - 1);
                    next = shuffled_bag[next_dist(rnd)];
                }
                A.setTrans(state, in, current_padding + out_dist(rnd), next);
            }
        }

    }


    return A;

}


