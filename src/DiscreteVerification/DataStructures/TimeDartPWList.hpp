/*
 * PWList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef TIMEDARTPWLIST_HPP_
#define TIMEDARTPWLIST_HPP_

#include "WaitingList.hpp"
#include <iostream>
#include "google/sparse_hash_map"
#include "NonStrictMarkingBase.hpp"
#include "WaitingList.hpp"
#include "TimeDart.hpp"
#include "ptrie.h"
#include "MarkingEncoder.h"

using namespace ptrie;

namespace VerifyTAPN {
    namespace DiscreteVerification {
        class TimeDartPWBase;
        class TimeDartPWHashMap;
        class TimeDartPWPData;

        class TimeDartPWBase {
        public:

            TimeDartPWBase(bool trace) : trace(trace), discoveredMarkings(0), maxNumTokensInAnyMarking(-1), stored(0), last(NULL) {
            };

            virtual ~TimeDartPWBase() {
            };

        public:

            virtual bool hasWaitingStates() = 0;

            long long size() const {
                return stored;
            };

            virtual bool add(NonStrictMarkingBase* marking, int youngest, WaitingDart* parent, int upper, int start) = 0;
            virtual TimeDartBase* getNextUnexplored() = 0;

            inline void setMaxNumTokensIfGreater(int i) {
                if (i > maxNumTokensInAnyMarking) maxNumTokensInAnyMarking = i;
            };
            
            TraceDart* getLast() {
                return last;
            };

            bool trace;
            int discoveredMarkings;
            int maxNumTokensInAnyMarking;
            long long stored;
        protected:
            TraceDart* last;
        };

        class TimeDartPWHashMap : public TimeDartPWBase {
        public:
            typedef std::vector<TimeDartBase*> TimeDartList;
            typedef google::sparse_hash_map<size_t, TimeDartList> HashMap;
        public:

            TimeDartPWHashMap() : TimeDartPWBase(false), waiting_list(), markings_storage(256000) {
            };

            TimeDartPWHashMap(WaitingList<TimeDartBase*>* w_l, bool trace) : TimeDartPWBase(trace), waiting_list(w_l), markings_storage(256000) {
            };
            virtual ~TimeDartPWHashMap();
            friend std::ostream& operator<<(std::ostream& out, TimeDartPWHashMap& x);
            virtual bool add(NonStrictMarkingBase* marking,int youngest, WaitingDart* parent, int upper, int start);
            virtual TimeDartBase* getNextUnexplored();

            virtual bool hasWaitingStates() {
                return (waiting_list->size() > 0);
            };
        protected:
            WaitingList<TimeDartBase*>* waiting_list;
        private:
            HashMap markings_storage;
        };

        std::ostream& operator<<(std::ostream& out, TimeDartPWHashMap& x);

        class TimeDartPWPData : public TimeDartPWBase {
        public:

            TimeDartPWPData(WaitingList<ptriepointer_t<TimeDartBase*> >* w_l, TAPN::TimedArcPetriNet& tapn, int knumber, int nplaces, int mage, bool trace) :
            TimeDartPWBase(trace), 
                    waiting_list(w_l), passed(), encoder(tapn, knumber) {
            };
            
            NonStrictMarkingBase* decode(ptriepointer_t<TimeDartBase*>& ewp){
                return encoder.decode(ewp);
            }
        private:
            WaitingList<ptriepointer_t<TimeDartBase*> >* waiting_list;
            ptrie_t<TimeDartBase*> passed;
            MarkingEncoder<TimeDartBase*> encoder;
            virtual bool add(NonStrictMarkingBase* marking, int youngest, WaitingDart* parent, int upper, int start);
            virtual TimeDartBase* getNextUnexplored();

            virtual bool hasWaitingStates() {
                return (waiting_list->size() > 0);
            };

        };

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
