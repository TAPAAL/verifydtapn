#include "TimeDartVerification.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace std;

        pair<int, int> TimeDartVerification::calculateStart(const TAPN::TimedTransition& transition, NonStrictMarkingBase* marking) {
            vector<Util::interval > start;
            Util::interval initial(0, INT_MAX);
            start.push_back(initial);

            if (transition.getNumberOfInputArcs() + transition.getNumberOfTransportArcs() == 0) { //always enabled
                pair<int, int> p(0, maxPossibleDelay(marking));
                return p;
            }

            // Inhibitor arcs
            for (TAPN::InhibitorArc::WeakPtrVector::const_iterator arc = transition.getInhibitorArcs().begin();
                    arc != transition.getInhibitorArcs().end();
                    arc++) {
                if (marking->numberOfTokensInPlace(arc->lock()->getInputPlace().getIndex()) >= arc->lock()->getWeight()) {
                    pair<int, int> p(-1, -1);
                    return p;
                }
            }


            // Standard arcs
            for (TAPN::TimedInputArc::WeakPtrVector::const_iterator arc = transition.getPreset().begin(); arc != transition.getPreset().end(); arc++) {
                vector<Util::interval > intervals;
                int range;
                if (arc->lock()->getInterval().getUpperBound() == INT_MAX) {
                    range = INT_MAX;
                } else {
                    range = arc->lock()->getInterval().getUpperBound() - arc->lock()->getInterval().getLowerBound();
                }
                int weight = arc->lock()->getWeight();

                TokenList tokens = marking->getTokenList(arc->lock()->getInputPlace().getIndex());
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
                        int low = arc->lock()->getInterval().getLowerBound() - tokens.at(i).getAge();
                        int heigh = arc->lock()->getInterval().getUpperBound() - tokens.at(j).getAge();

                        Util::interval element(low < 0 ? 0 : low,
                                arc->lock()->getInterval().getUpperBound() == INT_MAX ? INT_MAX : heigh);
                        Util::setAdd(intervals, element);
                    }
                    numberOfTokensAvailable -= tokens.at(i).getCount();
                }

                start = Util::setIntersection(start, intervals);
            }

            // Transport arcs
            for (TAPN::TransportArc::WeakPtrVector::const_iterator arc = transition.getTransportArcs().begin(); arc != transition.getTransportArcs().end(); arc++) {
                Util::interval arcGuard(arc->lock()->getInterval().getLowerBound(), arc->lock()->getInterval().getUpperBound());
                Util::interval invGuard(0, arc->lock()->getDestination().getInvariant().getBound());

                Util::interval arcInterval = boost::numeric::intersect(arcGuard, invGuard);
                vector<Util::interval > intervals;
                int range;
                if (arcInterval.upper() == INT_MAX) {
                    range = INT_MAX;
                } else {
                    range = arcInterval.upper() - arcInterval.lower();
                }
                int weight = arc->lock()->getWeight();

                TokenList tokens = marking->getTokenList(arc->lock()->getSource().getIndex());

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
                        Util::setAdd(intervals, element);
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
            for (PlaceList::const_iterator iter = marking->getPlaceList().begin(); iter != marking->getPlaceList().end(); iter++) {
                if (i < transition.getPreset().size() && iter->place->getIndex() == transition.getPreset().at(i).lock()->getInputPlace().getIndex()) {
                    if (transition.getPreset().at(i).lock()->getWeight() < iter->numberOfTokens()) {
                        MC = max(MC, iter->place->getMaxConstant() - iter->tokens.front().getAge());
                    }
                    i++;
                    continue;
                }
                MC = max(MC, iter->place->getMaxConstant() - iter->tokens.front().getAge());
            }

            return MC + 1;
        }

        int TimeDartVerification::maxPossibleDelay(NonStrictMarkingBase* marking) {
            int invariantPart = INT_MAX;

            for (PlaceList::const_iterator iter = marking->getPlaceList().begin(); iter != marking->getPlaceList().end(); iter++) {
                if (iter->place->getInvariant().getBound() != std::numeric_limits<int>::max() && iter->place->getInvariant().getBound() - iter->tokens.back().getAge() < invariantPart) {
                    invariantPart = iter->place->getInvariant().getBound() - iter->tokens.back().getAge();
                }
            }

            return invariantPart;
        }

        void TimeDartVerification::getTrace() {
            
            stack<NonStrictMarkingBase*> traceStack;

            TraceDart* trace = (TraceDart*) lastMarking;
            int upper = trace->start;
            NonStrictMarkingBase* last = NULL;
            NonStrictMarkingBase* l = NULL;
            if(deadlock){
                NonStrictMarkingBase* base = getBase(trace->dart);
                
                int diff = this->maxPossibleDelay(base) - trace->start;
                while (diff) {  // TODO loop seems to count the wrong way, not effecting anything, but wrong.
                        NonStrictMarkingBase* mc = new NonStrictMarkingBase(*base);
                        mc->incrementAge(trace->start + diff);
                        mc->setGeneratedBy(NULL);       // NULL indicates that it is a delay transition
                        if (last != NULL)
                            last->setParent(mc);          // set the parent of the last marking

                        last = mc;
                        mc->cut();
                        if(l == NULL)                   // set specific last marking to last marking in delay if deadlock
                            l = mc;
                        if(diff)                        // seems obsolete TODO check effect of remove.
                                traceStack.push(mc);            // add delay marking to the trace
                        mc->setParent(NULL);
                        diff--;
                    }
            }
            if (l == NULL) {                            // set last marking to first reached marking satisfying the prop if not set prior to this.
                l = new NonStrictMarkingBase(*getBase(lastMarking->dart));
                trace = ((TraceDart*) lastMarking);
                l->incrementAge(trace->start);
                l->cut();
                l->setParent(NULL);
            }
            while (trace != NULL) {
                int lower = trace->start;

                // create "last delay marking"
                NonStrictMarkingBase* base = getBase(trace->dart);
                NonStrictMarkingBase* m = new NonStrictMarkingBase(*base);
                m->setGeneratedBy(trace->generatedBy);
                m->incrementAge(lower);
                m->setParent(NULL);
                if(upper == INT_MAX){
                    upper = tapn.get()->getMaxConstant();
                }
                // create markings between transitions, one for each delay (exluding last one)
                if (upper > lower) {
                    int diff = upper - lower;   // amount to delay
                    while (diff) {
                        NonStrictMarkingBase* mc = new NonStrictMarkingBase(*base);
                        mc->setParent(NULL);
                        mc->incrementAge(lower + diff);
                        mc->setGeneratedBy(NULL);       // NULL indicates that it is a delay transition
                        if (last != NULL)
                            last->setParent(mc);          // set the parent of the last marking
                        if(!l->getParent())
                            l->setParent(mc);
                        last = mc;
                        mc->cut();
                        traceStack.push(mc);            // add delay marking to the trace
                        diff--;
                    }
                }
                if (last != NULL)
                    last->setParent(m);
                if(!l->getParent())
                            l->setParent(m);
                m->cut();
                last = m;
                traceStack.push(m);     // add the marking to the trace

                upper = trace->upper;
                trace = (TraceDart*) trace->parent;
            }
            
            printXMLTrace(l, traceStack, query->getQuantifier());
        }

    }
}
