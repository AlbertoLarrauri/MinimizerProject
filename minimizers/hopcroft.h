//
// Created by llarrauriborroto on 23/02/2021.
//

#ifndef PMIN_HOPCROFT_H
#define PMIN_HOPCROFT_H

#include "../machines/machines.h"

namespace SBCMin {

    DFSM hopcroft(const DFSM& dfsm);
    OFA hopcroft(const OFA& ofa);


    class HopcroftPartition {

    private:

        typedef std::vector<std::vector<int>> Partition;
        typedef std::vector<int> Set;

        int active_sets = 0;

        int size;

        std::vector<int> active_states;


        std::vector<int> to_set;

        std::vector<bool> target_flag;
        std::vector<int> active_targets_stack;

        std::vector<int> inactive_targets_stack;

        void init(Partition partition);

        void refine(Partition partition);

        void makeTarget(Set set);


    public:

        HopcroftPartition(DFSM &dfsm);


    }




}

#endif //PMIN_HOPCROFT_H
