/* 
 * File:   ReducingGameGenerator.h
 * Author: Peter G. Jensen
 *
 * Created on 14 November 2020, 15.40
 */

#ifndef REDUCINGGAMEGENERATOR_H
#define REDUCINGGAMEGENERATOR_H

#include "Generator.h"
#include "GameStubbornSet.h"
#include "GameGenerator.h"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        class ReducingGameGenerator : protected GameGenerator {
        protected:
            GameStubbornSet _stubborn;
        public:

            ReducingGameGenerator(TAPN::TimedArcPetriNet &tapn, AST::Query *query)
            : GameGenerator(tapn, query), _stubborn(tapn, query) {
            };
            
            void prepare(NonStrictMarkingBase *parent);
            NonStrictMarkingBase* next(bool controllable);
            void reset();
        };
    }
}
#endif /* REDUCINGGAMEGENERATOR_H */

