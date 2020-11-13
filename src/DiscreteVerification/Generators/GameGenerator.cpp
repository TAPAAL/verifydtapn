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
            _current_mode = ENVIRONMENT;
            _had_urgent = false;
            Generator::prepare(parent);
        }

        void GameGenerator::reset()
        {
            Generator::reset();
        }

        NonStrictMarkingBase* GameGenerator::next(bool controllable) {
            const auto cm = _current_mode;
            auto n = _next(!_had_urgent && controllable, [cm](auto a){
               return modes_match(a, cm);
            });
            if(n && last_fired())
                _had_urgent |= last_fired()->isUrgent();
            return n;
        }

        bool GameGenerator::modes_match(const TAPN::TimedTransition *trans, mode_e m) {
            switch (m) {
                case mode_e::CONTROLLABLE:
                    if (!trans->isControllable()) return false;
                    break;
                case mode_e::ENVIRONMENT:
                    if (trans->isControllable()) return false;
                    break;
                case mode_e::ALL:
                    break;
            }
            return true;
        }
    }
}
