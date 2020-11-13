/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ReducingGenerator.cpp
 * Author: Peter G. Jensen
 * 
 * Created on December 14, 2017, 8:44 PM
 */

#include "DiscreteVerification/Generators/ReducingGenerator.hpp"
#include "DiscreteVerification/QueryVisitor.hpp"
#include "DiscreteVerification/Generators/StubbornSet.h"


namespace VerifyTAPN {
    namespace DiscreteVerification {

        void ReducingGenerator::prepare(NonStrictMarkingBase *parent) {
            Generator::prepare(parent);
            _stubborn.prepare(parent);
            _seen_urgent |= _stubborn.urgent();
        }

        NonStrictMarkingBase *ReducingGenerator::next(bool do_delay) {
            if(_stubborn.irreducable())
                return Generator::next(do_delay);
            
            if (_done) return nullptr;
            // we need more permutations
            if (_current)
                return fire(_current);

            // move to next transition
            auto transition = _stubborn.pop_next();
            if(transition != nullptr)
            {
#ifndef DEBUG
                bool en =
#endif
                Generator::only_transition(transition);
                assert(en);
                return fire(transition);
            }
            // done, only delay waiting
            _done = true;
            if (do_delay && !_seen_urgent)
                return from_delay();
            return nullptr;
        }
    }
}
