/* 
 * File:   GameStubbornSet.cpp
 * Author: Peter G. Jensen
 * 
 * Created on 14 November 2020, 15.36
 */

#include "DiscreteVerification/Generators/GameStubbornSet.h"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        GameStubbornSet::GameStubbornSet(const TimedArcPetriNet& tapn, AST::Query* query)
        : StubbornSet(tapn, query) {
        };

        void GameStubbornSet::prepare(NonStrictMarkingBase *parent) {
            bool has_env = false;
            bool has_ctrl = false;
            _env_trans.clear();
            _ctrl_trans.clear();

            return _prepare(parent, [&has_env, &has_ctrl](auto a) {
                if (a->isControllable()) has_ctrl = true;
                else has_env = true;
            },
            [this, &has_env, &has_ctrl] {
                if (has_env && has_ctrl) return false; // not both!
                return true;

                if(!has_ctrl) {
                    // check REACH
                }

            });
            // we need to check for safety!
            assert(!has_ctrl || !has_env);
            if (has_ctrl)
                _ctrl_trans = _enabled_set;
            else
                _env_trans = _enabled_set;
        }

        const TimedTransition* GameStubbornSet::pop_next(bool controllable) {
            auto& queue = controllable ? _ctrl_trans : _env_trans;
            if (queue.empty()) return nullptr;
            else {
                auto el = queue.front();
                queue.pop_front();
                return _tapn.getTransitions()[el];
            }
        }
    }
}

