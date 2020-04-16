/* 
 * File:   SafetySynthesis.h
 * Author: Peter G. Jensen
 *
 * Created on 14 September 2015, 15:59
 */

#ifndef SAFETYSYNTHESIS_H
#define    SAFETYSYNTHESIS_H

#include <forward_list>

#include "../DataStructures/MarkingStore.h"
#include "../../Core/TAPN/TAPN.hpp"
#include "../QueryVisitor.hpp"
#include "../DataStructures/NonStrictMarkingBase.hpp"
#include "../Generator.h"
#include "../DataStructures/Waiting.h"


namespace VerifyTAPN {
    namespace DiscreteVerification {


        class SafetySynthesis {
        private:
            struct SafetyMeta;
            typedef MarkingStore<SafetyMeta> store_t;
            typedef weightedqueue_t<store_t::Pointer *> waiting_t;
            typedef std::stack<store_t::Pointer *> backstack_t;

            typedef std::pair<bool, store_t::Pointer *> depender_t;
            typedef std::forward_list<depender_t> depends_t;

            enum MarkingState {
                UNKNOWN,            // no successors generated yet
                PROCESSED,          // Generated successors
                MAYBE_WINNING,      // If no env strategy, then winning
                LOOSING,            // env wins
                WINNING
            };           // ctrl surely wins

            struct SafetyMeta {
                MarkingState state;
                bool urgent;
                bool waiting;                       // We only need stuff on waiting once
                size_t ctrl_children;                // Usefull.
                size_t env_children;
                depends_t dependers;                // A punch of parents
            };


            store_t *store;
            waiting_t *waiting;
            TAPN::TimedArcPetriNet &tapn;
            NonStrictMarking &initial_marking;
            AST::Query *query;
            VerificationOptions options;
            std::vector<int> placeStats;
            Generator generator;
            size_t discovered;
            size_t explored;
            unsigned int largest;
        public:
            SafetySynthesis(
                    TAPN::TimedArcPetriNet &tapn,
                    NonStrictMarking &initialMarking,
                    AST::Query *query,
                    VerificationOptions &options);

            bool run();

            virtual ~SafetySynthesis();

            unsigned int max_tokens() { return largest; };

            void print_stats();

        private:
            bool satisfies_query(NonStrictMarkingBase *m);

            void successors(MarkingStore<SafetyMeta>::Pointer *, NonStrictMarkingBase *, SafetyMeta &,
                            waiting_t &waiting, bool controller);

            void dependers_to_waiting(SafetyMeta &next_meta, backstack_t &waiting);
        };
    }
}

#endif    /* SAFETYSYNTHESIS_H */

