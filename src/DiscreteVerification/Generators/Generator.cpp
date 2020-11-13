/* 
 * File:   Generator.cpp
 * Author: Peter G. Jensen
 *
 * Created on 15 September 2015, 17:25
 */
#include "DiscreteVerification/Generators/Generator.h"


namespace VerifyTAPN {
    namespace DiscreteVerification {

        Generator::Generator(const TAPN::TimedArcPetriNet &tapn, AST::Query *query)
        : _tapn(tapn), _transition_iterator(tapn), _permutation(_transition_iterator.max_tokens()),
            _base_permutation(), _query(query),
            _transitionStatistics(tapn.getTransitions().size()) {
        }

        void Generator::prepare(NonStrictMarkingBase *parent) {
            this->_parent = parent;
            reset();
        }

        void Generator::reset() {
            _transition_iterator.prepare(_parent);
            _num_children = 0;
            _current = nullptr;
            _done = false;
            _seen_urgent = false;
        }

        bool Generator::only_transition(const TAPN::TimedTransition *trans) {
            if (_transition_iterator.is_enabled(trans, &_base_permutation)) {
                _current = trans;
                _permutation = _base_permutation;
                return true;
            }
            _current = nullptr;
            return false;
        }

        NonStrictMarkingBase *Generator::next(bool do_delay)
        {
            return _next(do_delay, [](auto a) { return true; });
        }

        NonStrictMarkingBase *Generator::_next(bool do_delay, std::function<bool(const TimedTransition*)> filter) {
            if (_done) return nullptr;

            // we need more permutations
            if (_current)
                return fire(_current);

            // move to next transition
            auto [transition, no_input] = _transition_iterator.next_transition(&_base_permutation);
            if (transition != nullptr) {
                if (no_input)
                    return fire_no_input(transition);
                else {
                    _permutation = _base_permutation; // start from zero
                    return fire(transition);
                }
            }
            // done, only delay waiting
            _done = true;
            if (do_delay && !_seen_urgent)
                return from_delay();
            return nullptr;
        }

        NonStrictMarkingBase *Generator::from_delay() {
            for (auto &place : _parent->getPlaceList()) {
                int inv = place.place->getInvariant().getBound();
                if (place.maxTokenAge() == inv) {
                    return nullptr;
                }
            }
            auto *m = new NonStrictMarkingBase(*_parent);
            m->incrementAge();
            _last_fired = nullptr;
            ++_num_children;
            return m;
        }

        NonStrictMarkingBase *Generator::fire_no_input(const TimedTransition* trans) {
            _last_fired = trans;
            _seen_urgent |= trans->isUrgent();
            ++_num_children;
            ++_transitionStatistics[trans->getIndex()];

            // lowhanging fruits first!
            NonStrictMarkingBase *child = new NonStrictMarking(*_parent);

            auto &postset = trans->getPostset();
            // could be optimized
            for (auto arc : postset) {
                Token t = Token(0, arc->getWeight());
                child->addTokenInPlace(arc->getOutputPlace(), t);
            }
            return child;
        }

        NonStrictMarkingBase* Generator::fire(const TimedTransition* trans) {
            _current = trans;
            _last_fired = trans;
            _seen_urgent |= trans->isUrgent();
            ++_num_children;
            ++_transitionStatistics[trans->getIndex()];
            return next_transition_permutation();
        }

        NonStrictMarkingBase *Generator::next_transition_permutation() {
            if (_current == nullptr) {
                assert(false);
                return nullptr;
            }
            auto *child = new NonStrictMarkingBase(*_parent);
            child->setGeneratedBy(nullptr);
            child->setParent(nullptr);
            int arccounter = 0;
            int last_movable = -1;
            PlaceList &placelist = child->getPlaceList();
            auto pit = placelist.begin();
            for (auto &input : _current->getPreset()) {
                int source = input->getInputPlace().getIndex();

                while (pit->place->getIndex() != source) {
                    ++pit;
                    assert(pit != placelist.end());
                }

                auto &tokenlist = pit->tokens;

                for (int i = input->getWeight() - 1; i >= 0; --i) {
                    size_t t_index = _permutation[arccounter + i];
                    size_t t_next = t_index + 1;
                    if (t_next < tokenlist.size() && input->getInterval().contains(
                            tokenlist[t_next].getAge())) {
                        last_movable = std::max(last_movable, arccounter + i);
                    }

                    assert(t_index < tokenlist.size());
                    if (tokenlist[t_index].getCount() > 1) tokenlist[t_index].remove(1);
                    else {
                        tokenlist.erase(tokenlist.begin() + t_index);
                        if (tokenlist.empty()) pit = placelist.erase(pit);
                    }
                }
                arccounter += input->getWeight();
            }

            pit = placelist.begin();
            // This has a problem if source and destination are the same!
            // TODO Also fix faster removal of tokens here!
            for (auto &transport : _current->getTransportArcs()) {
                int source = transport->getSource().getIndex();
                while (pit->place->getIndex() != source) {
                    ++pit;
                    assert(pit != placelist.end());
                }
                for (int i = transport->getWeight() - 1; i >= 0; --i) {
                    auto &tokenlist = pit->tokens;
                    size_t t_index = _permutation[arccounter + i];

                    size_t t_next = t_index + 1;
                    if (t_next < tokenlist.size() && transport->getInterval().contains(
                            tokenlist[t_next].getAge())) {
                        last_movable = std::max(last_movable, arccounter + i);
                    }

                    assert(t_index < tokenlist.size());
                    const Token token = tokenlist[t_index];
                    child->removeToken(source, token.getAge());
                    child->addTokenInPlace(transport->getDestination(),
                            token.getAge());
                    pit = placelist.begin();
                    while (pit != placelist.end() && pit->place->getIndex() < source) {
                        ++pit;
                    }
                }
                arccounter += transport->getWeight();
            }

            pit = placelist.begin();
            for (auto* output : _current->getPostset()) {
                while (pit != placelist.end() &&
                        pit->place->getIndex() < output->getOutputPlace().getIndex())
                    ++pit;
                if (pit != placelist.end() &&
                        pit->place->getIndex() == output->getOutputPlace().getIndex()) {
                    if (pit->tokens[0].getAge() == 0)
                        pit->tokens[0].add(output->getWeight());
                    else {
                        pit->tokens.insert(
                                pit->tokens.begin(), Token(0, output->getWeight()));
                    }
                } else {
                    pit = placelist.insert(pit, Place(&output->getOutputPlace()));
                    pit->tokens.insert(
                            pit->tokens.begin(), Token(0, output->getWeight()));
                }
            }
            // nobody can move
            if (last_movable == -1) _current = nullptr;
            else {
                _permutation[last_movable] += 1;
                for (int i = last_movable + 1; i < arccounter; ++i) {
                    _permutation[i] = _base_permutation[i];
                }
            }
            return child;
        }

        size_t Generator::children() {
            return _num_children;
        }

        void Generator::printTransitionStatistics(std::ostream &out) const {
            out << std::endl << "TRANSITION STATISTICS";
            for (unsigned int i = 0; i < _transitionStatistics.size(); i++) {
                if ((i) % 6 == 0) {
                    out << std::endl;
                    out << "<" << _tapn.getTransitions()[i]->getName() << ":" << _transitionStatistics[i] << ">";
                } else {
                    out << " <" << _tapn.getTransitions()[i]->getName() << ":" << _transitionStatistics[i] << ">";
                }
            }
            out << std::endl;
            out << std::endl;
        }
    }
}