/*
 * PWList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef WORKFLOWPWLIST_HPP_
#define WORKFLOWPWLIST_HPP_

#include "WaitingList.hpp"
#include <iostream>
#include "google/sparse_hash_map"
#include  "NonStrictMarking.hpp"
#include "NonStrictMarking.hpp"
#include "WaitingList.hpp"
#include "PWList.hpp"
#include "CoveredMarkingVisitor.h"

namespace VerifyTAPN::DiscreteVerification {

    class WorkflowPWListBasic : virtual public PWListBase {
    public:
        virtual NonStrictMarking *getCoveredMarking(NonStrictMarking *marking, bool useLinearSweep) = 0;

        virtual NonStrictMarking *getUnpassed() = 0;

        bool add(NonStrictMarking *marking) override = 0;

        virtual NonStrictMarking *addToPassed(NonStrictMarking *marking, bool strong) = 0;

        virtual void addLastToWaiting() = 0;

        virtual void setParent(NonStrictMarking *, NonStrictMarking *) = 0;
    };

    class WorkflowPWList : public WorkflowPWListBasic, public PWList {
    private:
        NonStrictMarking *last{};
    public:
        explicit WorkflowPWList(WaitingList<NonStrictMarking *> *w_l);

        NonStrictMarking *getCoveredMarking(NonStrictMarking *marking, bool useLinearSweep) override;

        NonStrictMarking *getUnpassed() override;

        bool add(NonStrictMarking *marking) override;

        NonStrictMarking *addToPassed(NonStrictMarking *marking, bool strong) override;

        NonStrictMarking *lookup(NonStrictMarking *marking) override;

        void addLastToWaiting() override {
            waiting_list->add(last, last);
        }

        void setParent(NonStrictMarking *marking, NonStrictMarking *parent) override {
            marking->setParent(parent);
        }

    };

    class WorkflowPWListHybrid : public WorkflowPWListBasic, public PWListHybrid {
    private:
        CoveredMarkingVisitor visitor;
        ptriepointer_t<MetaData *> last_pointer;
    public:
        WorkflowPWListHybrid(TAPN::TimedArcPetriNet &tapn,
                             WaitingList<ptriepointer_t<MetaData *> > *w_l,
                             int knumber,
                             int nplaces,
                             int mage);

        ~WorkflowPWListHybrid() override;

        NonStrictMarking *getCoveredMarking(NonStrictMarking *marking, bool useLinearSweep) override;

        NonStrictMarking *getUnpassed() override;

        bool add(NonStrictMarking *marking) override;

        NonStrictMarking *addToPassed(NonStrictMarking *marking, bool strong) override;

        void addLastToWaiting() override;

        void setParent(NonStrictMarking *marking, NonStrictMarking *) override {
            ((MetaDataWithTraceAndEncoding *) marking->meta)->parent = parent;
        }
    };

} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
