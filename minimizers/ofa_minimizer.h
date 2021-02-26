//
// Created by llarrauriborroto on 23/02/2021.
//

#ifndef PMIN_OFA_MINIMIZER_H
#define PMIN_OFA_MINIMIZER_H


#include "../machines/machines.h"
#include "cryptominisat5/cryptominisat.h"
#include "compat_matrix.h"


namespace SBCMin {

    class OFACNFBuilder;

    class CompatMatrix;

    class OFAMinimizer {

    public:
        enum CNFBuilders {
            BASIC_INCREMENTAL,
            BASIC_NON_INCREMENTAL,
        };


    private:

        OFA &ofa;
        const DFSM *result;
        std::unique_ptr<CompatMatrix> compat_matrix_ptr;
        std::unique_ptr<OFACNFBuilder> cnf_builder_ptr;

        bool clique_needed = true;
        CNFBuilders builder_type =BASIC_INCREMENTAL;


    public:


        inline OFAMinimizer(OFA &_ofa) : ofa(_ofa) {};

        inline void withClique(bool flag) {
            clique_needed = flag;
        }

        inline void setCNFBuilder(CNFBuilders type){
            builder_type=type;
        }


        void run();

        inline const DFSM &getResult(){
            try {
                if (result==nullptr) throw(0);
                return *result;
            } catch(int){
                std::cout<<"Result not obtained yet. NULLPTR returned \n";
            }
        }

    };


    class OFACNFBuilder {
    private:

        virtual bool trySolve() = 0;

        virtual void init() = 0;

        virtual void step() = 0;

        virtual void computeSolution() = 0;

    public:

        virtual const DFSM &getResult() = 0;

        void run();

    };

}


#endif //PMIN_OFA_MINIMIZER_H
