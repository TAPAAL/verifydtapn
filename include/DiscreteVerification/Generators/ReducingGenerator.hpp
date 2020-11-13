/* 
 * File:   ReducingGenerator.hpp
 * Author: Peter G. Jensen
 *
 * Created on December 14, 2017, 8:44 PM
 */

#ifndef REDUCINGGENERATOR_HPP
#define REDUCINGGENERATOR_HPP

#include "Generator.h"
#include "StubbornSet.h"
#include "DiscreteVerification/DataStructures/light_deque.h"
#include "InterestingVisitor.h"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        class ReducingGenerator : public Generator {
        private:
            StubbornSet _stubborn;
        public:            
            ReducingGenerator(TAPN::TimedArcPetriNet &tapn, AST::Query *query)
            : Generator(tapn, query), _stubborn(tapn, query) {
            };

            void prepare(NonStrictMarkingBase *parent) override;

            NonStrictMarkingBase *next(bool do_delay = true) override;
        };

    }
}

#endif /* REDUCINGGENERATOR_HPP */

