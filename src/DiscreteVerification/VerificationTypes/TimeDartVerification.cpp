#include "TimeDartVerification.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace std;

        pair<int, int> TimeDartVerification::calculateStart(const TAPN::TimedTransition& transition, NonStrictMarkingBase* marking) {
            vector<Util::interval > start;
            Util::interval initial(0, INT_MAX);
            start.push_back(initial);

            if (transition.NumberOfInputArcs() + transition.NumberOfTransportArcs() == 0) { //always enabled
                pair<int, int> p(0, INT_MAX);
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

        void TimeDartVerification::PrintXMLTraceTimeDart(NonStrictMarkingBase* m, std::stack<NonStrictMarkingBase*>& stack, AST::Quantifier query) {
            using namespace rapidxml;
            std::cerr << "Trace: " << std::endl;
            bool isFirst = true;
            bool foundLoop = false;
            bool delayedForever = false;
            NonStrictMarkingBase* old;

            std::stack<NonStrictMarkingBase*> newStack = std::stack<NonStrictMarkingBase*>(); 
            
            xml_document<> doc;
            xml_node<>* root = doc.allocate_node(node_element, "trace");
            doc.append_node(root);

            //create initial delay
            stack.pop();
            NonStrictMarkingBase* next = stack.top();

            int initial = next->makeBase(tapn.get());
            if (initial > 0) {
                xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(initial).c_str()));
                root->append_node(node);
                next->incrementAge(initial);
            }
            old = stack.top();
            stack.pop();

            while (!stack.empty()) {
                CreateTransitionNode(old, stack.top(), doc);
                old = stack.top();             
                if(stack.top()->equals(*m)){
                    root->append_node(doc.allocate_node(node_element, "loop"));
                }
                stack.pop();
            }

            /*
             while (!stack.empty()) {
                 if (isFirst) {
                     isFirst = false;
                 } else {
                     if (stack.top()->GetGeneratedBy()) {
                         root->append_node(CreateTransitionNode(old, stack.top(), doc));
                     } else {
                         int i = 1;
                         old = stack.top();
                         stack.pop();
                         while (!stack.empty() && stack.top()->GetGeneratedBy() == NULL) {
                             old = stack.top();
                             stack.pop();
                             i++;
                         }

                         if (stack.empty() && old->children > 0) {
                             xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string("forever"));
                             root->append_node(node);
                             delayedForever = true;
                             break;
                         }
                         xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(i).c_str()));
                         root->append_node(node);
                         stack.push(old);
                     }
                 }

                 if ((query == AST::EG || query == AST::AF)
                         && (stack.size() > 1 && stack.top()->equals(*m))
                         && (m->GetGeneratedBy() || stack.top()->parent)) {
                     root->append_node(doc.allocate_node(node_element, "loop"));
                     foundLoop = true;
                 }
                 old = stack.top();
                 stack.pop();
             }

             //Trace ended, goto * or deadlock
             if (query == AST::EG || query == AST::AF) {
                 if (!foundLoop && !delayedForever) {
                     if (m->children > 0) {
                         root->append_node(doc.allocate_node(node_element, "deadlock"));
                     } else {
                         // By default delay forever
                         xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string("forever"));
                         for (PlaceList::const_iterator iter = m->places.begin(); iter != m->places.end(); iter++) {
                             if (iter->place->GetInvariant().GetBound() != std::numeric_limits<int>::max()) {
                                 //Invariant, deadlock instead of delay forever
                                 node = doc.allocate_node(node_element, "deadlock");
                                 break;
                             }
                         }
                         root->append_node(node);
                     }
                 }
             }
             */
            std::cerr << doc;
        }

        void TimeDartVerification::CreateTransitionNode(NonStrictMarkingBase* old, NonStrictMarkingBase* current, rapidxml::xml_document<>& doc) {
            using namespace rapidxml;


            xml_node<>* transitionNode = doc.allocate_node(node_element, "transition");
            xml_attribute<>* id = doc.allocate_attribute("id", current->generatedBy->GetId().c_str());
            transitionNode->append_attribute(id);
            NonStrictMarkingBase* intersect = new NonStrictMarkingBase(*current);


            // remove all the "new" tokens
            for (TAPN::OutputArc::WeakPtrVector::const_iterator arc_iter = current->generatedBy->GetPostset().begin(); arc_iter != current->generatedBy->GetPostset().end(); arc_iter++) {
                for (unsigned int i = 0; i < intersect->places.size(); i++) {
                    if (intersect->places[i].place->GetIndex() == (*arc_iter).lock()->OutputPlace().GetIndex()) {
                        intersect->places[i].tokens.begin()->remove((*arc_iter).lock()->GetWeight());
                    }
                }
                //createTransitionSubNodes(old, current, doc, transitionNode, arc_iter->lock()->InputPlace(), arc_iter->lock()->Interval(), arc_iter->lock()->GetWeight());
            }
            // intersect and oldage should be == now, except for transported tokens
            // transported tokens dont change age - hence, find out the delay and create basemarkings
            int intersectage = intersect->makeBase(tapn.get());
            int oldage = old->makeBase(tapn.get());
            // remove all "transported" tokens




            for (TAPN::TransportArc::WeakPtrVector::const_iterator arc_iter = current->generatedBy->GetTransportArcs().begin(); arc_iter != current->generatedBy->GetTransportArcs().end(); arc_iter++) {
                // find arc in new marking
                int dest = -1;
                int newsrc = -1;
                for (int i = 0; dest < intersect->places.size(); i++) { // find the right place in the intersect
                    if (intersect->places[i].place->GetIndex() == (*arc_iter).lock()->Destination().GetIndex()) {
                        dest = i;
                    }
                    if (intersect->places[i].place->GetIndex() == (*arc_iter).lock()->Source().GetIndex()) {
                        newsrc = i;
                    }
                    if (newsrc >= 0 && dest >= 0)
                        break;
                }
                // find in arc in old marking
                unsigned int src = 0;
                for (; src < old->places.size(); src++) { // find the right place in the intersect
                    if (old->places[src].place->GetIndex() == (*arc_iter).lock()->Source().GetIndex()) {
                        break;
                    }
                }
                // find ages
                
                    for (TokenList::const_iterator otit = old->places[src].tokens.begin(); otit != old->places[src].tokens.end(); otit++) {
                        
                        bool found = false;
                        if (newsrc >= 0) {      // if the src exists in the intersecttoken, find the ages
                            for (TokenList::const_iterator ntit = intersect->places[newsrc].tokens.begin(); ntit != intersect->places[newsrc].tokens.end(); ntit++) {
                                if(ntit->getAge() == otit->getAge()){
                                    if(ntit->getCount() != otit->getCount()){
                                        int diff = ntit->getCount() - otit->getCount(); // work in progress
                                    }
                                    found = true;
                                    break;
                                }
                            }
                        }
                        if(!found){     // if no matches found, remove all at destination
                            int count = otit->getCount();   // count to remove
                            while(count > 0){
                                intersect->RemoveToken((*arc_iter).lock()->Destination().GetIndex(),otit->getAge());
                                count--;
                            }
                        }
                    }
            }
            intersect->CleanUp();



            for (PlaceList::const_iterator oit = old->places.begin(); oit != old->places.end(); oit++) {
                bool handled = false;
                for (PlaceList::const_iterator iit = intersect->places.begin(); iit != intersect->places.end(); iit++) {
                    if (iit->place == oit->place) {
                        for (TokenList::const_iterator tot = oit->tokens.begin(); tot != oit->tokens.end(); tot++) {
                            TokenList::const_iterator tit = iit->tokens.begin();
                            while (tit->getAge() < tot->getAge()) {
                                tit++;
                            }
                            if (tit->getAge() != tot->getAge()) {
                                Token t(*tot);
                                t.add(oldage);
                                transitionNode->append_node(this->createTokenNode(doc, *oit->place, t));
                                continue;
                            }
                            if (tot->getCount() > tit->getCount()) {
                                Token t(tit->getAge() + oldage, tot->getCount() - tit->getCount());
                                transitionNode->append_node(this->createTokenNode(doc, *oit->place, t));
                            }
                        }
                        handled = true;
                        break;
                    }
                }
                if (!handled) {
                    for (TokenList::const_iterator tit = oit->tokens.begin(); tit != oit->tokens.end(); tit++) {
                        Token t(*tit);
                        t.incrementAge(oldage);
                        transitionNode->append_node(this->createTokenNode(doc, *oit->place, t));
                    }
                }

            }
            doc.first_node("trace",5,false)->append_node(transitionNode);
            if(intersectage > oldage){
                xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(intersectage-oldage).c_str()));
                doc.first_node("trace",5,false)->append_node(node);
            }
        }
    }
}
