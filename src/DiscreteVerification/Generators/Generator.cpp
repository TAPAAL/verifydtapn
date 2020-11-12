/* 
 * File:   Generator.cpp
 * Author: Peter G. Jensen
 *
 * Created on 15 September 2015, 17:25
 */
#include "DiscreteVerification/Generators/Generator.h"


namespace VerifyTAPN { namespace DiscreteVerification {

    Generator::Generator(TAPN::TimedArcPetriNet &tapn, AST::Query *query)
            : tapn(tapn), allways_enabled(),
              place_transition(), permutation(),
              base_permutation(), query(query),
              transitionStatistics(tapn.getTransitions().size()) {
        size_t maxtokens = 0;
        for (auto transition : tapn.getTransitions()) {

            if (transition->getPreset().size() + transition->getTransportArcs().size() == 0) {
                allways_enabled.push_back(transition);
            } else {
                size_t tokens = 0;
                int index = std::numeric_limits<int>::max();
                for (auto arc : transition->getPreset()) {
                    index = std::min(arc->getInputPlace().getIndex(), index);
                    tokens += arc->getWeight();
                }

                for (auto arc : transition->getTransportArcs()) {
                    index = std::min(arc->getSource().getIndex(), index);
                    tokens += arc->getWeight();
                }
                while (index >= place_transition.size()) place_transition.emplace_back();
                place_transition[index].push_back(transition);
                maxtokens = std::max(tokens,
                                     maxtokens);
            }
        }
        base_permutation.resize(maxtokens);
    }

    void Generator::from_marking(NonStrictMarkingBase *parent, Mode mode, bool seen_urgent) {
        this->parent = parent;
        this->mode = mode;
        num_children = 0;
        place = 0;
        transition = 0;
        current = nullptr;
        done = false;
        this->seen_urgent = seen_urgent;
        did_noinput = false;
        _trans = nullptr;
    }

    bool Generator::only_transition(const TAPN::TimedTransition *trans) {
        current = _trans = trans;
        if (is_enabled(trans, &base_permutation)) {
            permutation = base_permutation;
            return true;
        }
        current = _trans = nullptr;
        return false;
    }

    NonStrictMarkingBase *Generator::next(bool do_delay) {
        if (done) return nullptr;
        // easy ones, continue to we find a result or out of transitions
        while (!did_noinput && num_children < allways_enabled.size()) {
            NonStrictMarkingBase *marking = next_no_input();
            ++num_children;
            if (marking) return marking;
        }

        // hard ones, continue to we find a result, or out of transitions
        if (did_noinput) // First iteration of while has happend
        {
            if (_trans != nullptr && _trans != current) {
                _trans = nullptr;
                return nullptr;
            }
            NonStrictMarkingBase *child = next_from_current();
            if (child) {
                ++num_children;
                return child;
            }
        }

        if (!_trans) {
            while (next_transition(!did_noinput)) {
                if (_trans != nullptr && _trans != current) {
                    _trans = nullptr;
                    return nullptr;
                }
                did_noinput = true;
                // from the current transition, try the next child
                NonStrictMarkingBase *child = next_from_current();
                if (child) {
                    ++num_children;
                    return child;
                }
            }
        } else {
            if (did_noinput) current = nullptr;
            else {
                did_noinput = true;
                auto r = Generator::next(do_delay);
                if (r == nullptr) {
                    _trans = nullptr;
                }
                return r;
            }
        }

        done = true;
        if (!seen_urgent && do_delay && _trans == nullptr) {
            ++num_children;
            return from_delay();
        }
        _trans = nullptr;
        return nullptr;
    }

    NonStrictMarkingBase *Generator::from_delay() {
        for (auto &place : parent->getPlaceList()) {
            int inv = place.place->getInvariant().getBound();
            if (place.maxTokenAge() == inv) {
                return nullptr;
            }
        }
        auto *m = new NonStrictMarkingBase(*parent);
        m->incrementAge();
        _last_fired = nullptr;
        return m;
    }

    bool Generator::modes_match(const TAPN::TimedTransition *trans) {
        switch (mode) {
            case Mode::CONTROLLABLE:
                if (!trans->isControllable()) return false;
                break;
            case Mode::ENVIRONMENT:
                if (trans->isControllable()) return false;
                break;
            case Mode::ALL:
                break;
        }
        return true;
    }

    NonStrictMarkingBase *Generator::next_no_input() {
        // lowhanging fruits first!
        NonStrictMarkingBase *child = new NonStrictMarking(*parent);
        auto trans = allways_enabled[num_children];
        if (inhibited(trans) || (_trans != nullptr && _trans != trans)) return nullptr;
        seen_urgent |= trans->isUrgent();
        if (!modes_match(trans)) return nullptr;

        auto &postset = trans->getPostset();
        // could be optimized
        for (auto arc : postset) {
            Token t = Token(0, arc->getWeight());
            child->addTokenInPlace(arc->getOutputPlace(), t);
        }
        _last_fired = trans;
        ++transitionStatistics[trans->getIndex()];
        return child;
    }

    NonStrictMarkingBase *Generator::next_from_current() {
        if (current == nullptr) return nullptr;
        auto *child = new NonStrictMarkingBase(*parent);
        child->setGeneratedBy(nullptr);
        child->setParent(nullptr);
        int arccounter = 0;
        int last_movable = -1;
        PlaceList &placelist = child->getPlaceList();
        auto pit = placelist.begin();
        for (auto &input : current->getPreset()) {
            int source = input->getInputPlace().getIndex();

            while (pit->place->getIndex() != source) {
                ++pit;
                assert(pit != placelist.end());
            }

            auto &tokenlist = pit->tokens;

            for (int i = input->getWeight() - 1; i >= 0; --i) {
                size_t t_index = permutation[arccounter + i];
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
        for (auto &transport : current->getTransportArcs()) {
            int source = transport->getSource().getIndex();
            while (pit->place->getIndex() != source) {
                ++pit;
                assert(pit != placelist.end());
            }
            for (int i = transport->getWeight() - 1; i >= 0; --i) {
                auto &tokenlist = pit->tokens;
                size_t t_index = permutation[arccounter + i];

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
        for (auto* output : current->getPostset()) {
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
        _last_fired = current;
        ++transitionStatistics[current->getIndex()];

        // nobody can move
        if (last_movable == -1) current = nullptr;
        else {
            permutation[last_movable] += 1;
            for (int i = last_movable + 1; i < arccounter; ++i) {
                permutation[i] = base_permutation[i];
            }
        }
        return child;
    }

    bool Generator::next_transition(bool isfirst) {
        if (parent->getPlaceList().empty()) return false;
        do {
            if (!isfirst)
                ++transition;
            isfirst = false;

            size_t placeindex = parent->getPlaceList()[place].place->getIndex();
            if (place_transition.size() <= placeindex ||
                transition >= place_transition[placeindex].size()) {
                ++place;
                transition = 0;
            }

            // no more places with tokens, we are done
            if (place >= parent->getPlaceList().size()) {
                return false;
            }
            placeindex = parent->getPlaceList()[place].place->getIndex();
            // if no transitions out, skip
            if (place_transition.size() <= placeindex ||
                place_transition[placeindex].empty())
                continue;

            current = place_transition[placeindex][transition];
            if (is_enabled(current, &base_permutation)) {
                permutation = base_permutation;
                return true;
            }
        } while (true);
    }

    const TAPN::InhibitorArc *Generator::inhibited(const TAPN::TimedTransition *trans) const {
        PlaceList &placelist = parent->getPlaceList();
        auto pit = placelist.begin();

        for (auto* inhib : trans->getInhibitorArcs()) {
            while (pit != placelist.end() &&
                   inhib->getInputPlace().getIndex() > pit->place->getIndex())
                ++pit;

            int tokens = inhib->getWeight();
            if (pit != placelist.end() &&
                inhib->getInputPlace().getIndex() == pit->place->getIndex()) {
                for (auto &t : pit->tokens)
                    tokens -= t.getCount();
            }
            if (tokens <= 0) return inhib;
        }
        return nullptr;
    }

    const TAPN::TimedPlace *
    Generator::compute_missing(const TAPN::TimedTransition *trans, std::vector<size_t> *permutations) {
        PlaceList &placelist = parent->getPlaceList();

        size_t arccounter = 0;
        auto pit = placelist.begin();
        for (auto &input : trans->getPreset()) {
            int source = input->getInputPlace().getIndex();
            while (pit != placelist.end() &&
                   pit->place->getIndex() < source)
                ++pit;

            if (pit == placelist.end() || pit->place->getIndex() != source)
                return &input->getInputPlace();

            int weight = input->getWeight();
            auto &tokenlist = pit->tokens;
            for (size_t index = 0; index < tokenlist.size(); ++index) {
                auto &token = tokenlist[index];
                if (input->getInterval().contains(token.getAge())) {
                    if (permutations) {
                        int n_tokens = std::min(weight, token.getCount());
                        for (int i = 0; i < n_tokens; ++i) {
                            (*permutations)[arccounter] = index;
                            ++arccounter;
                        }
                    }
                    weight -= token.getCount();
                    if (weight < 0) break;
                }
            }
            if (weight > 0) return &input->getInputPlace();
        }

        pit = placelist.begin();
        for (auto* transport : trans->getTransportArcs()) {
            int source = transport->getSource().getIndex();
            while (pit != placelist.end() &&
                   pit->place->getIndex() < source)
                ++pit;

            if (pit == placelist.end() || pit->place->getIndex() != source)
                return &transport->getSource();

            int weight = transport->getWeight();
            auto &tokenlist = pit->tokens;
            for (size_t index = 0; index < tokenlist.size(); ++index) {
                auto &token = tokenlist[index];
                if (transport->getInterval().contains(token.getAge())) {
                    if (permutations) {
                        int n_tokens = std::min(weight, token.getCount());
                        for (int i = 0; i < n_tokens; ++i) {
                            (*permutations)[arccounter] = index;
                            ++arccounter;
                        }
                    }
                    weight -= token.getCount();
                    if (weight < 0) break;
                }
            }
            if (weight > 0) return &transport->getSource();
        }
        return nullptr;
    }

    bool Generator::is_enabled(const TAPN::TimedTransition *trans, std::vector<size_t> *permutations) {

        // Check inhibitors
        if (!modes_match(trans)) return false;


        if (inhibited(trans)) return false;

        auto missing_tokens = compute_missing(trans, permutations);
        if (missing_tokens) return false;
        seen_urgent |= trans->isUrgent();
        return true;
    }

    size_t Generator::children() {
        return num_children;
    }

    void Generator::printTransitionStatistics(std::ostream &out) const {
        out << std::endl << "TRANSITION STATISTICS";
        for (unsigned int i = 0; i < transitionStatistics.size(); i++) {
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
} }