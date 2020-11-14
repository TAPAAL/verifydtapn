/* 
 * File:   GameStubbornSet.h
 * Author: Peter G. Jensen
 *
 * Created on 14 November 2020, 15.36
 */

#ifndef GAMESTUBBORNSET_H
#define GAMESTUBBORNSET_H

#include "StubbornSet.h"


namespace VerifyTAPN {
    namespace DiscreteVerification {

        class GameStubbornSet : protected StubbornSet {
        public:
            bool urgent() const { return StubbornSet::urgent(); }
            bool irreducable() const { return StubbornSet::irreducable(); }

            GameStubbornSet(const TimedArcPetriNet& tapn, AST::Query* query);
            void prepare(NonStrictMarkingBase *parent);
            const TimedTransition* pop_next(bool controllable);
            
        private:
            light_deque<uint32_t> _ctrl_trans;
            light_deque<uint32_t> _env_trans;
        };
    }
}

#endif /* GAMESTUBBORNSET_H */

