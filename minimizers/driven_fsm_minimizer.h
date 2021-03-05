////
//// Created by llarrauriborroto on 28/01/2021.
////

#ifndef PMIN_DRIVEN_FSM_MINIMIZER_H
#define PMIN_DRIVEN_FSM_MINIMIZER_H

#include "hopcroft.h"
#include <iostream>
#include <memory>
#include "cryptominisat5/cryptominisat.h"
#include "../machines/builders.h"
#include "ofa_minimizer.h"
#include "compat_matrix.h"

namespace SBCMin {

    class DrivenFSMMinimizer {
    private:
        const DFSM* driver_ptr= nullptr;
        const DFSM* driven_ptr= nullptr;
        std::unique_ptr<OFAMinimizer> default_ofa_minimizer_ptr= nullptr;
        OFAMinimizer* ofa_minimizer_ptr= nullptr;


    protected:

        std::unique_ptr<OFA> ofa_ptr;

        const DFSM* result_ptr;

        inline const DFSM& driven(){
            return *driven_ptr;
        }

        inline const DFSM& driver(){
            return *driver_ptr;
        }

        inline OFAMinimizer& ofaMinimizer(){
            return *ofa_minimizer_ptr;
        }

        inline OFA& ofa(){
            return *ofa_ptr;
        }

        inline const DFSM& result(){
            return *result_ptr;
        }


        virtual void minimizeImpl();


    public:

        virtual void minimize(const DFSM& _driven, const DFSM& _driver) final;


    };
}

#endif //PMIN_DRIVEN_FSM_MINIMIZER_H
