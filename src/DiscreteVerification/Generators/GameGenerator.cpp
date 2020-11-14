/* 
 * File:   GameGenerator.cpp
 * Author: Peter G. Jensen
 * 
 * Created on 12 November 2020, 21.39
 */

#include "DiscreteVerification/Generators/GameGenerator.h"


#include "DiscreteVerification/Generators/Generator.h"


namespace VerifyTAPN {
    namespace DiscreteVerification {

        void GameGenerator::prepare(NonStrictMarkingBase *parent) {
            _had_urgent = false;
            Generator::prepare(parent);
        }

        void GameGenerator::reset()
        {
            Generator::reset();
            _seen_urgent = _had_urgent;
        }

        NonStrictMarkingBase* GameGenerator::next(bool controllable) {
            auto n = _next(controllable, [controllable](auto a){
               return a->isControllable() == controllable;
            });
            if(n && last_fired())
                _had_urgent |= last_fired()->isUrgent();
            return n;
        }
    }
}
