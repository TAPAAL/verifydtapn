/*
 * PWList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef TimeDartLivenessPWList_HPP_
#define TimeDartLivenessPWList_HPP_

#include "WaitingList.hpp"
#include <iostream>
#include "google/sparse_hash_map"
#include "NonStrictMarkingBase.hpp"
#include "WaitingList.hpp"
#include "TimeDart.hpp"
#include "PData.h"

namespace VerifyTAPN {
    namespace DiscreteVerification {
        class TimeDartLivenessPWBase;
        class TimeDartLivenessPWHashMap;

        class TimeDartLivenessPWBase {
        public:
            typedef std::vector<LivenessDart*> TimeDartList;
            typedef google::sparse_hash_map<size_t, TimeDartList> HashMap;
        
        public:

            TimeDartLivenessPWBase() : discoveredMarkings(0), maxNumTokensInAnyMarking(-1), stored(0) {
            };

            virtual ~TimeDartLivenessPWBase() {
            };


        public: // inspectors
            virtual bool hasWaitingStates() = 0;

            virtual long long size() const {
                return stored;
            };

        public: // modifiers
            virtual std::pair<LivenessDart*, bool> add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* base, int youngest, WaitingDart* parent, int upper, int start) = 0;
            virtual WaitingDart* getNextUnexplored() = 0;
            virtual void popWaiting() = 0;
            virtual void flushBuffer() = 0;

            inline void setMaxNumTokensIfGreater(int i) {
                if (i > maxNumTokensInAnyMarking) maxNumTokensInAnyMarking = i;
            };

        public:
            int discoveredMarkings;
            int maxNumTokensInAnyMarking;
            long long stored;
        };

        class TimeDartLivenessPWHashMap : public TimeDartLivenessPWBase {
        public:

            TimeDartLivenessPWHashMap() : markings_storage(), waiting_list(){};
            
            TimeDartLivenessPWHashMap(VerificationOptions options, WaitingList<WaitingDart>* w_l) : TimeDartLivenessPWBase(), options(options), markings_storage(256000), waiting_list(w_l) {
            };

            ~TimeDartLivenessPWHashMap() {
            };
            friend std::ostream& operator<<(std::ostream& out, TimeDartLivenessPWHashMap& x);
            virtual std::pair<LivenessDart*, bool> add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* base, int youngest, WaitingDart* parent, int upper, int start);
            virtual WaitingDart* getNextUnexplored();
            virtual void popWaiting();

            virtual bool hasWaitingStates() {
                return (waiting_list->Size() > 0);
            };
            virtual void flushBuffer();
        private:
            VerificationOptions options;
            HashMap markings_storage;
            WaitingList<WaitingDart>* waiting_list;
        };

        class TimeDartLivenessPWPData : public TimeDartLivenessPWBase {
        public:


            
            TimeDartLivenessPWPData(VerificationOptions options, WaitingList<EncodingPointer<WaitingDart> >* w_l, boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, int nplaces, int mage) : TimeDartLivenessPWBase(), options(options), waiting_list(w_l), passed(tapn,  options.GetKBound(), nplaces, mage) {
            };

            ~TimeDartLivenessPWPData() {
            };
            friend std::ostream& operator<<(std::ostream& out, TimeDartLivenessPWHashMap& x);
            virtual std::pair<LivenessDart*, bool> add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* base, int youngest, WaitingDart* parent, int upper, int start);
            virtual WaitingDart* getNextUnexplored();
            virtual void popWaiting();

            virtual bool hasWaitingStates() {
                return (waiting_list->Size() > 0);
            };
            virtual void flushBuffer();
            NonStrictMarkingBase* decode(EncodingPointer<LivenessDart> *ewp){
                return passed.enumerateDecode(*ewp);
            }
            
        private:
            VerificationOptions options;
            WaitingList<EncodingPointer<WaitingDart> >* waiting_list;
            PData<LivenessDart> passed;
        };

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
