//
// Created by llarrauriborroto on 23/02/2021.
//

#ifndef PMIN_HOPCROFT_H
#define PMIN_HOPCROFT_H

#include "../machines/machines.h"


namespace SBCMin {


    class DFSMHopcroft {

    private:

        typedef std::vector<std::vector<int>> Partition;

        DFSM& dfsm;

        int dfsm_size=0;

        int number_of_sets = 0;

        std::vector<int> active_states;

        std::vector<int> partition_dict;

        std::vector<bool> is_set_tracked;
        std::vector<int> tracked_sets;

        std::vector<int> tracked_singletons;


        inline int& toSetOrSingleton(int state){
            assert(partition_dict.size()>state);
            return partition_dict[state];
        }

        inline bool isSetTracked(int set){
            assert(is_set_tracked.size()>set);
            return is_set_tracked[set];
        }

        inline bool finished(){
            return (tracked_singletons.empty() && tracked_sets.empty()) || active_states.empty();
        }


        int popTarget();

        void addSet(std::vector<int> set, bool tracking);

        void refine(int target);

        void clear();

        DFSMHopcroft(DFSM &dfsm);

        static DFSM extractDFSM(DFSMHopcroft& algorithm);


    public:

        static DFSM minimize(DFSM& dfsm);




    };



    class OFAHopcroft {

    private:

        typedef std::vector<std::vector<int>> Partition;

        OFA& ofa;

        int ofa_size=0;

        int number_of_sets = 0;

        std::vector<int> active_states;

        std::vector<int> partition_dict;

        std::vector<bool> is_set_tracked;
        std::vector<int> tracked_sets;

        std::vector<int> tracked_singletons;


        inline int& toSetOrSingleton(int state){
            assert(partition_dict.size()>state);
            return partition_dict[state];
        }

        inline bool isSetTracked(int set){
            assert(is_set_tracked.size()>set);
            return is_set_tracked[set];
        }

        inline bool finished(){
            return (tracked_singletons.empty() && tracked_sets.empty()) || active_states.empty();
        }


        int popTarget();

        void addSet(std::vector<int> set, bool tracking);

        void refine(int target);

        void clear();

        OFAHopcroft(OFA &ofa);

        static OFA extractOFA(OFAHopcroft& algorithm);


    public:

        static OFA minimize(OFA& OFA);


    };



    NFA minimizeDFA(const NFA& A);




}

#endif //PMIN_HOPCROFT_H
