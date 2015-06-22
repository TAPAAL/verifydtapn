/* 
 * File:   AbstractReachability.hpp
 * Author: Peter Jensen
 *
 * Created on October 28, 2013, 9:01 PM
 */

#ifndef ABSTRACTREACHABILITY_HPP
#define	ABSTRACTREACHABILITY_HPP

#include "../DataStructures/PWList.hpp"
#include "../../Core/TAPN/TAPN.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include "../../Core/VerificationOptions.hpp"
#include "../../Core/TAPN/TimedPlace.hpp"
#include "../../Core/TAPN/TimedTransition.hpp"
#include "../../Core/TAPN/TimedInputArc.hpp"
#include "../../Core/TAPN/TransportArc.hpp"
#include "../../Core/TAPN/InhibitorArc.hpp"
#include "../../Core/TAPN/OutputArc.hpp"
#include "../SuccessorGenerator.hpp"
#include "../QueryVisitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include "Verification.hpp"
#include "../DataStructures/WaitingList.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {
        using namespace std;
        
        template<typename T, typename U>
        class AbstractNaiveVerification : public Verification<U> {
        public:
            AbstractNaiveVerification(TAPN::TimedArcPetriNet& tapn, U& initialMarking, AST::Query* query, VerificationOptions options, T* pwList);
            ~AbstractNaiveVerification()
            {

            }
            
            void printTransitionStatistics() const {
                successorGenerator.printTransitionStatistics(std::cout);
            }

            void printStats();

        protected:
            bool isDelayPossible(U& marking);
            virtual bool addToPW(U* marking, U* parent) = 0;

            inline bool addToPW(U* m) {
                return addToPW(m, tmpParent);
            };
            
            U* getLastMarking() {
                return lastMarking;
            }

            inline unsigned int maxUsedTokens() {
                return pwList->maxNumTokensInAnyMarking;
            };

        protected:
            SuccessorGenerator<U> successorGenerator;
            U* lastMarking;
            U* tmpParent;
            T* pwList;
        };

        template<typename T,typename U>
        AbstractNaiveVerification<T,U>::AbstractNaiveVerification(TAPN::TimedArcPetriNet& tapn, U& initialMarking, AST::Query* query, VerificationOptions options, T* pwList)
        : Verification<U>(tapn, initialMarking, query, options), lastMarking(NULL), successorGenerator(tapn, *this), pwList(pwList) {

        };

        template<typename T,typename U>
        void AbstractNaiveVerification<T,U>::printStats() {
            std::cout << "  discovered markings:\t" << pwList->discoveredMarkings << std::endl;
            std::cout << "  explored markings:\t" << pwList->size() - pwList->explored() << std::endl;
            std::cout << "  stored markings:\t" << pwList->size() << std::endl;
        };

        template<typename T,typename U>
        bool AbstractNaiveVerification<T,U>::isDelayPossible(U& marking) {
            const PlaceList& places = marking.getPlaceList();
            if (places.size() == 0) return true; //Delay always possible in empty markings

            PlaceList::const_iterator markedPlace_iter = places.begin();
            for (TAPN::TimedPlace::Vector::const_iterator place_iter = this->tapn.getPlaces().begin(); place_iter != this->tapn.getPlaces().end(); place_iter++) {
                int inv = (*place_iter)->getInvariant().getBound();
                if (**place_iter == *(markedPlace_iter->place)) {
                    if (markedPlace_iter->maxTokenAge() > inv - 1) {
                        return false;
                    }

                    markedPlace_iter++;

                    if (markedPlace_iter == places.end()) return true;
                }
            }
            assert(false); // This happens if there are markings on places not in the TAPN
            return false;
        };
    }
}
#endif	/* ABSTRACTREACHABILITY_HPP */

