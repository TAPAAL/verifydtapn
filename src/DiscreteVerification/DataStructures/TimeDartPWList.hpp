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

            TimeDartPWBase() : discoveredMarkings(0), maxNumTokensInAnyMarking(-1), stored(0) {
            };

            virtual ~TimeDartPWBase() {
            };

        public:

            virtual bool HasWaitingStates() = 0;

            long long Size() const {
                return stored;
            };

            virtual bool Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* marking) = 0;
            virtual LivenessDart* GetNextUnexplored() = 0;

            inline void SetMaxNumTokensIfGreater(int i) {
                if (i > maxNumTokensInAnyMarking) maxNumTokensInAnyMarking = i;
            };


            int discoveredMarkings;
            int maxNumTokensInAnyMarking;
            long long stored;
        };

        class TimeDartPWHashMap : public TimeDartPWBase {
        public:
            typedef std::vector<LivenessDart*> NonStrictMarkingList;
            typedef google::sparse_hash_map<size_t, NonStrictMarkingList> HashMap;
        public:

            TimeDartPWHashMap() : TimeDartPWBase(), waiting_list(), markings_storage(256000) {
            };

            TimeDartPWHashMap(WaitingList<LivenessDart>* w_l) : TimeDartPWBase(), waiting_list(w_l), markings_storage(256000) {
            };
            virtual ~TimeDartPWHashMap();
            friend std::ostream& operator<<(std::ostream& out, TimeDartPWHashMap& x);
            virtual bool Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* marking);
            virtual LivenessDart* GetNextUnexplored();

            virtual bool HasWaitingStates() {
                return (waiting_list->Size() > 0);
            };
        protected:
            WaitingList<LivenessDart>* waiting_list;
        private:
            HashMap markings_storage;
        };

        std::ostream& operator<<(std::ostream& out, TimeDartPWHashMap& x);

        class TimeDartPWPData : public TimeDartPWBase {
        public:

            TimeDartPWPData(WaitingList<EncodingPointer<LivenessDart> >* w_l, boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, int knumber, int nplaces, int mage) :
            TimeDartPWBase(), waiting_list(w_l), passed(tapn, knumber, nplaces, mage) {
            };
        private:
            WaitingList<EncodingPointer<LivenessDart> >* waiting_list;
            PData<LivenessDart> passed;
            virtual bool Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* marking);
            virtual LivenessDart* GetNextUnexplored();

            virtual bool HasWaitingStates() {
                return (waiting_list->Size() > 0);
            };

        };

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
