/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef WORKFLOWSTRONGSOUNDNESS_HPP_
#define WORKFLOWSTRONGSOUNDNESS_HPP_

#include "../DataStructures/WorkflowPWList.hpp"
#include "Workflow.hpp"
#include <stack>
#include <algorithm>

namespace VerifyTAPN::DiscreteVerification {

    class WorkflowStrongSoundnessReachability : public Workflow {
    public:

        WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking,
                                            AST::Query *query, VerificationOptions options,
                                            WaitingList<NonStrictMarking *> *waiting_list);

        WorkflowStrongSoundnessReachability(TAPN::TimedArcPetriNet &tapn, NonStrictMarking &initialMarking,
                                            AST::Query *query, VerificationOptions options);

        ~WorkflowStrongSoundnessReachability() {
            pwList->deleteWaitingList();
            delete lastMarking;
            delete pwList;
        }

        bool run();

        virtual void getTrace();

        void printExecutionTime(ostream &stream) override {
            stream << "Maximum execution time: " << (maxValue * tapn.getGCD()) << endl;
        }


    protected:
        void findInOut();

        bool handleSuccessor(NonStrictMarking *marking, NonStrictMarking *parent);

        virtual void swapData(NonStrictMarking *marking, NonStrictMarking *old);

        virtual void clearTrace() {};    // cleanup

    protected:
        int maxValue;
        TimedPlace *outPlace;
        int validChildren{};
    };

    class WorkflowStrongSoundnessPTrie : public WorkflowStrongSoundnessReachability {
    public:
        WorkflowStrongSoundnessPTrie(
                TAPN::TimedArcPetriNet &tapn,
                NonStrictMarking &initialMarking,
                AST::Query *query,
                VerificationOptions options,
                WaitingList <ptriepointer_t<MetaData *>> *waiting_list);

        void getTrace() override;

        void deleteMarking(NonStrictMarking *marking) override {
            delete marking;
        }

    protected:
        void swapData(NonStrictMarking *marking, NonStrictMarking *old) override;

        void clearTrace() override;
    };

} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
