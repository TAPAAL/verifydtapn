/* 
 * File:   GameGenerator.h
 * Author: Peter G. Jensen
 *
 * Created on 12 November 2020, 21.39
 */

#ifndef GAMEGENERATOR_H
#define GAMEGENERATOR_H
#include "DiscreteVerification/Generators/Generator.h"
#include "Generator.h"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        class GameGenerator : protected Generator {
        protected:
            bool _seen_urgent = false;
            size_t _last_trans = 0;
            bool _done = false;
            Mode _current_mode;
        public:
            GameGenerator(TAPN::TimedArcPetriNet &tapn, AST::Query *query)
                    : Generator(tapn, query) {};
            
            void prepare(NonStrictMarkingBase *parent);
            
            NonStrictMarkingBase* next(bool controllable);
        };

    }
}

#endif /* GAMEGENERATOR_H */

