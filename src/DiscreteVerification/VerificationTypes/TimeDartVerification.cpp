#include "DiscreteVerification/VerificationTypes/TimeDartVerification.hpp"
#include "DiscreteVerification/DeadlockVisitor.hpp"

namespace VerifyTAPN { namespace DiscreteVerification {

   

    TimeDartVerification::TimeDartVerification(TAPN::TimedArcPetriNet &tapn, const VerificationOptions &options,
                                               AST::Query *query, NonStrictMarkingBase &initialMarking) :
            Verification<NonStrictMarkingBase>(tapn, initialMarking, query, options), exploredMarkings(0),
            allwaysEnabled(), successorGenerator(tapn, query) {
        loop = false;
        deadlock = false;
        //Find the transitions which don't have input arcs
        for (auto* t : tapn.getTransitions()) {
            if (t->getPreset().size() + t->getTransportArcs().size() == 0) {
                allwaysEnabled.push_back(t);
            }
            if (t->isUrgent()) { // no implementation for urgency in timedart engine yet
                std::cout << "The TimeDart engine cannot handle urgent transitions" << std::endl;
                std::exit(1);
            }
        }
        if(options.getPartialOrderReduction())
        {
           std::cout << "TimeDarts and partial order reduction no supported" << std::endl;
           std::exit(1);
        }
    }

    std::pair<int, int>
    TimeDartVerification::calculateStart(const TAPN::TimedTransition &transition, NonStrictMarkingBase *marking) {
        std::vector<Util::interval<>> start;
        Util::interval initial(0, std::numeric_limits<int32_t>::max());
        start.push_back(initial);

        if (transition.getNumberOfInputArcs() + transition.getNumberOfTransportArcs() == 0) { //always enabled
            std::pair<int, int> p(0, maxPossibleDelay(marking));
            return p;
        }

        // Inhibitor arcs
        for (auto* arc : transition.getInhibitorArcs()) {
            if (marking->numberOfTokensInPlace(arc->getInputPlace().getIndex()) >= arc->getWeight()) {
                std::pair<int, int> p(-1, -1);
                return p;
            }
        }


        // Standard arcs
        for (auto* arc : transition.getPreset()) {
            std::vector<Util::interval<>> intervals;
            int range;
            if (arc->getInterval().getUpperBound() == std::numeric_limits<int32_t>::max()) {
                range = std::numeric_limits<int32_t>::max();
            } else {
                range = arc->getInterval().getUpperBound() - arc->getInterval().getLowerBound();
            }
            int weight = arc->getWeight();

            TokenList tokens = marking->getTokenList(arc->getInputPlace().getIndex());
            if (tokens.size() == 0) {
                std::pair<int, int> p(-1, -1);
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
                if (numberOfTokensAvailable >= weight &&
                    tokens.at(j).getAge() - tokens.at(i).getAge() <= range) { //This span is interesting
                    int low = arc->getInterval().getLowerBound() - tokens.at(i).getAge();
                    int heigh = arc->getInterval().getUpperBound() - tokens.at(j).getAge();

                    Util::interval element(low < 0 ? 0 : low,
                                           arc->getInterval().getUpperBound() == std::numeric_limits<int32_t>::max() ? std::numeric_limits<int32_t>::max() : heigh);
                    Util::setAdd(intervals, element);
                }
                numberOfTokensAvailable -= tokens.at(i).getCount();
            }

            start = Util::setIntersection(start, intervals);
        }

        // Transport arcs
        for (auto* arc : transition.getTransportArcs()) {
            Util::interval arcGuard(arc->getInterval().getLowerBound(), arc->getInterval().getUpperBound());
            Util::interval invGuard(0, arc->getDestination().getInvariant().getBound());

            Util::interval arcInterval = Util::intersect(arcGuard, invGuard);
            std::vector<Util::interval<>> intervals;
            int range;
            if (arcInterval.upper() == std::numeric_limits<int32_t>::max()) {
                range = std::numeric_limits<int32_t>::max();
            } else {
                range = arcInterval.upper() - arcInterval.lower();
            }
            int weight = arc->getWeight();

            TokenList tokens = marking->getTokenList(arc->getSource().getIndex());

            if (tokens.size() == 0) {
                std::pair<int, int> p(-1, -1);
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
                if (numberOfTokensAvailable >= weight &&
                    tokens.at(j).getAge() - tokens.at(i).getAge() <= range) { //This span is interesting
                    Util::interval element(arcInterval.lower() - tokens.at(i).getAge(),
                                           arcInterval.upper() - tokens.at(j).getAge());
                    Util::setAdd(intervals, element);
                }
                numberOfTokensAvailable -= tokens.at(i).getCount();
            }

            start = Util::setIntersection(start, intervals);
        }

        int invariantPart = maxPossibleDelay(marking);

        std::vector<Util::interval<>> invEnd;
        Util::interval initialInv(0, invariantPart);
        invEnd.push_back(initialInv);
        start = Util::setIntersection(start, invEnd);

#if DEBUG
        std::cout << "Intervals in start: " << start.size() << std::endl;
#endif

        if (start.empty()) {
            std::pair<int, int> p(-1, -1);
            return p;
        } else {
            std::pair<int, int> p(start.front().lower(), start.back().upper());
            return p;
        }
    }

    int
    TimeDartVerification::calculateStop(const TAPN::TimedTransition &transition, NonStrictMarkingBase *marking) {
        int MC = -1;
        unsigned int i = 0;
        for (auto& iter : marking->getPlaceList()) {
            if (i < transition.getPreset().size() &&
                iter.place->getIndex() == transition.getPreset().at(i)->getInputPlace().getIndex()) {
                if (transition.getPreset().at(i)->getWeight() < iter.numberOfTokens()) {
                    MC = std::max(MC, iter.place->getMaxConstant() - iter.tokens.front().getAge());
                }
                i++;
                continue;
            }
            MC = std::max(MC, iter.place->getMaxConstant() - iter.tokens.front().getAge());
        }

        return MC + 1;
    }

    int TimeDartVerification::maxPossibleDelay(NonStrictMarkingBase *marking) {
        int invariantPart = std::numeric_limits<int32_t>::max();

        for (auto& iter : marking->getPlaceList()) {
            if (iter.place->getInvariant().getBound() != std::numeric_limits<int>::max() &&
                iter.place->getInvariant().getBound() - iter.tokens.back().getAge() < invariantPart) {
                invariantPart = iter.place->getInvariant().getBound() - iter.tokens.back().getAge();
            }
        }

        return invariantPart;
    }

    bool TimeDartVerification::generateAndInsertSuccessors(NonStrictMarkingBase &marking,
                                                           const TAPN::TimedTransition &transition) {
        successorGenerator.prepare(&marking);
        if (!successorGenerator.only_transition(&transition))
            return false;
        while (auto next = successorGenerator.next(false)) {
            next->setGeneratedBy(successorGenerator.last_fired());
            if (handleSuccessor(next))
                return true;
        }
        return false;
    }

    void TimeDartVerification::getTrace() {

        std::stack<NonStrictMarkingBase *> traceStack;

        auto *trace = (TraceDart *) lastMarking;
        int upper = trace->start;
        NonStrictMarkingBase *last = nullptr;
        NonStrictMarkingBase *l = nullptr;

        DeadlockVisitor deadlockVisitor = DeadlockVisitor();
        AST::BoolResult queryContainsDeadlock;
        deadlockVisitor.visit(*query, queryContainsDeadlock);

        // only if we have reached a deadlock (liveness) or
        // have a reachability query (for delay when deadlock prop is used) then
        // we want to create some end-delay
        if (deadlock || queryContainsDeadlock.value ||
            query->getQuantifier() == AST::EG || query->getQuantifier() == AST::AF) {
            NonStrictMarkingBase *base = getBase(trace->dart);

            int diff = this->maxPossibleDelay(base) - trace->start;
            // fix for when max possible delay = inf
            if (diff > tapn.getMaxConstant()) {
                diff = (tapn.getMaxConstant() + 1) - trace->start;
            }

            // find the lowest delay at which the query is satisfied
            while (diff && base->canDeadlock(tapn, trace->start + diff - 1)) {
                --diff;
            }

            while (diff) {  // while there is some delay in trace
                auto *mc = new NonStrictMarkingBase(*base);
                mc->incrementAge(trace->start + diff);
                mc->setGeneratedBy(nullptr);       // nullptr indicates that it is a delay transition

                if (last != nullptr) {
                    last->setParent(mc);        // set the parent of the last marking
                    mc->setNumberOfChildren(1);
                }

                last = mc;

                if (l == nullptr) {                  // set specific last marking to last marking in delay if deadlock
                    l = new NonStrictMarkingBase(*mc);
                }
                mc->cut(placeStats);

                traceStack.push(mc);            // add delay marking to the trace
                mc->setParent(nullptr);            // set parrent
                diff--;
            }
        }
        if (l ==
            nullptr) {                            // set last marking to first reached marking satisfying the prop if not set prior to this.
            l = new NonStrictMarkingBase(*getBase(lastMarking->dart));
            trace = ((TraceDart *) lastMarking);
            l->incrementAge(trace->start);
            l->cut(placeStats);
            l->setParent(nullptr);
            l->setGeneratedBy(((TraceDart *) lastMarking)->generatedBy);
        }
        while (trace != nullptr) {
            int lower = trace->start;

            // create "last delay marking"
            NonStrictMarkingBase *base = getBase(trace->dart);
            auto *m = new NonStrictMarkingBase(*base);
            m->setGeneratedBy(trace->generatedBy);
            m->incrementAge(lower);
            m->setParent(nullptr);
            if (upper == std::numeric_limits<int32_t>::max()) {
                upper = tapn.getMaxConstant();
            }
            // create markings between transitions, one for each delay (exluding last one)
            if (upper > lower) {
                int diff = upper - lower;   // amount to delay
                while (diff) {
                    auto *mc = new NonStrictMarkingBase(*base);
                    mc->setParent(nullptr);
                    mc->incrementAge(lower + diff);
                    mc->setGeneratedBy(nullptr);       // nullptr indicates that it is a delay transition
                    if (last != nullptr) {
                        last->setParent(mc);          // set the parent of the last marking
                        mc->setNumberOfChildren(1);
                    }
                    if (!l->getParent()) {
                        l->setParent(mc);
                    }
                    last = mc;
                    mc->cut(placeStats);
                    traceStack.push(mc);            // add delay marking to the trace
                    diff--;
                }
            }
            if (last != nullptr) {
                last->setParent(m);
                m->setNumberOfChildren(1);
            }
            if (!l->getParent()) {
                l->setParent(m);
                m->setNumberOfChildren(1);
            }
            m->cut(placeStats);
            last = m;
            traceStack.push(m); // add the marking to the trace

            upper = trace->upper;
            trace = (TraceDart *) trace->parent;
        }

        if (!deadlock) {
            l->setNumberOfChildren(1);
        }

        printXMLTrace(l, traceStack, query, tapn);
    }
} }
