//
// Created by llarrauriborroto on 23/02/2021.
//

#ifndef PMIN_OFA_MINIMIZER_H
#define PMIN_OFA_MINIMIZER_H


#include "../machines/machines.h"
#include "cryptominisat5/cryptominisat.h"
#include "compat_matrix.h"


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


        inline size_t stateSetToID(int state, int Class) {
            return Class * ofa().getSize() + state;
        }

        static inline size_t elegant_pairing(int x, int y) {
            return (x >= y ? (x * x) + x + y : (y * y) + x);
        }

        inline size_t setSetInToID(int Class1, int Class2, int input) {
            return (ofa().numberOfInputs() * elegant_pairing(Class1, Class2)) + input;
        }


        virtual bool trySolve() final;

        virtual void init() = 0;

        virtual bool step() = 0;

        virtual void computeSolution() final;


    protected:



        std::unique_ptr<CMSat::SATSolver> solver;
        std::vector<CMSat::Lit> assumptions;


        std::unique_ptr<DFSM> result_ptr;

        std::vector<size_t> state_set_vars;

        std::vector<size_t> set_set_vars;

        int upper_bound = 0;
        int lower_bound = 0;
        int current_size = 0;

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


    public:

        inline const DFSM &getResult() {
            return *result_ptr;
        }

        virtual bool run(const OFA &_ofa, int _upper_bound) final;

    };

}


#endif //PMIN_OFA_MINIMIZER_H
