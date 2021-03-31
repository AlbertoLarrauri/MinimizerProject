//
// Created by lazhares on 16/03/2021.
//

#ifndef PMIN_MONOTONE_RANGE_SEARCHERS_H
#define PMIN_MONOTONE_RANGE_SEARCHERS_H

#include <optional>
#include <functional>

namespace SBCMin {
    class MRS {
    public:

        virtual std::optional<int> operator()(int lower, int upper, std::function<bool(int)> query) = 0;
    };


    class BasicIncrementMRS : public MRS {
    public:
        virtual std::optional<int> operator()(int lower, int upper, std::function<bool(int)> query) {

            if (upper < lower) return std::nullopt;

            int current = lower;

            while (true) {
                if (query(current)) {
                    return current;
                }

                if (current == upper)  return std::nullopt;

                ++current;
            }


        }
    };


    class BasicDecrementMRS : public MRS {
    public:

        virtual std::optional<int> operator()(int lower, int upper, std::function<bool(int)> query) {
            if (upper < lower) return std::nullopt;

            std::optional<int> result = std::nullopt;

            int current=upper;
            while (true){
                if(!query(current)) return result;

                result=current;
                if(current==lower) return result;

                --current;
            }
        }

    };

    class BasicBinaryMRS : public MRS {
    public:
        virtual std::optional<int> operator()(int lower, int upper, std::function<bool(int)> query) {
            if (upper < lower) return std::nullopt;

            std::optional<int> result = std::nullopt;



            if(query(lower)) return lower;
            if(!query(upper)) return std::nullopt;

            int current_lower=lower+1;
            int current_upper=upper;



            while (current_lower!=current_upper){
                int current=(current_upper+current_lower)/2;
                if(query(current)){
                    current_upper=current;
                } else{
                    current_lower=current+1;
                }
            }
            return current_upper;
        }

    };


    class LazyBinaryMRS : public MRS {
    public:
        virtual std::optional<int> operator()(int lower, int upper, std::function<bool(int)> query) {
            if (upper < lower) return std::nullopt;

            std::optional<int> result = std::nullopt;
            bool upper_confirmed=false;

            int current_lower=lower;
            int current_upper=upper;



            while (current_lower!=current_upper){
                int current=(current_upper+current_lower)/2;
                if(query(current)){
                    current_upper=current;
                    upper_confirmed=true;
                } else{
                    current_lower=current+1;
                }
            }
            if(!upper_confirmed){
                if(!query(current_upper)) return std::nullopt;
            }
            return current_upper;
        }

    };

}
#endif //PMIN_MONOTONE_RANGE_SEARCHERS_H
