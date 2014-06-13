/*
 * DiscreteVerification.cpp
 *
 *  Created on: 23 Feb 2012
 *      Author: jakob
 */

#include "DiscreteVerification.hpp"
#include "DeadlockVisitor.hpp"

namespace VerifyTAPN {

    namespace DiscreteVerification {

        template<typename T> void VerifyAndPrint(TAPN::TimedArcPetriNet& tapn, Verification<T>& verifier, VerificationOptions& options, AST::Query* query);
        
        DiscreteVerification::DiscreteVerification() {
            // TODO Auto-generated constructor stub

        }

        DiscreteVerification::~DiscreteVerification() {
            // TODO Auto-generated destructor stub
        }

        int DiscreteVerification::run(TAPN::TimedArcPetriNet& tapn, std::vector<int> initialPlacement, AST::Query* query, VerificationOptions& options) {
            if (!tapn.isNonStrict()) {
                std::cout << "The supplied net contains strict intervals." << std::endl;
                return -1;
            }

            NonStrictMarking* initialMarking = new NonStrictMarking(tapn, initialPlacement);

            std::cout << "MC: " << tapn.getMaxConstant() << std::endl;
#if DEBUG
            std::cout << "Places: " << std::endl;
            for (TAPN::TimedPlace::Vector::const_iterator iter = tapn.get()->getPlaces().begin(); iter != tapn.get()->getPlaces().end(); iter++) {
                std::cout << "Place " << iter->get()->GetIndex() << " has category " << iter->get()->GetType() << std::endl;
            }
#endif

            if (initialMarking->size() > options.getKBound()) {
                std::cout << "The specified k-bound is less than the number of tokens in the initial markings.";
                return 1;
            }
            
            std::cout << options;

            // Select verification method
            if(options.getWorkflowMode() != VerificationOptions::NOT_WORKFLOW){
            	if (options.getVerificationType() == VerificationOptions::TIMEDART) {
					cout << "Workflow analysis currently only supports discrete exploration (i.e. not TimeDarts)." << endl;
					exit(1);
				}

            	if (options.getMemoryOptimization() != VerificationOptions::NO_MEMORY_OPTIMIZATION) {
					cout << "Workflow analysis currently does not support any memory optimizations (i.e. no PTries)." << endl;
					exit(1);
				}
            	WaitingList<NonStrictMarking>* strategy = getWaitingList<NonStrictMarking > (query, options);
            	if(options.getWorkflowMode() == VerificationOptions::WORKFLOW_SOUNDNESS){
            		WorkflowSoundness* verifier = new WorkflowSoundness(tapn, *initialMarking, query, options, strategy);

					if(verifier->getModelType() == verifier->NOTTAWFN){
						std::cerr << "Model is not a TAWFN!" << std::endl;
						return -1;
					}else if(verifier->getModelType() == verifier->ETAWFN){
						std::cout << "Model is a ETAWFN" << std::endl << std::endl;
					}else if(verifier->getModelType() == verifier->MTAWFN){
						std::cout << "Model is a MTAWFN" << std::endl << std::endl;
					}
					VerifyAndPrint(
                                                        tapn,
							*verifier,
							options,
							query);
					verifier->printExecutionTime(cout);
					verifier->printMessages(cout);
            	}
            	else{
            		// Assumes correct structure of net!
            		WorkflowStrongSoundnessReachability* verifier = new WorkflowStrongSoundnessReachability(tapn, *initialMarking, query, options, strategy);
            		VerifyAndPrint(
                                                        tapn,
							*verifier,
							options,
							query);
					verifier->printExecutionTime(cout);
            	}

            	delete strategy;
                return 0;
            }
            
            
            // asume that we dont reach here except if we are doing normal verification
            AST::BoolResult containsDeadlock;
            DeadlockVisitor deadlockVisitor = DeadlockVisitor();
            deadlockVisitor.visit(*query, containsDeadlock);
            if(containsDeadlock.value && options.getGCDLowerGuardsEnabled()){
                        cout << "Lowering constants by greatest common divisor is unsound for queries containing the deadlock proposition" << endl;
                        exit(1);
            }
            if((query->getQuantifier() == EG || query->getQuantifier() == AF) && options.getGCDLowerGuardsEnabled()){
                        cout << "Lowering constants by greatest common divisor is unsound for EG and AF queries" << endl;
                        exit(1);
            }
            
            
            if (options.getVerificationType() == VerificationOptions::DISCRETE) {

                if (options.getMemoryOptimization() == VerificationOptions::PTRIE) {
                    //TODO fix initialization
                    WaitingList<EncodingPointer<MetaData> >* strategy = getWaitingList<EncodingPointer<MetaData> > (query, options);
                    if (query->getQuantifier() == EG || query->getQuantifier() == AF) {
                        LivenessSearchPTrie verifier = LivenessSearchPTrie(tapn, *initialMarking, query, options, strategy);
                        VerifyAndPrint(
                                tapn,
                                verifier,
                                options,
                                query);
                    } else if (query->getQuantifier() == EF || query->getQuantifier() == AG) {
                        ReachabilitySearchPTrie verifier = ReachabilitySearchPTrie(tapn, *initialMarking, query, options, strategy);
                        VerifyAndPrint(
                                tapn,
                                verifier,
                                options,
                                query);

                    }
                    delete strategy;
                } else {
                    WaitingList<NonStrictMarking>* strategy = getWaitingList<NonStrictMarking > (query, options);
                    if (query->getQuantifier() == EG || query->getQuantifier() == AF) {
                        LivenessSearch verifier = LivenessSearch(tapn, *initialMarking, query, options, strategy);
                        VerifyAndPrint(
                                tapn,
                                verifier,
                                options,
                                query);
                    } else if (query->getQuantifier() == EF || query->getQuantifier() == AG) {
                        ReachabilitySearch verifier = ReachabilitySearch(tapn, *initialMarking, query, options, strategy);
                        VerifyAndPrint(
                                tapn,
                                verifier,
                                options,
                                query);
                    }
                    delete strategy;
                }
            } else if (options.getVerificationType() == VerificationOptions::TIMEDART) {
                if (query->getQuantifier() == EG || query->getQuantifier() == AF) {
                    if (containsDeadlock.value) {
                        std::cout << "The combination of TimeDarts, Deadlock proposition and EG or AF queries is currently not supported" << endl;
                        exit(1);
                    }                
                    if (options.getMemoryOptimization() == VerificationOptions::PTRIE) {
                        WaitingList<EncodingPointer<WaitingDart> >* strategy = getWaitingList<EncodingPointer<WaitingDart> > (query, options);
                        TimeDartLivenessPData verifier = TimeDartLivenessPData(tapn, *initialMarking, query, options, strategy);
                        VerifyAndPrint(
                                tapn,
                                verifier,
                                options,
                                query);
                        delete strategy;
                    } else {
                        WaitingList<WaitingDart>* strategy = getWaitingList<WaitingDart > (query, options);
                        TimeDartLiveness verifier = TimeDartLiveness(tapn, *initialMarking, query, options, strategy);
                        VerifyAndPrint(
                                tapn,
                                verifier,
                                options,
                                query);
                        delete strategy;
                    }
                } else if (query->getQuantifier() == EF || query->getQuantifier() == AG) {

                    if (options.getMemoryOptimization() == VerificationOptions::PTRIE) {
                        WaitingList<TimeDartEncodingPointer>* strategy = getWaitingList<TimeDartEncodingPointer > (query, options);
                        TimeDartReachabilitySearchPData verifier = TimeDartReachabilitySearchPData(tapn, *initialMarking, query, options, strategy);
                        VerifyAndPrint(
                                tapn,
                                verifier,
                                options,
                                query);
                        delete strategy;
                    } else {
                        WaitingList<TimeDartBase>* strategy = getWaitingList<TimeDartBase > (query, options);
                        TimeDartReachabilitySearch verifier = TimeDartReachabilitySearch(tapn, *initialMarking, query, options, strategy);
                        VerifyAndPrint(
                                tapn,
                                verifier,
                                options,
                                query);
                        delete strategy;
                    }
                }
            }

            return 0;
        }

        template<typename T> void VerifyAndPrint(TAPN::TimedArcPetriNet& tapn, Verification<T>& verifier, VerificationOptions& options, AST::Query* query) {
            bool result = (!options.isWorkflow() && (query->getQuantifier() == AG || query->getQuantifier() == AF)) ? !verifier.verify() : verifier.verify();

            if (options.getGCDLowerGuardsEnabled()) {
                std::cout << "Lowering all guards by greatest common divisor: " << tapn.getGCD() << std::endl;
            }
            std::cout << std::endl;
            
            verifier.printStats();
            verifier.printTransitionStatistics();

            std::cout << "Query is " << (result ? "satisfied" : "NOT satisfied") << "." << std::endl;
            std::cout << "Max number of tokens found in any reachable marking: ";
            if (verifier.maxUsedTokens() > options.getKBound())
                std::cout << ">" << options.getKBound() << std::endl;
            else
                std::cout << verifier.maxUsedTokens() << std::endl;
            
            if (options.getTrace() == VerificationOptions::SOME_TRACE) {
                if ((query->getQuantifier() == EF && result) || (query->getQuantifier() == AG && !result) || (query->getQuantifier() == EG && result) || (query->getQuantifier() == AF && !result) || (options.isWorkflow())) {
                    verifier.getTrace();
                } else {
                    std::cout << "A trace could not be generated due to the query result" << std::endl;
                }
            }
        }
    }

}
