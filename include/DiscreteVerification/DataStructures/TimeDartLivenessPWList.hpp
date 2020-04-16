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
#include <utility>
#include <utility>
#include "google/sparse_hash_map"
#include "NonStrictMarkingBase.hpp"
#include "WaitingList.hpp"
#include "TimeDart.hpp"
#include "MarkingEncoder.h"

namespace VerifyTAPN::DiscreteVerification {
        class TimeDartLivenessPWBase;

        class TimeDartLivenessPWHashMap;

        class TimeDartLivenessPWBase {
        public:
            typedef std::vector<LivenessDart *> TimeDartList;
            typedef google::sparse_hash_map<size_t, TimeDartList> HashMap;

        public:

            TimeDartLivenessPWBase() : discoveredMarkings(0), maxNumTokensInAnyMarking(-1), stored(0) {
            };

            virtual ~TimeDartLivenessPWBase() = default;;


        public: // inspectors
            virtual bool hasWaitingStates() = 0;

            virtual long long size() const {
                return stored;
            };

        public: // modifiers
            virtual std::pair<LivenessDart *, bool>
            add(NonStrictMarkingBase *base, int youngest, WaitingDart *parent, int upper, int start) = 0;

            virtual WaitingDart *getNextUnexplored() = 0;

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

            TimeDartLivenessPWHashMap() : markings_storage(), waiting_list() {};

            TimeDartLivenessPWHashMap(VerificationOptions options, WaitingList<WaitingDart *> *w_l)
                    : TimeDartLivenessPWBase(), options(std::move(std::move(options))), markings_storage(256000), waiting_list(w_l) {
            };

            ~TimeDartLivenessPWHashMap() override = default;;

            friend std::ostream &operator<<(std::ostream &out, TimeDartLivenessPWHashMap &x);

            std::pair<LivenessDart *, bool>
            add(NonStrictMarkingBase *base, int youngest, WaitingDart *parent, int upper, int start) override;

            WaitingDart *getNextUnexplored() override;

            void popWaiting() override;

            bool hasWaitingStates() override {
                return (waiting_list->size() > 0);
            };

            void flushBuffer() override;

        private:
            VerificationOptions options;
            HashMap markings_storage;
            WaitingList<WaitingDart *> *waiting_list;
        };

        class TimeDartLivenessPWPData : public TimeDartLivenessPWBase {
        public:
            typedef std::pair<WaitingDart *, ptriepointer_t<LivenessDart *> > waitingpair_t;


            TimeDartLivenessPWPData(const VerificationOptions& options, WaitingList <waitingpair_t> *w_l,
                                    TAPN::TimedArcPetriNet &tapn, int nplaces, int mage) :
                    TimeDartLivenessPWBase(),
                    options(options),
                    waiting_list(w_l),
                    passed(),
                    encoder(tapn, options.getKBound()) {
            };

            ~TimeDartLivenessPWPData() override = default;;

            friend std::ostream &operator<<(std::ostream &out, TimeDartLivenessPWHashMap &x);

            std::pair<LivenessDart *, bool>
            add(NonStrictMarkingBase *base, int youngest, WaitingDart *parent, int upper, int start) override;

            WaitingDart *getNextUnexplored() override;

            void popWaiting() override;

            bool hasWaitingStates() override {
                return (waiting_list->size() > 0);
            };

            void flushBuffer() override;

            NonStrictMarkingBase *decode(ptriepointer_t<LivenessDart *> &ewp) {
                return encoder.decode(ewp);
            }

        private:
            VerificationOptions options;
            WaitingList <waitingpair_t> *waiting_list;
            ptrie_t<LivenessDart *> passed;
            MarkingEncoder<LivenessDart *> encoder;
        };

    } /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
