////
//// Created by llarrauriborroto on 28/01/2021.
////

#include "driven_fsm_minimizer.h"
#include "OFAMinimizers/assumption_based.h"
#include "../machines/builders.h"

using namespace SBCMin;

void DrivenFSMMinimizer::minimize(const DFSM &_driven, const DFSM &_driver) {
    driven_ptr=&_driven;
    driver_ptr=&_driver;
    if(!ofa_minimizer_ptr){
        default_ofa_minimizer_ptr=std::make_unique<SBCMin::OFAMinimizers::AssumptionBased>();
        ofa_minimizer_ptr=default_ofa_minimizer_ptr.get();
    }
    minimizeImpl();
}

void DrivenFSMMinimizer::minimizeImpl() {
    ofa_ptr=std::make_unique<OFA>(buildOFA(driver(),driven()));
    ofaMinimizer().run(ofa(), driven().size());
    result_ptr=&ofaMinimizer().getResult();
}
