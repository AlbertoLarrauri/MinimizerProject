//
// Created by llarrauriborroto on 23/02/2021.
//

#ifndef PMIN_OFA_MINIMIZER_H
#define PMIN_OFA_MINIMIZER_H


#include "../machines/machines.h"
#include "cryptominisat5/cryptominisat.h"
#include "compat_matrix.h"
#include <string>
#include <sstream>
#include <chrono>
#include <utility>
#include "monotone_range_searchers.h"

namespace SBCMin {


    namespace OFAMinimizers {
        enum Minimizers {
            BASIC_INCREMENTAL,
            BASIC_NON_INCREMENTAL,
        };
    }


    class OFAMinimizer {
    private:

        const OFA *ofa_ptr;
        std::unique_ptr<const CompatMatrix> compat_matrix_ptr;

        inline virtual void buildMatrix() {
            compat_matrix_ptr = std::make_unique<CompatMatrix>(ofa());

        }


        static inline size_t elegant_pairing(int x, int y) {
            return (x >= y ? (x * x) + x + y : (y * y) + x);
        }



        virtual void init()=0;

        virtual bool runImpl() = 0;


    protected:


        std::unique_ptr<CMSat::SATSolver> solver;

        std::unique_ptr<DFSM> result_ptr;

        std::vector<size_t> state_set_vars;

        std::vector<size_t> set_set_vars;

        int upper_bound = 0;
        int lower_bound = 0;
        int current_size = 0;


        inline size_t stateSetToID(int state, int Class) {
            return Class * ofa().getSize() + state;
        }


        inline size_t setSetInToID(int Class1, int Class2, int input) {
            return (ofa().numberOfInputs() * elegant_pairing(Class1, Class2)) + input;
        }

        inline const OFA &ofa() {
            return *ofa_ptr;
        }

        inline const CompatMatrix &compat_matrix() {
            return *compat_matrix_ptr;
        }


        virtual inline void resize(int number_of_sets) {
            state_set_vars.resize(ofa().getSize() * number_of_sets);
            set_set_vars.resize(number_of_sets * number_of_sets * ofa().numberOfInputs());
        }


        inline size_t &stateSetVar(int state, int set) {
            return state_set_vars[stateSetToID(state, set)];
        }

        inline size_t &setSetVar(int Class1, int Class2, int input) {
            return set_set_vars[setSetInToID(Class1, Class2, input)];
        }

        virtual bool trySolve(const std::vector<CMSat::Lit> &assumptions={}) final;

        virtual void computeSolution() final;



    public:

        inline const DFSM &getResult() {
            return *result_ptr;
        }

        virtual bool run(const OFA &_ofa, int _upper_bound) final {
            ofa_ptr = &_ofa;
            buildMatrix();
            upper_bound = _upper_bound;
            lower_bound = compat_matrix().getClique().size();
            init();
            return runImpl();
        }

    };


    class OFAMinimizerWCustomStrategy: public OFAMinimizer{
    private:

        typedef
        std::function<std::optional<int>(int lower, int upper, std::function<bool(int)> query)> Strategy;

       Strategy strat=BasicIncrementMRS();


        bool runImpl() final;


    protected:
        virtual bool query(int size)=0;


    public:

        void setStrategy(Strategy _strat){
            strat=std::move(_strat);
        }



    };

}


#endif //PMIN_OFA_MINIMIZER_H
