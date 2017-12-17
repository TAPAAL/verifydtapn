/* 
 * File:   AbstractReachability.hpp
 * Author: Peter Jensen
 *
 * Created on October 28, 2013, 9:01 PM
 */

#ifndef ABSTRACTREACHABILITY_HPP
#define	ABSTRACTREACHABILITY_HPP

#include <memory>

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
#include "../Generator.h"
#include "../QueryVisitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include "Verification.hpp"
#include "../DataStructures/WaitingList.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {
        using namespace std;
        enum SRes {
            ADDTOPW_RETURNED_TRUE,
            ADDTOPW_RETURNED_FALSE_URGENTENABLED,
            ADDTOPW_RETURNED_FALSE
        };        
        template<typename T, typename U, typename S>
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
            virtual bool handleSuccessor(U* marking, U* parent) = 0;

            inline bool handleSuccessor(U* m) {
                return handleSuccessor(m, tmpParent);
            };
            
            U* getLastMarking() {
                return lastMarking;
            }

            inline unsigned int maxUsedTokens() {
                return pwList->maxNumTokensInAnyMarking;
            };

        protected:
            SRes generateAndInsertSuccessors(NonStrictMarkingBase& from);
            S successorGenerator;
            U* lastMarking;
            U* tmpParent;
            T* pwList;
        };

        template<typename T,typename U, typename S>
        AbstractNaiveVerification<T,U,S>::AbstractNaiveVerification(TAPN::TimedArcPetriNet& tapn, U& initialMarking, AST::Query* query, VerificationOptions options, T* pwList)
        : Verification<U>(tapn, initialMarking, query, options), successorGenerator(tapn, query), lastMarking(NULL), pwList(pwList) {

        };

        template<typename T,typename U, typename S>
        void AbstractNaiveVerification<T,U,S>::printStats() {
            std::cout << "  discovered markings:\t" << pwList->discoveredMarkings << std::endl;
            std::cout << "  explored markings:\t" << pwList->size() - pwList->explored() << std::endl;
            std::cout << "  stored markings:\t" << pwList->size() << std::endl;
        };

        template<typename T,typename U, typename S>
        bool AbstractNaiveVerification<T,U,S>::isDelayPossible(U& marking) {
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
        
        template<typename T,typename U, typename S>
        SRes AbstractNaiveVerification<T,U,S>::generateAndInsertSuccessors(NonStrictMarkingBase& from) {
            

            successorGenerator.from_marking(&from);
            while(auto next = std::unique_ptr<NonStrictMarkingBase>(successorGenerator.next(false)))
            {                
                U* ptr = new U(*next);
                if(handleSuccessor(ptr))
                {
                    return ADDTOPW_RETURNED_TRUE;
                }
            }
            
            return successorGenerator.urgent() ? SRes::ADDTOPW_RETURNED_FALSE_URGENTENABLED : SRes::ADDTOPW_RETURNED_FALSE;
        }
        
    }
}
#endif	/* ABSTRACTREACHABILITY_HPP */

