/* 
 * File:   Generator.cpp
 * Author: Peter G. Jensen
 *
 * Created on 15 September 2015, 17:25
 */
#include "Generator.h"


namespace VerifyTAPN {
    namespace DiscreteVerification {
         
        Generator::Generator(TAPN::TimedArcPetriNet& tapn) 
        : tapn(tapn), allways_enabled(), 
                place_transition(tapn.getPlaces().size()), permutation(), 
                base_permutation()
        {
            size_t maxtokens = 0;
            for (auto transition : tapn.getTransitions()) {
                
                if (transition->getPreset().size() + transition->getTransportArcs().size() == 0) {
                    allways_enabled.push_back(transition);
                }         
                else
                {
                    size_t tokens = 0;
                    int index = std::numeric_limits<int>::max();
                    for(auto arc : transition->getPreset())
                    {
                        index = std::min(arc->getInputPlace().getIndex(), index);
                        tokens += arc->getWeight();
                    }
                    
                    for(auto arc : transition->getTransportArcs())
                    {
                        index = std::min(arc->getSource().getIndex(), index);                        
                        tokens += arc->getWeight();
                    }
                    place_transition[index].push_back(transition);
                    maxtokens = std::max( tokens, 
                                        maxtokens);
                }
            }
            base_permutation.resize(maxtokens);
        }
        
        void Generator::from_marking(NonStrictMarkingBase* parent, Mode mode, bool seen_urgent)
        {
            this->parent = parent;
            this->mode = mode;
            noinput_nr = 0;
            place = 0;
            transition = 0;
            current = NULL;
            done = false;
            this->seen_urgent = seen_urgent;
            did_noinput = false;
        }
        
        NonStrictMarkingBase* Generator::next(bool do_delay)
        {
            if(done) return NULL;
            // easy ones, continue to we find a result or out of transitions
            while(!did_noinput && noinput_nr < allways_enabled.size())
            {
                NonStrictMarkingBase* marking = next_no_input();
                ++noinput_nr;
                if(marking) return marking;
            }

            // hard ones, continue to we find a result, or out of transitions
            while(next_transition(!did_noinput))
            {
                did_noinput = true;
                // from the current transition, try the next child
                NonStrictMarkingBase* child = next_from_current();
                if(child) return child;
            }
            
            done = true;                            
            if(!seen_urgent && do_delay)
            {
                return from_delay();
            }
            return NULL;
        }
        
        NonStrictMarkingBase* Generator::from_delay()
        {
            for(auto& place : parent->getPlaceList())
            {
                int inv = place.place->getInvariant().getBound();
                if (place.maxTokenAge() == inv) {
                    return NULL;
                }
            }
            NonStrictMarkingBase* m = new NonStrictMarkingBase(*parent);
            m->incrementAge();
            return m;
        }
        
        bool Generator::modes_match(const TAPN::TimedTransition* trans)
        {
            switch(mode)
            {
                case Mode::CONTROLLABLE:
                    if(!trans->isControllable()) return false;
                    break;
                case Mode::ENVIRONMENT:
                    if(trans->isControllable()) return false;
                    break;
                case Mode::ALL:
                    break;
            }
            return true;
        }
        
        NonStrictMarkingBase* Generator::next_no_input()
        {
            // lowhanging fruits first!
            NonStrictMarkingBase* child = new NonStrictMarking(*parent);
            auto trans = allways_enabled[noinput_nr];
            seen_urgent |= trans->isUrgent();            
            if(!modes_match(trans)) return NULL;
                
            auto& postset = trans->getPostset();
            // could be optimized 
            for(auto arc : postset)
            {
                Token t = Token(0, arc->getWeight()); 
                child->addTokenInPlace(arc->getOutputPlace(), t);
            }
            return child;
        }
        
        NonStrictMarkingBase* Generator::next_from_current()
        {
            if(current == NULL) return NULL;
            NonStrictMarkingBase* child = new NonStrictMarkingBase(*parent);
            child->setGeneratedBy(NULL);
            child->setParent(NULL);
            size_t arccounter = 0;
            int last_movable = -1;

            for(auto& input : current->getPreset())
            {
                for(int i = 0; i < input->getWeight(); ++i)
                {
                    size_t t_index = permutation[arccounter];
                    int source = input->getInputPlace().getIndex();
                    auto& tokenlist = child->getTokenList(source);
                    const Token& token = tokenlist[permutation[arccounter]];
                    child->removeToken(source, token.getAge());

                    ++t_index;
                    if(t_index != tokenlist.size() && input->getInterval().contains(
                            tokenlist[t_index].getAge()))
                    {
                        last_movable = arccounter;
                    }
                    ++arccounter;
                }
            }

            for(auto& transport : current->getTransportArcs())
            {
                for(int i = 0; i < transport->getWeight(); ++i)
                {
                    size_t t_index = permutation[arccounter];
                    int source = transport->getSource().getIndex();
                    auto& tokenlist = child->getTokenList(source);
                    const Token token = tokenlist[t_index];
                    child->removeToken(source, token.getAge());
                    child->addTokenInPlace(transport->getDestination(), 
                            token.getAge());

                    ++t_index;
                    if(t_index != tokenlist.size() && transport->getInterval().contains(
                            tokenlist[t_index].getAge()))
                    {
                        last_movable = arccounter;
                    }
                    ++arccounter;
                }
            }
                        
            for(auto& output : current->getPostset())
            {
                Token t = Token(0, output->getWeight());
                child->addTokenInPlace(output->getOutputPlace(), t);
            }

            // nobody can move
            if(last_movable == -1) current = NULL;
            else
            {
                permutation[last_movable] += 1;
                for(size_t i = last_movable + 1; i < arccounter; ++i)
                {
                    permutation[i] = base_permutation[i];
                }
            }
            return child;
        }
        
        bool Generator::next_transition(bool isfirst)
        {            
            if(parent->getPlaceList().size() == 0) return false;
            do{
                if(!isfirst)
                    ++transition;
                isfirst = false;

                size_t placeindex = parent->getPlaceList()[place].place->getIndex();                
                if(transition >= place_transition[placeindex].size())
                {
                    ++place;
                    transition = 0;
                }

                // no more places with tokens, we are done
                if(place >= parent->getPlaceList().size())
                {
                    return false;
                }
                placeindex = parent->getPlaceList()[place].place->getIndex();
                // if no transitions out, skip
                if(place_transition[placeindex].size() == 0) continue;

                current = place_transition[placeindex][transition];
                if(is_enabled())
                    return true;
            } while (true);
        }
        
        bool Generator::is_enabled()
        {
            // Check inhibitors
            if(!modes_match(current)) return false;

            for(auto& inhib : current->getInhibitorArcs())
            {
                int tokens = parent->numberOfTokensInPlace(
                                            inhib->getInputPlace().getIndex());
                if(tokens >= inhib->getWeight())
                {
                    return false;
                }
            }   
            
            
            size_t arccounter = 0;
            
            for(auto& input : current->getPreset())
            {
                base_permutation[arccounter] = std::numeric_limits<size_t>::max();
                int source = input->getInputPlace().getIndex();
                int weight = input->getWeight();
                auto& tokenlist = parent->getTokenList(source);
                for(size_t index = 0; index < tokenlist.size(); ++index)
                {
                    auto& token = tokenlist[index];
                    if(input->getInterval().contains(token.getAge()))
                    {
                        for(int i = 0; i < std::min(weight, token.getCount()); ++i)
                        {
                            base_permutation[arccounter] = std::min(
                                    index, 
                                    base_permutation[arccounter]
                                    );
                            ++arccounter;
                        }
                        weight -= token.getCount();
                        if(weight < 0) break;
                    }
                }
                if(weight > 0) return false;
            }

            for(auto& transport : current->getTransportArcs())
            {
                base_permutation[arccounter] = std::numeric_limits<size_t>::max();
                int source = transport->getSource().getIndex();
                int weight = transport->getWeight();
                auto& tokenlist = parent->getTokenList(source);
                for(size_t index = 0; index < tokenlist.size(); ++index)
                {
                    auto& token = tokenlist[index];
                    if(transport->getInterval().contains(token.getAge()))
                    {
                        for(int i = 0; i < std::min(weight, token.getCount()); ++i)
                        {
                            base_permutation[arccounter] = std::min(
                                    index, 
                                    base_permutation[arccounter]
                                    );
                            ++arccounter;
                        }
                        weight -= token.getCount();
                        if(weight < 0) break;
                    }
                }
                if(weight > 0) return false;
            }

            permutation = base_permutation;
            seen_urgent |= current->isUrgent();
            return true;
        }

        size_t Generator::children()
        {
            return noinput_nr;
        }
    }
}