#ifndef SMCVERIFICATION_HPP
#define SMCVERIFICATION_HPP

#include "DiscreteVerification/VerificationTypes/Verification.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarking.hpp"
#include "DiscreteVerification/Generators/SMCRunGenerator.h"
#include "Core/Query/SMCQuery.hpp"

#include <mutex>

namespace VerifyTAPN::DiscreteVerification {

class SMCVerification : public Verification<RealMarking> {

    public:

        SMCVerification(TAPN::TimedArcPetriNet &tapn, RealMarking &initialMarking, AST::SMCQuery *query,
                        VerificationOptions options) 
            : Verification(tapn, initialMarking, query, options)
            , runGenerator(tapn)
            , numberOfRuns(0), maxTokensSeen(0), smcSettings(query->getSmcSettings())
            { }

        virtual bool run() override;
        virtual bool parallel_run();

        virtual void prepare() { }

        virtual bool executeRun(SMCRunGenerator* generator = nullptr);

        virtual void printStats() override;
        void printTransitionStatistics() const override;

        unsigned int maxUsedTokens() override;
        void setMaxTokensIfGreater(unsigned int i);

        virtual bool reachedRunBound(SMCRunGenerator* generator = nullptr);
        
        virtual void handleRunResult(const bool res, int steps, double delay) = 0;
        virtual bool mustDoAnotherRun() = 0;

        virtual void printResult() = 0;

        inline bool mustSaveTrace() const { return traces.size() < options.getSmcTraces(); }
        void handleTrace(const bool runRes, SMCRunGenerator* generator = nullptr);
        void saveTrace(SMCRunGenerator* generator = nullptr);

        void getTrace() override;

        void printHumanTrace(RealMarking *m, std::stack<RealMarking *> &stack, AST::Quantifier query);

        void printXMLTrace(RealMarking *m, std::stack<RealMarking *> &stack, AST::Query *query, TAPN::TimedArcPetriNet &tapn);

        rapidxml::xml_node<> *createTransitionNode(RealMarking *old, RealMarking *current, rapidxml::xml_document<> &doc);

        void createTransitionSubNodes(RealMarking *old, RealMarking *current, rapidxml::xml_document<> &doc,
                                      rapidxml::xml_node<> *transitionNode, const TAPN::TimedPlace &place,
                                      const TAPN::TimeInterval &interval, int weight);

        rapidxml::xml_node<> *
        createTokenNode(rapidxml::xml_document<> &doc, const TAPN::TimedPlace &place, const RealToken &token);

    protected:

        SMCRunGenerator runGenerator;
        SMCSettings smcSettings;
        size_t numberOfRuns;
        unsigned int maxTokensSeen;
        double totalTime = 0;
        unsigned long totalSteps = 0;
        int64_t durationNs = 0;

        std::mutex run_res_mutex;

        std::vector<std::stack<RealMarking*>> traces;

};

}

#endif  /* SMC_VERIFICATION_HPP */