#include "TimeDartVerification.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace std;

        pair<int, int> TimeDartVerification::calculateStart(const TAPN::TimedTransition& transition, NonStrictMarkingBase* marking) {
            vector<Util::interval > start;
            Util::interval initial(0, INT_MAX);
            start.push_back(initial);

            if (transition.NumberOfInputArcs() + transition.NumberOfTransportArcs() == 0) { //always enabled
                pair<int, int> p(0, maxPossibleDelay(marking));
                return p;
            }

            // Inhibitor arcs
            for (TAPN::InhibitorArc::WeakPtrVector::const_iterator arc = transition.GetInhibitorArcs().begin();
                    arc != transition.GetInhibitorArcs().end();
                    arc++) {
                if (marking->NumberOfTokensInPlace(arc->lock()->InputPlace().GetIndex()) >= arc->lock()->GetWeight()) {
                    pair<int, int> p(-1, -1);
                    return p;
                }
            }


            // Standard arcs
            for (TAPN::TimedInputArc::WeakPtrVector::const_iterator arc = transition.GetPreset().begin(); arc != transition.GetPreset().end(); arc++) {
                vector<Util::interval > intervals;
                int range;
                if (arc->lock()->Interval().GetUpperBound() == INT_MAX) {
                    range = INT_MAX;
                } else {
                    range = arc->lock()->Interval().GetUpperBound() - arc->lock()->Interval().GetLowerBound();
                }
                int weight = arc->lock()->GetWeight();

                TokenList tokens = marking->GetTokenList(arc->lock()->InputPlace().GetIndex());
                if (tokens.size() == 0) {
                    pair<int, int> p(-1, -1);
                    return p;
                }

                unsigned int j = 0;
                int numberOfTokensAvailable = tokens.at(j).getCount();
                for (unsigned int i = 0; i < tokens.size(); i++) {
                    if (numberOfTokensAvailable < weight) {
                        for (j++; j < tokens.size() && numberOfTokensAvailable < weight; j++) {
                            numberOfTokensAvailable += tokens.at(j).getCount();
                        }
                        j--;
                    }
                    if (numberOfTokensAvailable >= weight && tokens.at(j).getAge() - tokens.at(i).getAge() <= range) { //This span is interesting
                        int low = arc->lock()->Interval().GetLowerBound() - tokens.at(i).getAge();
                        int heigh = arc->lock()->Interval().GetUpperBound() - tokens.at(j).getAge();

                        Util::interval element(low < 0 ? 0 : low,
                                arc->lock()->Interval().GetUpperBound() == INT_MAX ? INT_MAX : heigh);
                        Util::set_add(intervals, element);
                    }
                    numberOfTokensAvailable -= tokens.at(i).getCount();
                }

                start = Util::setIntersection(start, intervals);
            }

            // Transport arcs
            for (TAPN::TransportArc::WeakPtrVector::const_iterator arc = transition.GetTransportArcs().begin(); arc != transition.GetTransportArcs().end(); arc++) {
                Util::interval arcGuard(arc->lock()->Interval().GetLowerBound(), arc->lock()->Interval().GetUpperBound());
                Util::interval invGuard(0, arc->lock()->Destination().GetInvariant().GetBound());

                Util::interval arcInterval = boost::numeric::intersect(arcGuard, invGuard);
                vector<Util::interval > intervals;
                int range;
                if (arcInterval.upper() == INT_MAX) {
                    range = INT_MAX;
                } else {
                    range = arcInterval.upper() - arcInterval.lower();
                }
                int weight = arc->lock()->GetWeight();

                TokenList tokens = marking->GetTokenList(arc->lock()->Source().GetIndex());

                if (tokens.size() == 0) {
                    pair<int, int> p(-1, -1);
                    return p;
                }

                unsigned int j = 0;
                int numberOfTokensAvailable = tokens.at(j).getCount();
                for (unsigned int i = 0; i < tokens.size(); i++) {
                    if (numberOfTokensAvailable < weight) {
                        for (j++; j < tokens.size() && numberOfTokensAvailable < weight; j++) {
                            numberOfTokensAvailable += tokens.at(j).getCount();
                        }
                        j--;
                    }
                    if (numberOfTokensAvailable >= weight && tokens.at(j).getAge() - tokens.at(i).getAge() <= range) { //This span is interesting
                        Util::interval element(arcInterval.lower() - tokens.at(i).getAge(),
                                arcInterval.upper() - tokens.at(j).getAge());
                        Util::set_add(intervals, element);
                    }
                    numberOfTokensAvailable -= tokens.at(i).getCount();
                }

                start = Util::setIntersection(start, intervals);
            }

            int invariantPart = maxPossibleDelay(marking);

            vector<Util::interval > invEnd;
            Util::interval initialInv(0, invariantPart);
            invEnd.push_back(initialInv);
            start = Util::setIntersection(start, invEnd);

#if DEBUG
            std::cout << "Intervals in start: " << start.size() << std::endl;
#endif

            if (start.empty()) {
                pair<int, int> p(-1, -1);
                return p;
            } else {
                pair<int, int> p(start.front().lower(), start.back().upper());
                return p;
            }
        }

        int TimeDartVerification::calculateStop(const TAPN::TimedTransition& transition, NonStrictMarkingBase* marking) {
            int MC = -1;

            unsigned int i = 0;
            for (PlaceList::const_iterator iter = marking->GetPlaceList().begin(); iter != marking->GetPlaceList().end(); iter++) {
                if (i < transition.GetPreset().size() && iter->place->GetIndex() == transition.GetPreset().at(i).lock()->InputPlace().GetIndex()) {
                    if (transition.GetPreset().at(i).lock()->GetWeight() < iter->NumberOfTokens()) {
                        MC = max(MC, iter->place->GetMaxConstant() - iter->tokens.front().getAge());
                    }
                    i++;
                    continue;
                }
                MC = max(MC, iter->place->GetMaxConstant() - iter->tokens.front().getAge());
            }

            return MC + 1;
        }

        int TimeDartVerification::maxPossibleDelay(NonStrictMarkingBase* marking) {
            int invariantPart = INT_MAX;

            for (PlaceList::const_iterator iter = marking->GetPlaceList().begin(); iter != marking->GetPlaceList().end(); iter++) {
                if (iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max() && iter->place->GetInvariant().GetBound() - iter->tokens.back().getAge() < invariantPart) {
                    invariantPart = iter->place->GetInvariant().GetBound() - iter->tokens.back().getAge();
                }
            }

            return invariantPart;
        }

        vector<NonStrictMarkingBase*> TimeDartVerification::getPossibleNextMarkings(NonStrictMarkingBase& marking, const TAPN::TimedTransition& transition) {
            return successorGenerator.generateSuccessors(marking, transition);
        }

        void TimeDartVerification::GetTrace() {
            stack<NonStrictMarkingBase*> traceStack;

            TraceDart* trace = (TraceDart*) lastMarking;
            int upper = trace->start;
            NonStrictMarkingBase* last = NULL;

            while (trace != NULL) {
                int lower = trace->start;

                // create "last delay marking"
                NonStrictMarkingBase* base = getBase(trace->dart);
                NonStrictMarkingBase* m = new NonStrictMarkingBase(*base);
                m->SetGeneratedBy(trace->generatedBy);
                m->incrementAge(lower);
                if(upper == INT_MAX){
                    upper = tapn.get()->MaxConstant();
                }
                // create markings between transitions, one for each delay (exluding last one)
                if (upper > lower) {
                    int diff = upper - lower;   // amount to delay
                    while (diff) {
                        NonStrictMarkingBase* mc = new NonStrictMarkingBase(*base);
                        mc->incrementAge(lower + diff);
                        mc->SetGeneratedBy(NULL);       // NULL indicates that it is a delay transition
                        if (last != NULL)
                            last->parent = mc;          // set the parent of the last marking
                        last = mc;
                        mc->cut();
                        traceStack.push(mc);            // add delay marking to the trace
                        diff--;
                    }
                }
                if (last != NULL)
                    last->parent = m;
                m->cut();
                last = m;

                traceStack.push(m);     // add the marking to the trace

                upper = trace->upper;
                trace = (TraceDart*) trace->parent;
            }
            last = new NonStrictMarkingBase(*getBase(lastMarking->dart));
            trace = ((TraceDart*)lastMarking);
            last->incrementAge(trace->start);
 
           if(deadlock){
                NonStrictMarkingBase* base = getBase(trace->dart);
                
                int diff = this->maxPossibleDelay(base) - trace->start;
                while (diff) {  // TODO loop seems to count the wrong way, not effecting anything, but wrong.
                        NonStrictMarkingBase* mc = new NonStrictMarkingBase(*base);
                        mc->incrementAge(trace->start + diff);
                        mc->SetGeneratedBy(NULL);       // NULL indicates that it is a delay transition
                        if (last != NULL)
                            last->parent = mc;          // set the parent of the last marking
                        last = mc;
                        mc->cut();
                        if(diff)
                                traceStack.push(mc);            // add delay marking to the trace
                        mc->SetParent(NULL);
                        diff--;
                    }
            }

            PrintXMLTrace(last, traceStack, query->GetQuantifier());
        }

    }
}
