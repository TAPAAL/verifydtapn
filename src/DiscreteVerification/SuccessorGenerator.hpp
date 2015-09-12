/*
 * SuccessorGenerator.hpp
 *
 *  Created on: 22/03/2012
 *      Author: MathiasGS
 */

#ifndef SUCCESSORGENERATOR_HPP_
#define SUCCESSORGENERATOR_HPP_

#include "../Core/TAPN/TAPN.hpp"
#include "DataStructures/NonStrictMarking.hpp"
#include "google/sparse_hash_map"
#include <limits>
#include <vector>
#include "VerificationTypes/Verification.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace std;
        using namespace TAPN;

        struct ArcRef {
            TokenList enabledBy;

            virtual ~ArcRef() {
            };
        };

        struct InputArcRef : ArcRef {

            InputArcRef(const TimedInputArc& arc) : arc(arc) {
            }
            const TimedInputArc& arc;
        };

        struct InhibitorArcRef : ArcRef {

            InhibitorArcRef(const InhibitorArc& arc) : arc(arc) {
            }
            const InhibitorArc& arc;
        };

        struct TransportArcRef : ArcRef {

            TransportArcRef(const TransportArc& arc) : arc(arc) {
            }
            const TransportArc& arc;
        };

        template<typename T>
        class ArcAndTokens {
        public:
            TokenList enabledBy;
            vector<unsigned int > modificationVector;
            bool isOK;

            ArcAndTokens(TokenList enabledBy, vector<unsigned int > modificationVector)
            : enabledBy(enabledBy), modificationVector(modificationVector) {
            }

            ArcAndTokens(TokenList enabledBy, int weight)
            : enabledBy(enabledBy), modificationVector(vector<unsigned int>(weight)) {
                isOK = this->reset();
            }

            virtual ~ArcAndTokens() {
            }

            virtual void moveToken(Token& token, T& m) = 0;

            void clearModificationVector() {
                modificationVector.clear();
            }

            bool reset() {
                int weight = modificationVector.size();

                int index = 0;
                // Construct available tokens
                for (vector<Token>::iterator placeTokensIter = enabledBy.begin();
                        placeTokensIter != enabledBy.end() && index < weight;
                        placeTokensIter++) {
                    for (int i = 0; i < placeTokensIter->getCount() && index < weight; i++) {
                        modificationVector[index] = distance(enabledBy.begin(), placeTokensIter);
                        index++;
                    }
                }

                if (index < weight) return false;
                return true;
            }
        };

        template<typename T>
        class InputArcAndTokens : public ArcAndTokens<T> {
        public:
            const TimedInputArc& arc;

            InputArcAndTokens(const TimedInputArc& arc, TokenList enabledBy, vector<unsigned int > modificationVector)
            : ArcAndTokens<T>(enabledBy, modificationVector), arc(arc) {
            }

            InputArcAndTokens(const TimedInputArc& arc, TokenList enabledBy)
            : ArcAndTokens<T>(enabledBy, arc.getWeight()), arc(arc) {
            }

            void moveToken(Token& token, T& m) {
                m.removeToken(arc.getInputPlace().getIndex(), token.getAge());
            }
        };

        template<typename T>
        class TransportArcAndTokens : public ArcAndTokens<T> {
        public:
            const TransportArc& arc;

            TransportArcAndTokens(const TransportArc& arc, TokenList enabledBy, vector<unsigned int > modificationVector)
            : ArcAndTokens<T>(enabledBy, modificationVector), arc(arc) {
            }

            TransportArcAndTokens(const TransportArc& arc, TokenList enabledBy)
            : ArcAndTokens<T>(enabledBy, arc.getWeight()), arc(arc) {
            }

            void moveToken(Token& token, T& m) {
                m.removeToken(arc.getSource().getIndex(), token.getAge());
                m.addTokenInPlace(arc.getDestination(), token);
            }
        };

        enum Result {ADDTOPW_RETURNED_TRUE, ADDTOPW_RETURNED_FALSE, ADDTOPW_RETURNED_FALSE_URGENTENABLED};
        
        template<typename T>
        class SuccessorGenerator {
            typedef google::sparse_hash_map<const void*, TokenList> ArcHashMap;
            typedef ArcAndTokens<T> ArcAndTokenWithType;
            typedef typename std::vector<ArcAndTokenWithType*> ArcAndTokensVector;

        public:
            
            SuccessorGenerator(TAPN::TimedArcPetriNet& tapn, Verification<T>& verifier);
            ~SuccessorGenerator();
            
            Result generateAndInsertSuccessors(const T& marking);
            void printTransitionStatistics(std::ostream & out) const;

            inline bool doSuccessorsExist();
            
        private:

            bool generateMarkings(const T& init_marking, const std::vector< const TimedTransition* >& transitions, ArcHashMap& enabledArcs);
            bool generatePermutations(T& init_marking, const TimedTransition& transition, ArcHashMap& enabledArcs, unsigned int index);

            bool insertMarking(T& init_marking, const TimedTransition& transition, ArcAndTokensVector& indicesOfCurrentPermutation) const;
            bool incrementModificationVector(vector<unsigned int >& modificationVector, TokenList& enabledTokens) const;

            const TAPN::TimedArcPetriNet& tapn;
            vector<const TAPN::TimedTransition*> allwaysEnabled;
            void processArc(
                    ArcHashMap& enabledArcs,
                    std::vector<unsigned int>& enabledTransitionArcs,
                    std::vector<const TAPN::TimedTransition*>& enabledTransitions,
                    const Place& place,
                    const TAPN::TimeInterval& interval,
                    const void* arcAddress,
                    const TimedTransition& transition,
                    int bound = std::numeric_limits<int>().max(),
                    bool isInhib = false
                    );

            inline void clearTransitionsArray() {
                memset(transitionStatistics, 0, numberoftransitions * sizeof (transitionStatistics[0]));
            }

            unsigned int numberoftransitions;
            unsigned int* transitionStatistics;
            Verification<T>& verifier;
            bool succesorsExist;
            bool urgentEnabled;
        };

        template<typename T>
        SuccessorGenerator<T>::~SuccessorGenerator() {
            delete[] transitionStatistics;
        }
        
        template<typename T>
        SuccessorGenerator<T>::SuccessorGenerator(TAPN::TimedArcPetriNet& tapn, Verification<T>& verifier) : tapn(tapn), allwaysEnabled(), numberoftransitions(tapn.getTransitions().size()), transitionStatistics(), verifier(verifier), succesorsExist(true) {
            //Find the transitions which don't have input arcs
            transitionStatistics = new unsigned int [numberoftransitions];
            clearTransitionsArray();
            for (TimedTransition::Vector::const_iterator iter = tapn.getTransitions().begin(); iter != tapn.getTransitions().end(); iter++) {
                if ((*iter)->getPreset().size() + (*iter)->getTransportArcs().size() == 0) {
                    allwaysEnabled.push_back((*iter));
                }
            }
        }

        template<typename T>
        bool SuccessorGenerator<T>::doSuccessorsExist(){
            return this->succesorsExist;
        }
        
        template<typename T>
        Result SuccessorGenerator<T>::generateAndInsertSuccessors(const T& marking) {
            succesorsExist = false;
            urgentEnabled = false;
            ArcHashMap enabledArcs(tapn.getInhibitorArcs().size() + tapn.getInputArcs().size() + tapn.getTransportArcs().size());
            std::vector<unsigned int> enabledTransitionArcs(tapn.getTransitions().size(), 0);

            std::vector<const TAPN::TimedTransition* > enabledTransitions;

            for (PlaceList::const_iterator iter = marking.getPlaceList().begin(); iter < marking.getPlaceList().end(); iter++) {
                for (TAPN::TimedInputArc::Vector::const_iterator arc_iter = iter->place->getInputArcs().begin();
                        arc_iter != iter->place->getInputArcs().end(); arc_iter++) {
                    processArc(enabledArcs, enabledTransitionArcs, enabledTransitions,
                            *iter, (*arc_iter)->getInterval(), (*arc_iter), (*arc_iter)->getOutputTransition());
                }

                for (TAPN::TransportArc::Vector::const_iterator arc_iter = iter->place->getTransportArcs().begin();
                        arc_iter != iter->place->getTransportArcs().end(); arc_iter++) {
                    processArc(enabledArcs, enabledTransitionArcs, enabledTransitions,
                            *iter, (*arc_iter)->getInterval(), (*arc_iter),
                            (*arc_iter)->getTransition(), (*arc_iter)->getDestination().getInvariant().getBound());
                }

                for (TAPN::InhibitorArc::Vector::const_iterator arc_iter = iter->place->getInhibitorArcs().begin();
                        arc_iter != iter->place->getInhibitorArcs().end(); arc_iter++) {
                    TimeInterval t(false, 0, std::numeric_limits<int>().max(), true);
                    processArc(enabledArcs, enabledTransitionArcs, enabledTransitions,
                            *iter, t, (*arc_iter), (*arc_iter)->getOutputTransition(), std::numeric_limits<int>().max(), true);
                }
            }

            enabledTransitions.insert(enabledTransitions.end(), allwaysEnabled.begin(), allwaysEnabled.end());
            if(generateMarkings(marking, enabledTransitions, enabledArcs)){
                return ADDTOPW_RETURNED_TRUE;
            } else {
                if(urgentEnabled){
                    return ADDTOPW_RETURNED_FALSE_URGENTENABLED;
                } else{
                    return ADDTOPW_RETURNED_FALSE;
                }
            }

        }

        template<typename T>
        void SuccessorGenerator<T>::processArc(
                ArcHashMap& enabledArcs,
                std::vector<unsigned int>& enabledTransitionArcs,
                std::vector<const TAPN::TimedTransition* >& enabledTransitions,
                const Place& place,
                const TAPN::TimeInterval& interval,
                const void* arcAddress,
                const TimedTransition& transition,
                int bound,
                bool isInhib
                ) {
            bool arcIsEnabled = false;
            for (TokenList::const_iterator token_iter = place.tokens.begin(); token_iter != place.tokens.end(); token_iter++) {
                if (interval.getLowerBound() <= token_iter->getAge() && token_iter->getAge() <= interval.getUpperBound() && token_iter->getAge() <= bound) {
                    enabledArcs[arcAddress].push_back(*token_iter);
                    arcIsEnabled = true;
                }
            }
            if (arcIsEnabled && !isInhib) {
                enabledTransitionArcs[transition.getIndex()]++;
            }
            if (enabledTransitionArcs[transition.getIndex()] == transition.getPreset().size() + transition.getTransportArcs().size() && !isInhib) {
                enabledTransitions.push_back(&transition);
            }
        }
        
        template<typename T>
        bool SuccessorGenerator<T>::generateMarkings(const T& init_marking,
                const std::vector< const TimedTransition* >& transitions, ArcHashMap& enabledArcs) {

            //Iterate over transitions
            for (std::vector< const TimedTransition* >::const_iterator iter = transitions.begin(); iter != transitions.end(); iter++) {
                bool inhibited = false;
                //Check that no inhibitors is enabled;

                for (TAPN::InhibitorArc::Vector::const_iterator inhib_iter = (*iter)->getInhibitorArcs().begin(); inhib_iter != (*iter)->getInhibitorArcs().end(); inhib_iter++) {
                    // Maybe this could be done more efficiently using ArcHashMap? Dunno exactly how it works
                    if (init_marking.numberOfTokensInPlace((*inhib_iter)->getInputPlace().getIndex()) >= (*inhib_iter)->getWeight()) {
                        inhibited = true;
                        break;
                    }
                }
                if (inhibited) continue;
                T m(init_marking);
                m.setGeneratedBy(*iter);
                //Generate markings for transition
                if(generatePermutations(m, *(*iter), enabledArcs, 0)){
                    return true;
                }
            }
            return false;
        }

        template<typename T>
        bool SuccessorGenerator<T>::generatePermutations(T& init_marking, const TimedTransition& transition, ArcHashMap& enabledArcs, unsigned int index) {

            // Initialize vectors
            ArcAndTokensVector indicesOfCurrentPermutation;
            for (TimedInputArc::Vector::const_iterator iter = transition.getPreset().begin(); iter != transition.getPreset().end(); iter++) {
                InputArcAndTokens<T>* arcAndTokens = new InputArcAndTokens<T > (**iter, enabledArcs[(*iter)]);
                if (arcAndTokens->isOK) {
                    indicesOfCurrentPermutation.push_back(arcAndTokens);
                } else {
                    delete arcAndTokens;
                    for(unsigned int i = 0; i < indicesOfCurrentPermutation.size(); ++i) delete indicesOfCurrentPermutation[i];
                    return false;
                }
            }
            // Transport arcs
            for (TransportArc::Vector::const_iterator iter = transition.getTransportArcs().begin(); iter != transition.getTransportArcs().end(); iter++) {
                TransportArcAndTokens<T>* arcAndTokens = new TransportArcAndTokens<T > (**iter, enabledArcs[(*iter)]);
                if (arcAndTokens->isOK) {
                    indicesOfCurrentPermutation.push_back(arcAndTokens);
                } else {
                    delete arcAndTokens;
                    for(unsigned int i = 0; i < indicesOfCurrentPermutation.size(); ++i) delete indicesOfCurrentPermutation[i];
                    return false;
                }
            }

            // only here the weights have been properly checked.
            if(transition.isUrgent()){
                this->urgentEnabled = true;
            }
            
            // Write statistics
            transitionStatistics[transition.getIndex()]++;

            // Generate permutations
            bool changedSomething = true;
            succesorsExist = true;
            while (changedSomething) {
                changedSomething = false;
                if(insertMarking(init_marking, transition, indicesOfCurrentPermutation)){
                    for(unsigned int i = 0; i < indicesOfCurrentPermutation.size(); ++i) delete indicesOfCurrentPermutation[i];
                    return true;
                }

                //Loop through arc indexes from the back
                for (int arcAndTokenIndex = indicesOfCurrentPermutation.size() - 1; arcAndTokenIndex >= 0; arcAndTokenIndex--) {
                    TokenList& enabledTokens = indicesOfCurrentPermutation.at(arcAndTokenIndex)->enabledBy;
                    vector<unsigned int >& modificationVector = indicesOfCurrentPermutation[arcAndTokenIndex]->modificationVector;
                    if (incrementModificationVector(modificationVector, enabledTokens)) {
                        changedSomething = true;
                        break;
                    }
                }
            }
            for(unsigned int i = 0; i < indicesOfCurrentPermutation.size(); ++i) delete indicesOfCurrentPermutation[i];
            return false;
        }

        template<typename T>
        bool SuccessorGenerator<T>::incrementModificationVector(vector<unsigned int >& modificationVector, TokenList& enabledTokens) const {
            unsigned int numOfTokenIndices = enabledTokens.size();

            unsigned int* refrences = new unsigned int[numOfTokenIndices];
            unsigned int* org_refrences = new unsigned int[numOfTokenIndices];

            for (unsigned int i = 0; i < enabledTokens.size(); i++) {
                refrences[i] = enabledTokens[i].getCount();
            }

            for (unsigned int i = 0; i < modificationVector.size(); i++) {
                refrences[modificationVector[i]]--;
            }

            memcpy(org_refrences, refrences, sizeof (unsigned int) *numOfTokenIndices);

            int modificationVectorSize = modificationVector.size();

            vector<unsigned int> tmp = modificationVector;
            // Loop through modification vector from the back
            for (int i = modificationVectorSize - 1; i >= 0; i--) {

                //Possible to increment index
                if (modificationVector[i] < numOfTokenIndices - 1 && org_refrences[modificationVector.at(i) + 1] > 0) {
                    //Increment index
                    refrences[modificationVector.at(i)]++;
                    modificationVector.at(i)++;
                    refrences[modificationVector.at(i)]--;

                    // Fix following indexes
                    if (i < modificationVectorSize - 1) {
                        unsigned int toSet = modificationVector.at(i);

                        for (i++; i < modificationVectorSize; i++) {
                            //Find next index to set (die if not possible)
                            while (refrences[toSet] == 0) {
                                toSet++;
                                if (toSet >= numOfTokenIndices) {
                                    modificationVector = tmp;
                                    delete [] refrences;
                                    delete [] org_refrences;
                                    return false;
                                }
                            }

                            //Set index
                            refrences[toSet]--;
                            modificationVector[i] = toSet;
                        }
                    }
                    delete [] refrences;
                    delete [] org_refrences;
                    return true;
                } else {
                    // Free index
                    refrences[modificationVector[i]]++;
                }
            }
            modificationVector = tmp;
            delete [] refrences;
            delete [] org_refrences;
            return false;
        }

        template<typename T>
        bool SuccessorGenerator<T>::insertMarking(T& init_marking, const TimedTransition& transition, ArcAndTokensVector& indicesOfCurrentPermutation) const {
            T* m = new T(init_marking);
            for (typename ArcAndTokensVector::iterator iter = indicesOfCurrentPermutation.begin(); iter != indicesOfCurrentPermutation.end(); iter++) {
                vector<unsigned int>& tokens = (*iter)->modificationVector;

                for (vector< unsigned int >::const_iterator tokenIter = tokens.begin(); tokenIter < tokens.end(); tokenIter++) {
                    Token t(((*iter)->enabledBy)[*tokenIter].getAge(), 1);
                    (*iter)->moveToken(t, *m);
                }
            }

            for (OutputArc::Vector::const_iterator postsetIter = transition.getPostset().begin(); postsetIter != transition.getPostset().end(); postsetIter++) {
                Token t(0, (*postsetIter)->getWeight());
                m->addTokenInPlace((*postsetIter)->getOutputPlace(), t);
            }
            return verifier.handleSuccessor(m);
        }

        template<typename T>
        void SuccessorGenerator<T>::printTransitionStatistics(std::ostream& out) const {
            out << std::endl << "TRANSITION STATISTICS";
            for (unsigned int i = 0; i < numberoftransitions; i++) {
                if ((i) % 6 == 0) {
                    out << std::endl;
                    out << "<" << tapn.getTransitions()[i]->getName() << ":" << transitionStatistics[i] << ">";
                } else {
                    out << " <" << tapn.getTransitions()[i]->getName() << ":" << transitionStatistics[i] << ">";
                }
            }
            out << std::endl;
            out << std::endl;
        }
    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* SUCCESSORGENERATOR_HPP_ */
