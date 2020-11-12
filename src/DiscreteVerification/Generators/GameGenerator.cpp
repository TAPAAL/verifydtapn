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
            _seen_urgent = false;
            _current_mode = ENVIRONMENT;
            _last_trans = 0;
            _done = false;
            Generator::from_marking(parent);
        }

        NonStrictMarkingBase* GameGenerator::next(bool controllable) {
            auto m = controllable ? CONTROLLABLE : ENVIRONMENT;
            if (m != _current_mode) {
                _last_trans = 0;
            }

            // TODO, reuse tricks from generator 
            _current_mode = m;
            if (current) {
                auto res = Generator::next_from_current();
                if (res) return res;
            }
            for (; _last_trans < tapn.getTransitions().size(); ++_last_trans) {
                auto* trans = tapn.getTransitions()[_last_trans];
                if (!modes_match(trans, _current_mode)) continue;
                if (!Generator::only_transition(trans)) continue;
                _seen_urgent |= trans->isUrgent();
                ++_last_trans;
                return next(controllable);
            }

            if (controllable && !_done) {
                _done = true;
                if (!seen_urgent) {
                    return from_delay();
                }
            }
            return nullptr;
        }
    }
}
