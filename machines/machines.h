//
// Created by llarrauriborroto on 13/01/2021.
//

#ifndef PMIN_MACHINES_H
#define PMIN_MACHINES_H

#include<iostream>
#include <vector>
#include <optional>
#include <unordered_map>
#include <cassert>
#include <memory>
#include <boost/container/flat_set.hpp>


namespace SBCMin
{
//static char wrong_size_1[1 + sizeof(std::size_t) - sizeof(std::vector<size_t>::size_type)];
//
//static char wrong_size_2[1 - sizeof(std::size_t) + sizeof(std::vector<size_t>::size_type)];






    class FSM{
    protected:
        int size_impl=0;
        int in_alphabet_size;
        int out_alphabet_size;

        //CONSTRUCTORS

        FSM(int in_size, int out_size): in_alphabet_size(in_size), out_alphabet_size(out_size), size_impl(0){};


        virtual void reset(int in_size=0, int out_size=0){
            in_alphabet_size=in_size;
            out_alphabet_size=out_size;
            size_impl=0;
        }

    public:
        inline int size() const{return size_impl;}
        inline int numberOfInputs() const{return in_alphabet_size;}
        inline int numberOfOutputs() const{return out_alphabet_size;}



    };

    class OFA;
    class DFSM;






    DFSM& cascadeDFSM(const DFSM& A, const DFSM& B);

    class DFSM: public virtual FSM{
    private:

        class DTrans{
        public:
            int out;
            int next;
        };

        // Transition data:

        typedef std::vector<DTrans> IMPL;

        IMPL impl;

        // Helper function to get transition index;

        inline size_t SItoID(const int state, const int in) const{
            return size_t(in_alphabet_size) * size_t(state) + size_t(in);
        }



    public:

        inline  DFSM(int in_size, int out_size): FSM(in_size,out_size){
        }

        DFSM(const DFSM& other)=default;
        DFSM& operator=(const DFSM& other)=default;

        inline DFSM(DFSM&& other) noexcept:
        //Fist copies FSM parameters: dfsm_size, input alphabet dfsm_size, and output alphabet dfsm_size
                FSM(other),
                impl(std::move(other.impl))
        {
            other.reset();
        }

        inline DFSM& operator=(DFSM&& other){
            FSM::operator=(other);
            impl=std::move(other.impl);
            other.reset();
            return *this;
        }

        inline void reset(int in_size=0, int out_size=0) override{
            FSM::reset(in_size,out_size);
            impl.clear();
        }



        inline void addStates(int amount=1){
            size_impl+=amount;
            impl.resize(size_impl * in_alphabet_size);
        }


        inline const int& getOut(int state, int in) const{
            return impl.at(SItoID(state, in)).out;
        }

        inline const int& getSucc(int state, int in)const{
            return impl.at(SItoID(state, in)).next;
        }

        inline void setOut(int state, int in, int  out){
            impl.at(SItoID(state, in)).out=out;
        }

        inline void setSucc(int state, int in, int next){
            impl.at(SItoID(state, in)).next=next;
        }

        inline void setTrans(int state, int in , int out, int next){
            setOut(state,in, out);
            setSucc(state,in ,next);
        }


        void print();

    };

    class OFA: public virtual FSM{

    private:

        class NTrans{
        public:
            int out;
            std::vector<int> successors;

        };


        // Transition data. We keep track of getSources and targets of each transition.

        typedef std::vector<std::unordered_map<size_t, std::vector<int>>> SOURCE_IMPL;
        typedef std::vector<std::optional<NTrans>> IMPL;
        IMPL impl;
        SOURCE_IMPL source_data;

        // Helper functions to get indices in the data structures

        inline size_t SItoID(const int state, const int in) const{
            return size_t(in_alphabet_size) * size_t(state) + size_t(in);
        }



        inline size_t IOtoID(const int i, const int o) const{
            return size_t(in_alphabet_size)*size_t(o)+ size_t(i);
        }


    public:

        inline OFA(int in_size, int out_size): FSM(in_size, out_size){
        }

        size_t numberOfTransitions();

        OFA(const OFA& other)=default;

        OFA& operator=(const OFA& other)=default;

        inline OFA(OFA&& other):
                FSM(other),
                impl(std::move(other.impl)),
                source_data(std::move(other.source_data))
        {
            other.reset();
        }

        inline OFA& operator=(OFA&& other){
            FSM::operator=(other);
            impl=std::move(other.impl);
            source_data=std::move(other.source_data);
            other.reset();
            return *this;
        }



        inline void reset(int in_size=0, int out_size=0){
            FSM::reset(in_size,out_size);
            impl.clear();
            source_data.clear();
        }



        inline bool hasTransition(const int state, const int in) const {
            return impl[SItoID(state, in)].has_value();
        }



        inline const int  getOut(int state, int in) const {
            assert(impl[SItoID(state, in)].has_value());
            return impl[SItoID(state, in)]->out;

        }

        inline void setOut(int state, int in, int out){
            impl[SItoID(state, in)].emplace();
            impl[SItoID(state, in)]->out=out;
        }

        inline void addSucc(int state, int in, int succ){
            assert(impl[SItoID(state, in)]);
            impl[SItoID(state, in)]->successors.push_back(succ);
            source_data[succ][IOtoID(in, getOut(state, in))].emplace_back(state);
        }

        inline const std::vector<int> &
        getSuccs(const int state, const int in) const {
            assert(impl[SItoID(state, in)]);
            return (impl[SItoID(state, in)]->successors);
        }

        inline void addStates(int amount=1){
            size_impl+=amount;
            impl.resize(size_impl * in_alphabet_size);
            source_data.resize(size_impl * in_alphabet_size * out_alphabet_size);
        }

        inline bool hasSources(int state, int in, int out){
            return source_data[state].count(IOtoID(in, out));
        }


        inline const std::vector<int>& getSources(int state, int in, int out){
            assert(hasSources(state, in, out));
            return source_data[state][IOtoID(in, out)];
        }

        inline const std::unordered_map<size_t, std::vector<int>> & sourceData(int state) const {
            return source_data[state];
        }

        inline const auto& getSourceData(int state){
            return source_data[state];
        }



        void print();
    };



    bool areEquivalent(const DFSM& A, const DFSM& B);



    class NFA {
    private:
        typedef boost::container::flat_set<uint32_t> flat_set;
//        typedef std::NumVec<std::optional<NumVec>> T_IMPL;
        typedef std::vector<flat_set> T_IMPL;
        T_IMPL impl;
        T_IMPL info_impl;
        T_IMPL r_impl;
        T_IMPL r_info_impl;

        inline size_t SIToID(const uint32_t state, const uint32_t in) const {
            return size_t(in_alphabet_size) * size_t(state) + size_t(in);
        }

    protected:
        uint32_t size_impl = 0;
        uint32_t in_alphabet_size;

    public:

        inline NFA(uint32_t in_size) : in_alphabet_size(in_size) {};

        inline uint32_t size() const {
            return size_impl;
        }

        inline uint32_t numberOfInputs() const {
            return in_alphabet_size;
        }

        inline void addStates(uint32_t amount = 1) {
            size_impl += amount;

            impl.resize(size_impl * in_alphabet_size);
            r_impl.resize(size_impl * in_alphabet_size);

            info_impl.resize(size_impl);
            r_info_impl.resize(size_impl);
        }


        inline void addSucc(uint32_t state, uint32_t in, uint32_t next) {
            auto &symbols = info_impl.at(state);
            auto &succs = impl.at(SIToID(state, in));
            auto &r_symbols = r_info_impl.at(next);
            auto &r_succs = r_impl.at(SIToID(next, in));

            if(!r_succs.empty() && *r_succs.rbegin()==state) return;

            if (succs.empty()) {
                symbols.insert(in);
            }
            if (r_succs.empty()) {
                r_symbols.insert(in);
            }
            succs.insert(next);
            r_succs.insert(state);
            return;
        }


        inline const flat_set &getSuccs(uint32_t state, uint32_t in) const {
//            assert(!(impl.at(SIToID(state, in)).empty()));
            return impl.at(SIToID(state, in));
        }

        inline const flat_set &getSymbols(uint32_t state) const {
            return info_impl.at(state);
        }


        inline bool hasSymbol(uint32_t state, uint32_t in) const {
            auto &succs = impl.at(SIToID(state, in));
            return !succs.empty();
        }

        inline const flat_set &getRSuccs(uint32_t state, uint32_t in) const {
//            assert(!(r_impl.at(SIToID(state, in)).empty()));
            return r_impl.at(SIToID(state, in));
        }

        inline const flat_set &getRSymbols(uint32_t state) const {
            return r_info_impl.at(state);
        }

        inline bool hasRSymbol(uint32_t state, uint32_t in) const {
            auto &succs = r_impl.at(SIToID(state, in));
            return !succs.empty();
        }

        bool isDeterministic() const;

        void print() const;


//        std::unordered_set<uint32_t> propagate(const std::unordered_set<uint32_t> &state_set, uint32_t in) const;

//        std::unordered_set<uint32_t> back_propagate(const std::unordered_set<uint32_t>& target,
//                                                    const std::unordered_set<uint32_t>& source,
//                                                    uint32_t in) const;




    };





}





#endif //PMIN_MACHINES_H
