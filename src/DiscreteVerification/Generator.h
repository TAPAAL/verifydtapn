/* 
 * File:   Generator.h
 * Author: Peter G. Jensen
 *
 * Created on 15 September 2015, 17:25
 */

#ifndef GENERATOR_H
#define	GENERATOR_H

#include <vector>
#include <algorithm>
#include "../Core/TAPN/TAPN.hpp"
#include "DataStructures/NonStrictMarkingBase.hpp"
#include "DataStructures/NonStrictMarking.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {
        class Generator {
            typedef std::vector<const TAPN::TimedTransition*> transitions_t;
            public:
                enum Mode {CONTROLLABLE, ENVIRONMENT, ALL};
            private:
                const TAPN::TimedArcPetriNet& tapn; 
                NonStrictMarkingBase* parent;
                Mode mode;  
                transitions_t allways_enabled;
                std::vector<transitions_t> place_transition;
                size_t noinput_nr;
                size_t place;
                size_t transition;
                const TAPN::TimedTransition* current;
                std::vector<size_t> permutation;
                std::vector<size_t> base_permutation;
                bool done;
                bool seen_urgent;
                bool did_noinput;
            public:
                Generator(TAPN::TimedArcPetriNet& tapn);
                void from_marking(NonStrictMarkingBase* parent, Mode mode = ALL);
                NonStrictMarkingBase* next(bool do_delay = false);
                NonStrictMarkingBase* from_delay();
                size_t children();
                bool is_enabled();
                
        private:
            NonStrictMarkingBase* next_no_input();
            NonStrictMarkingBase* next_from_current();
            bool next_transition(bool isfirst);
            bool modes_match(const TAPN::TimedTransition* trans);
        };
       
    }
}



#endif	/* GENERATOR_H */

