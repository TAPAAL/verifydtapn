/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef WORKFLOWSOUNDNESS_HPP_
#define WORKFLOWSOUNDNESS_HPP_

#include "DiscreteVerification/DataStructures/WorkflowPWList.hpp"
#include "Core/TAPN/TAPN.hpp"
#include "Core/QueryParser/AST.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/TAPN/TimedPlace.hpp"
#include "Core/TAPN/TimedTransition.hpp"
#include "Core/TAPN/TimedInputArc.hpp"
#include "Core/TAPN/TransportArc.hpp"
#include "Core/TAPN/InhibitorArc.hpp"
#include "Core/TAPN/OutputArc.hpp"
#include "DiscreteVerification/QueryVisitor.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarking.hpp"
#include "DiscreteVerification/DataStructures/WaitingList.hpp"
#include "Workflow.hpp"

#include <stack>

using namespace ptrie;
namespace VerifyTAPN { namespace DiscreteVerification {

    class WorkflowSoundness : public Workflow {
    public:
        enum ModelType {
            MTAWFN, ETAWFN, NOTTAWFN
        };

        WorkflowSoundness(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::Query *query,
                          VerificationOptions options, WaitingList<NonStrictMarking *> *waiting_list);

        WorkflowSoundness(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::Query *query,
                          VerificationOptions options);

        virtual ~WorkflowSoundness();

        bool run();

        virtual void getTrace(NonStrictMarking *marking);

        virtual void getTrace() { this->getTrace(lastMarking); };

        void printExecutionTime(std::ostream &stream) override {
            stream << "Minimum execution time: " << minExec << std::endl;
        }

        void printMessages(std::ostream &stream) {
            if (coveredMarking != nullptr) {
                stream << "Covered marking: " << *coveredMarking << std::endl;
                getTrace(coveredMarking);
            }
        }

        inline ModelType getModelType() const { return modelType; }

        virtual int numberOfPassed();

    protected:
        bool handleSuccessor(NonStrictMarking *marking, NonStrictMarking *parent);

        bool checkForCoveredMarking(NonStrictMarking *marking);

        ModelType calculateModelType();

        virtual void addParentMeta(MetaData *meta, MetaData *parent);

        virtual void setMetaParent(NonStrictMarking *) {};

    protected:
        std::stack<MetaData *> passedStack{};
        int minExec;
        unsigned int linearSweepTreshold;
        NonStrictMarking *coveredMarking;
        ModelType modelType;

    };


    class WorkflowSoundnessPTrie : public WorkflowSoundness {
    public:
        WorkflowSoundnessPTrie(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking, AST::Query *query,
                               const VerificationOptions &options,
                               WaitingList<ptriepointer_t<MetaData *> > *waiting_list);

        void addParentMeta(MetaData *meta, MetaData *parent) override;

        int numberOfPassed() override;

        void deleteMarking(NonStrictMarking *marking) override {
            delete marking;
        }

        void getTrace(NonStrictMarking *marking) override;

    protected:
        void setMetaParent(NonStrictMarking *) override;
    };

} }/* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
