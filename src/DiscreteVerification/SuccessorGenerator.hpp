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
#include "boost/tuple/tuple_io.hpp"
#include "boost/ptr_container/ptr_vector.hpp"
#include "VerificationTypes/Verification.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace std;
        using namespace TAPN;
        using namespace boost;

        struct ArcRef {
            TokenList enabledBy;

            virtual ~ArcRef() {
            };
        };

        struct InputArcRef : ArcRef {

            InputArcRef(boost::weak_ptr<TimedInputArc> arc) : arc(arc) {
            }
            boost::weak_ptr<TimedInputArc> arc;
        };

        struct InhibitorArcRef : ArcRef {

            InhibitorArcRef(boost::weak_ptr<InhibitorArc> arc) : arc(arc) {
            }
            boost::weak_ptr<InhibitorArc> arc;
        };

        struct TransportArcRef : ArcRef {

            TransportArcRef(boost::weak_ptr<TransportArc> arc) : arc(arc) {
            }
            boost::weak_ptr<TransportArc> arc;
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
            boost::weak_ptr<TimedInputArc> arc;

            InputArcAndTokens(boost::weak_ptr<TimedInputArc> arc, TokenList enabledBy, vector<unsigned int > modificationVector)
            : ArcAndTokens<T>(enabledBy, modificationVector), arc(arc) {
            }

            InputArcAndTokens(boost::weak_ptr<TimedInputArc> arc, TokenList enabledBy)
            : ArcAndTokens<T>(enabledBy, arc.lock()->GetWeight()), arc(arc) {
            }

            void moveToken(Token& token, T& m) {
                m.RemoveToken(arc.lock()->InputPlace().GetIndex(), token.getAge());
            }
        };

        template<typename T>
        class TransportArcAndTokens : public ArcAndTokens<T> {
        public:
            boost::weak_ptr<TransportArc> arc;

            TransportArcAndTokens(boost::weak_ptr<TransportArc> arc, TokenList enabledBy, vector<unsigned int > modificationVector)
            : ArcAndTokens<T>(enabledBy, modificationVector), arc(arc) {
            }

            TransportArcAndTokens(boost::weak_ptr<TransportArc> arc, TokenList enabledBy)
            : ArcAndTokens<T>(enabledBy, arc.lock()->GetWeight()), arc(arc) {
            }

            void moveToken(Token& token, T& m) {
                m.RemoveToken(arc.lock()->Source().GetIndex(), token.getAge());
                m.AddTokenInPlace(arc.lock()->Destination(), token);
            }
        };

        template<typename T>
        class SuccessorGenerator {
            typedef google::sparse_hash_map<const void*, TokenList> ArcHashMap;
            typedef ArcAndTokens<T> ArcAndTokenWithType;
            typedef typename boost::ptr_vector< ArcAndTokenWithType > ArcAndTokensVector;

        public:
            SuccessorGenerator(TAPN::TimedArcPetriNet& tapn, Verification<T>& verifier);
            ~SuccessorGenerator();
            bool generateSuccessors(const T& marking) const;
            void PrintTransitionStatistics(std::ostream & out) const;
            
        private:
            TokenList getPlaceFromMarking(const T& marking, int placeID) const;

            bool generateMarkings(const T& init_marking, const std::vector< const TimedTransition* >& transitions, ArcHashMap& enabledArcs) const;
            bool recursiveGenerateMarking(T& init_marking, const TimedTransition& transition, ArcHashMap& enabledArcs, unsigned int index) const;

            bool addMarking(T& init_marking, const TimedTransition& transition, ArcAndTokensVector& indicesOfCurrentPermutation) const;
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
                    ) const;

            inline void ClearTransitionsArray() {
                memset(transitionStatistics, 0, numberoftransitions * sizeof (transitionStatistics[0]));
            }

            unsigned int numberoftransitions;
            unsigned int* transitionStatistics;
            Verification<T>& verifier;
        };

        template<typename T>
        SuccessorGenerator<T>::~SuccessorGenerator() {

        }

        template<typename T>
        SuccessorGenerator<T>::SuccessorGenerator(TAPN::TimedArcPetriNet& tapn, Verification<T>& verifier) : tapn(tapn), allwaysEnabled(), numberoftransitions(tapn.GetTransitions().size()), transitionStatistics(), verifier(verifier) {
            //Find the transitions which don't have input arcs
            transitionStatistics = new unsigned int [numberoftransitions];
            ClearTransitionsArray();
            for (TimedTransition::Vector::const_iterator iter = tapn.GetTransitions().begin(); iter != tapn.GetTransitions().end(); iter++) {
                if ((*iter)->GetPreset().size() + (*iter)->GetTransportArcs().size() == 0) {
                    allwaysEnabled.push_back(iter->get());
                }
            }
        }

        template<typename T>
        bool SuccessorGenerator<T>::generateSuccessors(const T& marking) const {
            ArcHashMap enabledArcs(tapn.GetInhibitorArcs().size() + tapn.GetInputArcs().size() + tapn.GetTransportArcs().size());
            std::vector<unsigned int> enabledTransitionArcs(tapn.GetTransitions().size(), 0);
            std::vector<const TAPN::TimedTransition* > enabledTransitions;

            for (PlaceList::const_iterator iter = marking.places.begin(); iter < marking.places.end(); iter++) {
                for (TAPN::TimedInputArc::WeakPtrVector::const_iterator arc_iter = iter->place->GetInputArcs().begin();
                        arc_iter != iter->place->GetInputArcs().end(); arc_iter++) {
                    processArc(enabledArcs, enabledTransitionArcs, enabledTransitions,
                            *iter, arc_iter->lock()->Interval(), arc_iter->lock().get(), arc_iter->lock()->OutputTransition());
                }

                for (TAPN::TransportArc::WeakPtrVector::const_iterator arc_iter = iter->place->GetTransportArcs().begin();
                        arc_iter != iter->place->GetTransportArcs().end(); arc_iter++) {
                    processArc(enabledArcs, enabledTransitionArcs, enabledTransitions,
                            *iter, arc_iter->lock()->Interval(), arc_iter->lock().get(),
                            arc_iter->lock()->Transition(), arc_iter->lock()->Destination().GetInvariant().GetBound());
                }

                for (TAPN::InhibitorArc::WeakPtrVector::const_iterator arc_iter = iter->place->GetInhibitorArcs().begin();
                        arc_iter != iter->place->GetInhibitorArcs().end(); arc_iter++) {
                    TimeInterval t(false, 0, std::numeric_limits<int>().max(), true);
                    processArc(enabledArcs, enabledTransitionArcs, enabledTransitions,
                            *iter, t, arc_iter->lock().get(), arc_iter->lock()->OutputTransition(), std::numeric_limits<int>().max(), true);
                }
            }

            enabledTransitions.insert(enabledTransitions.end(), allwaysEnabled.begin(), allwaysEnabled.end());
            return generateMarkings(marking, enabledTransitions, enabledArcs);
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
                ) const {
            bool arcIsEnabled = false;
            for (TokenList::const_iterator token_iter = place.tokens.begin(); token_iter != place.tokens.end(); token_iter++) {
                if (interval.GetLowerBound() <= token_iter->getAge() && token_iter->getAge() <= interval.GetUpperBound() && token_iter->getAge() <= bound) {
                    enabledArcs[arcAddress].push_back(*token_iter);
                    arcIsEnabled = true;
                }
            }
            if (arcIsEnabled && !isInhib) {
                enabledTransitionArcs[transition.GetIndex()]++;
            }
            if (enabledTransitionArcs[transition.GetIndex()] == transition.GetPreset().size() + transition.GetTransportArcs().size() && !isInhib) {
                enabledTransitions.push_back(&transition);
            }
        }

        template<typename T>
        TokenList SuccessorGenerator<T>::getPlaceFromMarking(const T& marking, int placeID) const {
            for (PlaceList::const_iterator iter = marking.GetPlaceList().begin();
                    iter != marking.GetPlaceList().end();
                    iter++) {
                if (iter->place->GetIndex() == placeID) return iter->tokens;
            }
            return TokenList();
        }

        template<typename T>
        bool SuccessorGenerator<T>::generateMarkings(const T& init_marking,
                const std::vector< const TimedTransition* >& transitions, ArcHashMap& enabledArcs) const {

            //Iterate over transitions
            for (std::vector< const TimedTransition* >::const_iterator iter = transitions.begin(); iter != transitions.end(); iter++) {
                bool inhibited = false;
                //Check that no inhibitors is enabled;

                for (TAPN::InhibitorArc::WeakPtrVector::const_iterator inhib_iter = (*iter)->GetInhibitorArcs().begin(); inhib_iter != (*iter)->GetInhibitorArcs().end(); inhib_iter++) {
                    // Maybe this could be done more efficiently using ArcHashMap? Dunno exactly how it works
                    if (init_marking.NumberOfTokensInPlace(inhib_iter->lock().get()->InputPlace().GetIndex()) >= inhib_iter->lock().get()->GetWeight()) {
                        inhibited = true;
                        break;
                    }
                }
                if (inhibited) continue;
                T m(init_marking);
                m.SetGeneratedBy(*iter);
                //Generate markings for transition
                if(recursiveGenerateMarking(m, *(*iter), enabledArcs, 0)){
                    return true;
                }
            }
            return false;
        }

        template<typename T>
        bool SuccessorGenerator<T>::recursiveGenerateMarking(T& init_marking, const TimedTransition& transition, ArcHashMap& enabledArcs, unsigned int index) const {

            // Initialize vectors
            ArcAndTokensVector indicesOfCurrentPermutation;
            for (TimedInputArc::WeakPtrVector::const_iterator iter = transition.GetPreset().begin(); iter != transition.GetPreset().end(); iter++) {
                InputArcAndTokens<T>* arcAndTokens = new InputArcAndTokens<T > (*iter, enabledArcs[iter->lock().get()]);
                if (arcAndTokens->isOK) {
                    indicesOfCurrentPermutation.push_back(arcAndTokens);
                } else {
                    return false;
                }
            }
            // Transport arcs
            for (TransportArc::WeakPtrVector::const_iterator iter = transition.GetTransportArcs().begin(); iter != transition.GetTransportArcs().end(); iter++) {
                TransportArcAndTokens<T>* arcAndTokens = new TransportArcAndTokens<T > (*iter, enabledArcs[iter->lock().get()]);
                if (arcAndTokens->isOK) {
                    indicesOfCurrentPermutation.push_back(arcAndTokens);
                } else {
                    return false;
                }
            }

            // Write statistics
            transitionStatistics[transition.GetIndex()]++;

            // Generate permutations
            bool changedSomething = true;
            verifier.endOfMaxRun = false;
            while (changedSomething) {
                changedSomething = false;
                if(addMarking(init_marking, transition, indicesOfCurrentPermutation)){
                    return true;
                }

                //Loop through arc indexes from the back
                for (int arcAndTokenIndex = indicesOfCurrentPermutation.size() - 1; arcAndTokenIndex >= 0; arcAndTokenIndex--) {
                    TokenList& enabledTokens = indicesOfCurrentPermutation.at(arcAndTokenIndex).enabledBy;
                    vector<unsigned int >& modificationVector = indicesOfCurrentPermutation.at(arcAndTokenIndex).modificationVector;
                    if (incrementModificationVector(modificationVector, enabledTokens)) {
                        changedSomething = true;
                        break;
                    }
                }
            }
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
        bool SuccessorGenerator<T>::addMarking(T& init_marking, const TimedTransition& transition, ArcAndTokensVector& indicesOfCurrentPermutation) const {
            T* m = new T(init_marking);
            for (typename ArcAndTokensVector::iterator iter = indicesOfCurrentPermutation.begin(); iter != indicesOfCurrentPermutation.end(); iter++) {
                vector<unsigned int>& tokens = iter->modificationVector;

                for (vector< unsigned int >::const_iterator tokenIter = tokens.begin(); tokenIter < tokens.end(); tokenIter++) {
                    Token t((iter->enabledBy)[*tokenIter].getAge(), 1);
                    iter->moveToken(t, *m);
                }
            }

            for (OutputArc::WeakPtrVector::const_iterator postsetIter = transition.GetPostset().begin(); postsetIter != transition.GetPostset().end(); postsetIter++) {
                Token t(0, postsetIter->lock()->GetWeight());
                m->AddTokenInPlace(postsetIter->lock()->OutputPlace(), t);
            }
            return verifier.addToPW(m);
        }

        template<typename T>
        void SuccessorGenerator<T>::PrintTransitionStatistics(std::ostream& out) const {
            out << std::endl << "TRANSITION STATISTICS";
            for (unsigned int i = 0; i < numberoftransitions; i++) {
                if ((i) % 6 == 0) {
                    out << std::endl;
                    out << "<" << tapn.GetTransitions()[i]->GetName() << ":" << transitionStatistics[i] << ">";
                } else {
                    out << " <" << tapn.GetTransitions()[i]->GetName() << ":" << transitionStatistics[i] << ">";
                }
            }
            out << std::endl;
            out << std::endl;
        }
    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* SUCCESSORGENERATOR_HPP_ */
