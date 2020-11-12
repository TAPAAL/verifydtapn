/* 
 * File:   Generator.h
 * Author: Peter G. Jensen
 *
 * Created on 15 September 2015, 17:25
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include "Core/TAPN/TAPN.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarkingBase.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarking.hpp"

#include <vector>
#include <algorithm>

namespace VerifyTAPN { namespace DiscreteVerification {
    class Generator {
        typedef std::vector<const TAPN::TimedTransition *> transitions_t;
    public:
        enum Mode {
            CONTROLLABLE, ENVIRONMENT, ALL
        };
    protected:
        const TAPN::TimedArcPetriNet &tapn;
        NonStrictMarkingBase *parent{};
        Mode mode;
        transitions_t allways_enabled;
        std::vector<transitions_t> place_transition;
        size_t num_children{};
        size_t place{};
        size_t transition{};
        const TAPN::TimedTransition *current{};
        std::vector<size_t> permutation;
        std::vector<size_t> base_permutation;
        bool done{};
        bool seen_urgent{};
        bool did_noinput{};
        const TAPN::TimedTransition *_trans = nullptr;
        AST::Query *query;
        std::vector<uint32_t> transitionStatistics;
        const TAPN::TimedTransition *_last_fired = nullptr;
    public:
        Generator(TAPN::TimedArcPetriNet &tapn, AST::Query *query);

        virtual void from_marking(NonStrictMarkingBase *parent, Mode mode = ALL, bool urgent = false);

        virtual NonStrictMarkingBase *next(bool do_delay = true);

        bool only_transition(const TAPN::TimedTransition *trans);

        NonStrictMarkingBase *from_delay();

        size_t children();

        bool is_enabled(const TAPN::TimedTransition *trans, std::vector<size_t> *permutations = nullptr);

        const TAPN::TimedPlace *
        compute_missing(const TAPN::TimedTransition *trans, std::vector<size_t> *permutations);

        const TAPN::InhibitorArc *inhibited(const TAPN::TimedTransition *trans) const;

        bool urgent() { return seen_urgent; };

        void printTransitionStatistics(std::ostream &out) const;

        const TAPN::TimedTransition *last_fired() const { return _last_fired; }

    protected:
        NonStrictMarkingBase *next_no_input();

        NonStrictMarkingBase *next_from_current();

        bool next_transition(bool isfirst);

        static bool modes_match(const TAPN::TimedTransition *trans, Mode m);
    };
} }
#endif    /* GENERATOR_H */

