/*
 * PWList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef PWLIST_HPP_
#define PWLIST_HPP_

#include "DiscreteVerification/DataStructures/WaitingList.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarking.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarking.hpp"
#include "DiscreteVerification/DataStructures/WaitingList.hpp"
#include "DiscreteVerification/DataStructures/MarkingEncoder.h"

#include <google/sparse_hash_map>
#include <iostream>

using namespace ptrie;
namespace VerifyTAPN { namespace DiscreteVerification {

    class PWListBase {
    public:
        PWListBase() : stored(0), discoveredMarkings(0), maxNumTokensInAnyMarking(-1), isLiveness(false) {};

        explicit PWListBase(bool isLiveness) : stored(0), discoveredMarkings(0), maxNumTokensInAnyMarking(-1),
                                               isLiveness(isLiveness) {};
        int stored;
        int discoveredMarkings;
        int maxNumTokensInAnyMarking;
        bool isLiveness;

        virtual bool hasWaitingStates() = 0;

        virtual long long size() const = 0;

        virtual bool add(NonStrictMarking *marking) = 0;

        virtual NonStrictMarking *lookup(NonStrictMarking *marking) { return nullptr; }

        virtual NonStrictMarking *getNextUnexplored() = 0;

        virtual long long explored() = 0;

        virtual void deleteWaitingList() {};

        virtual ~PWListBase() = default;

        inline void setMaxNumTokensIfGreater(int i) {
            if (i > maxNumTokensInAnyMarking)
                maxNumTokensInAnyMarking = i;
        };
    };

    class PWList : public virtual PWListBase {
    public:
        typedef std::vector<NonStrictMarking *> NonStrictMarkingList;
        typedef google::sparse_hash_map<size_t, NonStrictMarkingList> HashMap;
    public:
        PWList() : PWListBase(false), markings_storage(256000), waiting_list() {};

        PWList(WaitingList<NonStrictMarking *> *w_l, bool isLiveness) : PWListBase(isLiveness),
                                                                        markings_storage(256000),
                                                                        waiting_list(w_l) {};

        ~PWList() override;

        friend std::ostream &operator<<(std::ostream &out, PWList &x);

    public: // inspectors
        bool hasWaitingStates() override {
            return (waiting_list->size() > 0);
        };

        long long size() const override {
            return stored;
        };

        long long explored() override { return waiting_list->size(); };

    public: // modifiers
        bool add(NonStrictMarking *marking) override;

        NonStrictMarking *getNextUnexplored() override;

        void deleteWaitingList() override { delete waiting_list; };

    protected:
        HashMap markings_storage;
        WaitingList<NonStrictMarking *> *waiting_list;
    };

    class PWListHybrid : public virtual PWListBase {
    public:
        typedef unsigned int uint;

    public:

        PWListHybrid(TAPN::TimedArcPetriNet &tapn,
                     WaitingList<ptriepointer_t<MetaData *> > *w_l,
                     int knumber,
                     int nplaces,
                     int mage,
                     bool isLiveness,
                     bool makeTrace) :
                PWListBase(isLiveness),
                waiting_list(w_l),
                makeTrace(makeTrace),
                passed(),
                encoder(tapn, knumber) {
            discoveredMarkings = 0;
            parent = nullptr;
        };

        ~PWListHybrid() override;

    public: // inspectors
        NonStrictMarking *decode(ptriepointer_t<MetaData *> &ep) {

            NonStrictMarkingBase *base = encoder.decode(ep);
            auto *m = new NonStrictMarking(*base);
            delete base;
            return m;
        };

        bool hasWaitingStates() override {
            return (waiting_list->size() > 0);
        };

        long long size() const override {
            return passed.size();
        };

        long long explored() override { return waiting_list->size(); };

        void printMemStats() {
//                passed->printMemStats();
        }

        void deleteWaitingList() override { delete waiting_list; };

    public: // modifiers
        bool add(NonStrictMarking *marking) override;

        NonStrictMarking *getNextUnexplored() override;

    protected:

        WaitingList<ptriepointer_t<MetaData *> > *waiting_list;
        bool makeTrace;
    public:
        MetaDataWithTraceAndEncoding *parent;
        ptrie_t<MetaData *> passed;
        MarkingEncoder<MetaData *, NonStrictMarking> encoder;
    };

    std::ostream &operator<<(std::ostream &out, PWList &x);

} } /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
