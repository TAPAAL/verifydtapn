/*
 * DiscreteVerification.cpp
 *
 *  Created on: 23 Feb 2012
 *      Author: jakob
 */

#include "DiscreteVerification.hpp"

namespace VerifyTAPN {

    namespace DiscreteVerification {

        template<typename T> void VerifyAndPrint(Verification<T>* verifier, VerificationOptions& options, AST::Query* query);

        DiscreteVerification::DiscreteVerification() {
            // TODO Auto-generated constructor stub

        }

        DiscreteVerification::~DiscreteVerification() {
            // TODO Auto-generated destructor stub
        }

        int DiscreteVerification::run(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, std::vector<int> initialPlacement, AST::Query* query, VerificationOptions& options) {
            if (!(*tapn).IsNonStrict()) {
                std::cout << "The supplied net contains strict intervals." << std::endl;
                return -1;
            }

            NonStrictMarking* initialMarking = new NonStrictMarking(*tapn, initialPlacement);

            std::cout << "MC: " << tapn->MaxConstant() << std::endl;
#if DEBUG
            std::cout << "Places: " << std::endl;
            for (TAPN::TimedPlace::Vector::const_iterator iter = tapn.get()->GetPlaces().begin(); iter != tapn.get()->GetPlaces().end(); iter++) {
                std::cout << "Place " << iter->get()->GetIndex() << " has category " << iter->get()->GetType() << std::endl;
            }
#endif

            if (initialMarking->size() > options.getKBound()) {
                std::cout << "The specified k-bound is less than the number of tokens in the initial markings.";
                return 1;
            }

            std::cout << options << std::endl;

            WaitingList<NonStrictMarking>* strategy = NULL;
            // Select verification method
            if (options.getVerificationType() == VerificationOptions::DISCRETE) {
                
                if (options.getMemoryOptimization() == VerificationOptions::PTRIE) {
                    //TODO fix initialization
                    WaitingList<EncodingPointer<MetaData> >* strategy = getWaitingList<EncodingPointer<MetaData> > (query, options);
                    if (query->getQuantifier() == EG || query->getQuantifier() == AF) {
                        VerifyAndPrint(
                                new LivenessSearchPTrie(tapn, *initialMarking, query, options, strategy),
                                options,
                                query);
                    } else if (query->getQuantifier() == EF || query->getQuantifier() == AG) {
                        VerifyAndPrint(
                                new ReachabilitySearchPTrie(tapn, *initialMarking, query, options, strategy),
                                options,
                                query);
                    }
                } else {
                    WaitingList<NonStrictMarking>* strategy = getWaitingList<NonStrictMarking > (query, options);
                    if (query->getQuantifier() == EG || query->getQuantifier() == AF) {
                        VerifyAndPrint(
                                new LivenessSearch(tapn, *initialMarking, query, options, strategy),
                                options,
                                query);
                    } else if (query->getQuantifier() == EF || query->getQuantifier() == AG) {
                        VerifyAndPrint(
                                new ReachabilitySearch(tapn, *initialMarking, query, options, strategy),
                                options,
                                query);
                    }
                }
            } else if (options.getVerificationType() == VerificationOptions::TIMEDART) {
                if (query->getQuantifier() == EG || query->getQuantifier() == AF) {
                    if (options.getMemoryOptimization() == VerificationOptions::PTRIE) {
                        WaitingList<EncodingPointer<WaitingDart> >* strategy = getWaitingList<EncodingPointer<WaitingDart> > (query, options);
                        VerifyAndPrint(
                                new TimeDartLivenessPData(tapn, *initialMarking, query, options, strategy),
                                options,
                                query);
                    } else {
                        WaitingList<WaitingDart>* strategy = getWaitingList<WaitingDart > (query, options);
                        VerifyAndPrint(
                                new TimeDartLiveness(tapn, *initialMarking, query, options, strategy),
                                options,
                                query);
                    }
                } else if (query->getQuantifier() == EF || query->getQuantifier() == AG) {

                    if (options.getMemoryOptimization() == VerificationOptions::PTRIE) {
                        WaitingList<TimeDartEncodingPointer>* strategy = getWaitingList<TimeDartEncodingPointer > (query, options);
                        VerifyAndPrint(
                                new TimeDartReachabilitySearchPData(tapn, *initialMarking, query, options, strategy),
                                options,
                                query);
                    } else {
                        WaitingList<TimeDartBase>* strategy = getWaitingList<TimeDartBase > (query, options);
                        VerifyAndPrint(
                                new TimeDartReachabilitySearch(tapn, *initialMarking, query, options, strategy),
                                options,
                                query);
                    }
                }
            }


            delete strategy;

            return 0;
        }

        template<typename T> void VerifyAndPrint(Verification<T>* verifier, VerificationOptions& options, AST::Query* query) {
            bool result = (query->getQuantifier() == AG || query->getQuantifier() == AF) ? !verifier->verify() : verifier->verify();

            verifier->printStats();
            verifier->printTransitionStatistics();

            std::cout << "Query is " << (result ? "satisfied" : "NOT satisfied") << "." << std::endl;
            std::cout << "Max number of tokens found in any reachable marking: ";
            if (verifier->maxUsedTokens() > options.getKBound())
                std::cout << ">" << options.getKBound() << std::endl;
            else
                std::cout << verifier->maxUsedTokens() << std::endl;

            if (options.getTrace() == VerificationOptions::SOME_TRACE) {
                if ((query->getQuantifier() == EF && result) || (query->getQuantifier() == AG && !result) || (query->getQuantifier() == EG && result) || (query->getQuantifier() == AF && !result)) {
                    verifier->getTrace();
                } else {
                    std::cout << "A trace could not be generated due to the query result" << std::endl;
                }
            }
        }
    }

}
