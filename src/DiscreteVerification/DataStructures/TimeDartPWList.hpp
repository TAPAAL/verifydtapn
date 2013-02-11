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
#include "PData.h"

namespace VerifyTAPN {
    namespace DiscreteVerification {
        class TimeDartPWBase;
        class TimeDartPWHashMap;
        class TimeDartPWPData;

        class TimeDartPWBase {
        public:

            TimeDartPWBase(bool trace) : trace(trace), discoveredMarkings(0), maxNumTokensInAnyMarking(-1), stored(0) {
            };

            virtual ~TimeDartPWBase() {
            };

        public:

            virtual bool HasWaitingStates() = 0;

            long long Size() const {
                return stored;
            };

            virtual bool Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* marking, int youngest, WaitingDart* parent, int upper) = 0;
            virtual TimeDartBase* GetNextUnexplored() = 0;

            inline void SetMaxNumTokensIfGreater(int i) {
                if (i > maxNumTokensInAnyMarking) maxNumTokensInAnyMarking = i;
            };

            bool trace;
            int discoveredMarkings;
            int maxNumTokensInAnyMarking;
            long long stored;
        };

        class TimeDartPWHashMap : public TimeDartPWBase {
        public:
            typedef std::vector<TimeDartBase*> NonStrictMarkingList;
            typedef google::sparse_hash_map<size_t, NonStrictMarkingList> HashMap;
        public:

            TimeDartPWHashMap() : TimeDartPWBase(false), waiting_list(), markings_storage(256000) {
            };

            TimeDartPWHashMap(WaitingList<TimeDartBase>* w_l, bool trace) : TimeDartPWBase(trace), waiting_list(w_l), markings_storage(256000) {
            };
            virtual ~TimeDartPWHashMap();
            friend std::ostream& operator<<(std::ostream& out, TimeDartPWHashMap& x);
            virtual bool Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* marking,int youngest, WaitingDart* parent, int upper);
            virtual TimeDartBase* GetNextUnexplored();

            virtual bool HasWaitingStates() {
                return (waiting_list->Size() > 0);
            };
        protected:
            WaitingList<TimeDartBase>* waiting_list;
        private:
            HashMap markings_storage;
        };

        std::ostream& operator<<(std::ostream& out, TimeDartPWHashMap& x);

        class TimeDartPWPData : public TimeDartPWBase {
        public:

            TimeDartPWPData(WaitingList<EncodingPointer<TimeDartBase> >* w_l, boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, int knumber, int nplaces, int mage, bool trace) :
            TimeDartPWBase(trace), waiting_list(w_l), passed(tapn, knumber, nplaces, mage) {
            };
        private:
            WaitingList<EncodingPointer<TimeDartBase> >* waiting_list;
            PData<TimeDartBase> passed;
            virtual bool Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* marking, int youngest, WaitingDart* parent, int upper);
            virtual TimeDartBase* GetNextUnexplored();

            virtual bool HasWaitingStates() {
                return (waiting_list->Size() > 0);
            };

        };

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
