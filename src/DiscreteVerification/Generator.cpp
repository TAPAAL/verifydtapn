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
            size_t maxarcs = 0;
            for (auto transition : tapn.getTransitions()) {
                
                if (transition->getPreset().size() + transition->getTransportArcs().size() == 0) {
                    allways_enabled.push_back(transition);
                }         
                else
                {
                    int index = std::numeric_limits<int>::max();
                    for(auto arc : transition->getPreset())
                    {
                        index = std::min(arc->getInputPlace().getIndex(), index);
                    }
                    
                    for(auto arc : transition->getTransportArcs())
                    {
                        index = std::min(arc->getSource().getIndex(), index);                        
                    }
                    place_transition[index].push_back(transition);
                    maxarcs = std::max( transition->getPreset().size() + 
                                        transition->getPostset().size(), 
                                        maxarcs);
                }
            }
            base_permutation.resize(maxarcs);
        }
        
        void Generator::from_marking(NonStrictMarkingBase* parent, Mode mode)
        {
            this->parent = parent;
            this->mode = mode;
            successors = 0;
            place = 0;
            transition = 0;
            current = 0;
        }
        
        NonStrictMarkingBase* Generator::next()
        {
            // easy ones, continue to we find a result or out of transitions
            while(successors < allways_enabled.size())
            {
                NonStrictMarkingBase* marking = next_no_input();
                if(marking) return marking;
            }
            
            // hard ones, continue to we find a result, or out of transitions
            while(true)
            {
                // from the current transition, try the next child
                NonStrictMarkingBase* child = next_from_current();
                if(child) return child;
                // find the next transition
                if(!next_transition()) return NULL;
            }
        }
        
        NonStrictMarkingBase* Generator::next_no_input()
        {
            // lowhanging fruits first!
            NonStrictMarkingBase* child = new NonStrictMarking(*parent);
            auto& postset = allways_enabled[successors]->getPostset();
            // could be optimized h
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
            for(auto& transport : current->getTransportArcs())
            {
                int t_index = permutation[arccounter];
                int source = transport->getSource().getIndex();
                auto& tokenlist = child->getTokenList(source);
                const Token& token = tokenlist[t_index];
                child->removeToken(source, token.getAge());
                child->addTokenInPlace(transport->getDestination(), 
                        token.getAge());
                
                if(t_index != 0 && transport->getInterval().contains(
                        tokenlist[t_index-1].getAge()))
                {
                    last_movable = arccounter;
                }
                
                ++arccounter;
            }
            
            for(auto& input : current->getPreset())
            {
                int t_index = permutation[arccounter];
                int source = input->getInputPlace().getIndex();
                auto& tokenlist = child->getTokenList(source);
                const Token& token = tokenlist[permutation[arccounter]];
                child->removeToken(source, token.getAge());

                if(t_index != 0 && input->getInterval().contains(
                        tokenlist[t_index-1].getAge()))
                {
                    last_movable = arccounter;
                }
                ++arccounter;
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
                permutation[last_movable] -= 1;
                for(size_t i = last_movable + 1; i < arccounter; ++i)
                {
                    permutation[i] = base_permutation[i];
                }
            }
            return child;
        }
        
        bool Generator::next_transition()
        {
            do{
                ++transition;
                if(transition >= place_transition[place].size())
                {
                    ++place;
                    transition = 0;
                }

                // no more places with tokens, we are done
                if(place >= parent->getPlaceList().size())
                {
                    return false;
                }

                // if no transitions out, skip
                if(place_transition[place].size() == 0) continue;

                current = place_transition[place][transition];
                if(is_enabled())
                    return true;
            } while (true);
        }
        
        bool Generator::is_enabled()
        {
            for(auto& inhib : current->getInhibitorArcs())
            {
                size_t tokens = parent->numberOfTokensInPlace(
                                            inhib->getInputPlace().getIndex());
                if(tokens > inhib->getWeight())
                {
                    return false;
                }
            }   
            size_t arccounter = 0;
            for(auto& transport : current->getTransportArcs())
            {
                base_permutation[arccounter] = 0;
                int source = transport->getSource().getIndex();
                int weight = transport->getWeight();
                auto& tokenlist = parent->getTokenList(source);
                for(size_t index = 0; index < tokenlist.size(); ++index)
                {
                    auto& token = tokenlist[index];
                    if(transport->getInterval().contains(token.getAge()))
                    {
                        base_permutation[arccounter] = std::max(
                                index, 
                                base_permutation[arccounter]
                                );
                        weight -= token.getCount();
                    }
                }
                if(weight > 0) return false;
                ++arccounter;
            }
            
            for(auto& input : current->getPreset())
            {
                base_permutation[arccounter] = 0;
                int source = input->getInputPlace().getIndex();
                int weight = input->getWeight();
                auto& tokenlist = parent->getTokenList(source);
                for(size_t index = 0; index < tokenlist.size(); ++index)
                {
                    auto& token = tokenlist[index];
                    if(input->getInterval().contains(token.getAge()))
                    {
                        base_permutation[arccounter] = std::max(
                                index, 
                                base_permutation[arccounter]
                                );
                        weight -= token.getCount();
                    }
                }
                if(weight > 0) return false;
                ++arccounter;
            }
            permutation = base_permutation;
            return true;
        }

        size_t Generator::children()
        {
            return successors;
        }
    }
}