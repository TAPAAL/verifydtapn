#include "DiscreteVerification/InteractiveMode.hpp"
#include "DiscreteVerification/Generators/Generator.h"
#include "DiscreteVerification/Generators/NextEnabledGenerator.h"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <set>
#include <limits>
#include <string_view>

namespace VerifyTAPN::DiscreteVerification {
    int InteractiveMode::run(
        TAPN::TimedArcPetriNet& tapn,
        const std::vector<TokenList>& initialTokens,
        const unfoldtacpn::ColoredPetriNetBuilder& builder,
        const TraceMapper& mapper,
        const VerificationOptions& options
    ) {
        InteractiveMode self(tapn, initialTokens, builder, mapper, options);
        return self._run_internal();
    }

    InteractiveMode::InteractiveMode(
        TAPN::TimedArcPetriNet& tapn,
        const std::vector<TokenList>& initialTokens,
        const unfoldtacpn::ColoredPetriNetBuilder& builder,
        const TraceMapper& mapper,
        const VerificationOptions& options
    ) : _tapn(tapn), _builder(builder), _mapper(mapper), _options(options),
        _currentMarking(std::make_unique<NonStrictMarking>(tapn, initialTokens)) {
    }

    int InteractiveMode::_run_internal() {
        auto initialMarkingInput = _readUntilDoubleNewline(std::cin);
        if (initialMarkingInput.empty()) {
            return 0;
        }

        rapidxml::xml_document<> initialMarkingXml;
        try {
            initialMarkingXml.parse<0>(initialMarkingInput.data());
            auto newMarking = _parseMarking(initialMarkingXml, std::cerr);
            if (newMarking) {
                _currentMarking = std::move(newMarking);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing initial marking XML: " << e.what() << std::endl;
            return 1;
        }

        _printValidBindings(std::cout, *_currentMarking);
        std::cout << "\n\n" << std::flush;

        while (true) {
            auto input = _readUntilDoubleNewline(std::cin);
            if (input.empty()) {
                return 0;
            }

            size_t start = input.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) continue;
            size_t end = input.find_last_not_of(" \t\r\n");
            std::string trimmed = input.substr(start, end - start + 1);

            if (trimmed == "EXIT") {
                return 0;
            }

            rapidxml::xml_document<> inputXml;
            try {
                inputXml.parse<0>(input.data());
            } catch (const std::exception& e) {
                std::cerr << "Error parsing input XML: " << e.what() << std::endl;
                continue;
            }

            auto* root = inputXml.first_node();
            if (!root) {
                std::cerr << "No XML element found" << std::endl;
                continue;
            }

            std::string rootName = root->name();
            if (rootName == "marking") {
                auto newMarking = _parseMarking(inputXml, std::cerr);
                if (newMarking) {
                    _currentMarking = std::move(newMarking);
                }
            } else if (rootName == "transition") {
                auto selected = _parseTransition(inputXml, std::cerr);
                if (selected.has_value()) {
                    _fireTransition(selected->first);
                    _printCurrentMarking(std::cout, *_currentMarking);
                    std::cout << "\n\n";
                }
            } else if (rootName == "delay") {
                int amount = 1;
                if (auto* valAttr = root->first_attribute("value")) {
                    amount = std::stoi(valAttr->value());
                } else if (root->value() && strlen(root->value()) > 0) {
                    amount = std::stoi(root->value());
                }

                bool delayPossible = true;
                // Check urgent transitions
                NextEnabledGenerator neg(_tapn);
                neg.prepare(_currentMarking.get());
                for (const auto* t : _tapn.getTransitions()) {
                    if (t->isUrgent() && neg.is_enabled(t)) {
                        delayPossible = false;
                        break;
                    }
                }
                // Check place invariants
                if (delayPossible) {
                    for (const auto& placeObj : _currentMarking->getPlaceList()) {
                        int inv = placeObj.place->getInvariant().getBound();
                        if (placeObj.maxTokenAge() + amount > inv) {
                            delayPossible = false;
                            break;
                        }
                    }
                }

                if (delayPossible) {
                    _currentMarking->incrementAge(amount);
                }
                _printCurrentMarking(std::cout, *_currentMarking);
                std::cout << "\n\n";
            }

            _printValidBindings(std::cout, *_currentMarking);
            std::cout << "\n\n" << std::flush;
        }

        return 0;
    }

    std::string InteractiveMode::_readUntilDoubleNewline(std::istream& in) {
        in >> std::noskipws;
        std::stringstream rv;
        std::string line;
        bool lastIsNewline = false;
        while (std::getline(in, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty()) {
                if (lastIsNewline) {
                    return rv.str();
                }
                lastIsNewline = true;
            } else {
                lastIsNewline = false;
            }
            rv << line << "\n";
        }
        return rv.str();
    }

    void InteractiveMode::_printCurrentMarking(std::ostream& out, const NonStrictMarkingBase& currentMarking) const {
        out << "<marking>\n";

        // Group tokens by original place name
        std::map<std::string, std::vector<std::tuple<std::string, int, int>>> placeTokens;

        for (const auto& placeObj : currentMarking.getPlaceList()) {
            const TAPN::TimedPlace* place = placeObj.place;
            if (placeObj.numberOfTokens() <= 0) continue;

            const std::string& unfoldedName = place->getName();
            auto origPlace = _mapper.mapPlace(unfoldedName);
            if (!origPlace) continue;

            std::string_view colorName = _mapper.getColor(unfoldedName);

            for (const auto& token : placeObj.tokens) {
                placeTokens[std::string(*origPlace)].emplace_back(std::string(colorName), token.getAge(), token.getCount());
            }
        }

        for (const auto& [origPlace, tokens] : placeTokens) {
            out << "\t<place id=\"" << origPlace << "\">\n";
            for (const auto& [colorName, age, count] : tokens) {
                out << "\t\t<token age=\"" << age << "\" count=\"" << count << "\" color=\"" << colorName << "\"/>\n";
            }
            out << "\t</place>\n";
        }

        out << "</marking>";
    }

    void InteractiveMode::_fireTransition(const TAPN::TimedTransition* trans) {
        NextEnabledGenerator neg(_tapn);
        neg.prepare(_currentMarking.get());

        if (!neg.is_enabled(trans)) {
            int maxDelay = _getMaxPossibleDelay(*_currentMarking, neg);
            int minDelay = 0;
            if (!_isDelayEnabled(trans, *_currentMarking, maxDelay, minDelay, neg)) {
                std::cerr << "Transition " << trans->getName() << " is not enabled" << std::endl;
                return;
            }
            _currentMarking->incrementAge(minDelay);
            neg.prepare(_currentMarking.get());
        }

        Generator generator(_tapn, nullptr);
        generator.prepare(_currentMarking.get());
        if (generator.only_transition(trans)) {
            if (NonStrictMarkingBase* next = generator.next(false)) {
                _currentMarking.reset(next);
            }
        }
    }

    std::optional<InteractiveMode::DelayInterval> InteractiveMode::_computeArcDelayWindow(
        const TAPN::TimedPlace& place,
        int weight,
        const TAPN::TimeInterval& interval,
        const NonStrictMarkingBase& marking
    ) const {
        const auto& placeList = marking.getPlaceList();

        auto pit = placeList.begin();
        while (pit != placeList.end() && pit->place->getIndex() < place.getIndex()) {
            ++pit;
        }

        if (pit == placeList.end() || pit->place->getIndex() != place.getIndex()) {
            return std::nullopt;
        }

        int totalTokens = pit->numberOfTokens();
        if (totalTokens < weight) {
            return std::nullopt;
        }

        // Build token ages in descending order
        std::vector<int> tokenAges;
        tokenAges.reserve(totalTokens);
        for (auto it = pit->tokens.rbegin(); it != pit->tokens.rend(); ++it) {
            for (int c = 0; c < it->getCount(); ++c) {
                tokenAges.push_back(it->getAge());
            }
        }

        int arc_low  = std::numeric_limits<int>::max();
        int arc_high = std::numeric_limits<int>::min();
        bool found = false;

        for (size_t k = 0; k + weight <= tokenAges.size(); ++k) {
            int oldestAge   = tokenAges[k];
            int youngestAge = tokenAges[k + weight - 1];

            int low_k = interval.isLowerBoundStrict()
                ? (interval.getLowerBound() + 1 - youngestAge)
                : (interval.getLowerBound()     - youngestAge);
            low_k = std::max(0, low_k);

            int high_k;
            if (interval.getUpperBound() == std::numeric_limits<int>::max() ||
                interval.getUpperBound() == std::numeric_limits<unsigned int>::max()) {
                high_k = std::numeric_limits<int>::max();
            } else if (interval.isUpperBoundStrict()) {
                high_k = interval.getUpperBound() - 1 - oldestAge;
            } else {
                high_k = interval.getUpperBound() - oldestAge;
            }

            if (low_k <= high_k) {
                found    = true;
                arc_low  = std::min(arc_low,  low_k);
                arc_high = std::max(arc_high, high_k);
            }
        }

        if (!found) {
            return std::nullopt;
        }
        
        return DelayInterval{arc_low, arc_high};
    }

    std::optional<InteractiveMode::DelayInterval> InteractiveMode::_getTransitionDelayRange(
        const TAPN::TimedTransition* trans,
        const NonStrictMarkingBase& marking,
        int maxAllowedDelay,
        const NextEnabledGenerator& generator
    ) const {
        if (generator.is_inhibited(trans)) {
            return std::nullopt;
        }

        int trans_low  = 0;
        int trans_high = maxAllowedDelay;

        auto applyWindow = [&](const TAPN::TimedPlace& place, int weight, const TAPN::TimeInterval& interval) -> bool {
            auto r = _computeArcDelayWindow(place, weight, interval, marking);
            if (!r) return false;
            trans_low  = std::max(trans_low,  r->low);
            trans_high = std::min(trans_high, r->high);
            return trans_low <= trans_high;
        };

        for (const auto* arc : trans->getPreset()) {
            if (!applyWindow(arc->getInputPlace(), arc->getWeight(), arc->getInterval())) {
                return std::nullopt;
            }
        }

        for (const auto* transport : trans->getTransportArcs()) {
            if (!applyWindow(transport->getSource(), transport->getWeight(), transport->getInterval())) {
                return std::nullopt;
            }
        }

        if (trans_low > trans_high) {
            return std::nullopt;
        }

        return DelayInterval{trans_low, trans_high};
    }

    int InteractiveMode::_getMaxPossibleDelay(const NonStrictMarkingBase& marking, const NextEnabledGenerator& generator) const {
        // If any urgent transition is enabled at delay 0, delay > 0 is blocked
        for (const auto* t : _tapn.getTransitions()) {
            if (t->isUrgent() && generator.is_enabled(t)) {
                return 0;
            }
        }

        // Check place invariants for all places holding tokens
        int maxDelay = std::numeric_limits<int>::max();
        for (const auto& placeObj : marking.getPlaceList()) {
            if (placeObj.numberOfTokens() == 0) continue;
            int maxAge = placeObj.maxTokenAge();
            const auto& inv = placeObj.place->getInvariant();
            if (inv.getBound() != std::numeric_limits<int>::max() &&
                inv.getBound() != std::numeric_limits<unsigned int>::max()) {
                int placeMax;
                if (inv.isBoundStrict()) {
                    placeMax = inv.getBound() - 1 - maxAge;
                } else {
                    placeMax = inv.getBound() - maxAge;
                }
                if (placeMax < maxDelay) {
                    maxDelay = placeMax;
                }
            }
        }

        maxDelay = std::max(0, maxDelay);

        // Check if any urgent transition becomes enabled at some delay
        for (const auto* t : _tapn.getTransitions()) {
            if (t->isUrgent()) {
                auto range = _getTransitionDelayRange(t, marking, maxDelay, generator);
                if (range.has_value() && range->low <= maxDelay) {
                    maxDelay = std::min(maxDelay, range->low);
                }
            }
        }

        return maxDelay;
    }

    bool InteractiveMode::_isDelayEnabled(
        const TAPN::TimedTransition* trans,
        const NonStrictMarkingBase& marking,
        int maxPossibleDelay,
        int& outMinDelay,
        const NextEnabledGenerator& generator
    ) const {
        if (maxPossibleDelay < 1) return false;

        auto range = _getTransitionDelayRange(trans, marking, maxPossibleDelay, generator);
        if (!range.has_value()) return false;

        int eff_low = std::max(1, range->low);
        int eff_high = std::min(maxPossibleDelay, range->high);

        if (eff_low <= eff_high) {
            outMinDelay = eff_low;
            return true;
        }

        return false;
    }

    void InteractiveMode::_printValidBindings(std::ostream& out, const NonStrictMarkingBase& currentMarking) const {
        NextEnabledGenerator generator(_tapn);
        generator.prepare(&currentMarking);

        int maxDelay = _getMaxPossibleDelay(currentMarking, generator);

        std::map<std::string, std::vector<TraceMapper::BindingList>> enabledByOrigTrans;
        std::map<std::string, std::vector<TraceMapper::BindingList>> delayEnabledByOrigTrans;
        std::map<std::string, int> minDelayByOrigTrans;

        for (const auto* trans : _tapn.getTransitions()) {
            std::string_view origTrans = _mapper.mapTransition(trans->getName());
            const auto& bindings = _mapper.getBindings(trans->getName());

            if (generator.is_enabled(trans)) {
                enabledByOrigTrans[std::string(origTrans)].push_back(bindings);
            } else {
                int minDelay = 0;
                if (maxDelay >= 1 && _isDelayEnabled(trans, currentMarking, maxDelay, minDelay, generator)) {
                    std::string key(origTrans);
                    delayEnabledByOrigTrans[key].push_back(bindings);
                    auto it = minDelayByOrigTrans.find(key);
                    if (it == minDelayByOrigTrans.end() || minDelay < it->second) {
                        minDelayByOrigTrans[key] = minDelay;
                    }
                }
            }
        }

        auto printTransitionBindings = [&out](const auto& bindingLists) {
            for (const auto& bindingList : bindingLists) {
                out << "\t\t<binding>\n";
                for (const auto& [varId, colorVal] : bindingList) {
                    out << "\t\t\t<variable id=\"" << varId << "\">\n";
                    out << "\t\t\t\t<color>" << colorVal << "</color>\n";
                    out << "\t\t\t</variable>\n";
                }
                out << "\t\t</binding>\n";
            }
        };

        out << "<valid-bindings>\n";
        for (const auto& [origTrans, bindingLists] : enabledByOrigTrans) {
            out << "\t<transition id=\"" << origTrans << "\">\n";
            printTransitionBindings(bindingLists);
            out << "\t</transition>\n";
        }
        out << "</valid-bindings>\n";

        out << "<delay-enabled-bindings>\n";
        for (const auto& [origTrans, bindingLists] : delayEnabledByOrigTrans) {
            int md = minDelayByOrigTrans[origTrans];
            out << "\t<transition id=\"" << origTrans << "\" delay-enabled=\"true\" min-delay=\"" << md << "\">\n";
            printTransitionBindings(bindingLists);
            out << "\t</transition>\n";
        }
        out << "</delay-enabled-bindings>";
    }

    void InteractiveMode::_parseColorSubterms(rapidxml::xml_node<>* node, int count, std::vector<std::pair<std::string, int>>& out) const {
        if (!node) return;
        std::string name = node->name();
        if (name == "structure" || name == "subterm") {
            for (auto* child = node->first_node(); child; child = child->next_sibling()) {
                _parseColorSubterms(child, count, out);
            }
        } else if (name == "add") {
            for (auto* child = node->first_node(); child; child = child->next_sibling()) {
                _parseColorSubterms(child, count, out);
            }
        } else if (name == "numberof") {
            int currentCount = 1;
            rapidxml::xml_node<>* colorSubterm = nullptr;
            for (auto* sub = node->first_node("subterm"); sub; sub = sub->next_sibling("subterm")) {
                if (auto* numConst = sub->first_node("numberconstant")) {
                    if (auto* valAttr = numConst->first_attribute("value")) {
                        currentCount = std::stoi(valAttr->value());
                    }
                } else {
                    colorSubterm = sub;
                }
            }
            if (colorSubterm) {
                _parseColorSubterms(colorSubterm, currentCount, out);
            }
        } else if (name == "useroperator") {
            if (auto* decl = node->first_attribute("declaration")) {
                out.emplace_back(decl->value(), count);
            }
        } else if (name == "dotconstant") {
            out.emplace_back("dot", count);
        } else if (name == "finiteintrangeconstant") {
            if (auto* val = node->first_attribute("value")) {
                out.emplace_back(val->value(), count);
            }
        } else if (name == "tuple") {
            std::string tupleStr = "(";
            bool first = true;
            for (auto* sub = node->first_node("subterm"); sub; sub = sub->next_sibling("subterm")) {
                auto* child = sub->first_node();
                if (!child) continue;
                if (!first) tupleStr += ",";
                first = false;
                if (std::string(child->name()) == "dotconstant") {
                    tupleStr += "dot";
                } else if (auto* decl = child->first_attribute("declaration")) {
                    tupleStr += decl->value();
                } else if (auto* val = child->first_attribute("value")) {
                    tupleStr += val->value();
                }
            }
            tupleStr += ")";
            out.emplace_back(tupleStr, count);
        }
    }

    std::vector<std::pair<std::string, int>> InteractiveMode::_parseStructureExpressions(rapidxml::xml_node<>* placeNode) const {
        std::vector<std::pair<std::string, int>> result;
        for (auto* child = placeNode->first_node(); child; child = child->next_sibling()) {
            _parseColorSubterms(child, 1, result);
        }
        return result;
    }

    std::unique_ptr<NonStrictMarkingBase> InteractiveMode::_parseMarking(
        const rapidxml::xml_document<>& markingXml,
        std::ostream& errorOut
    ) {
        auto* root = markingXml.first_node("marking");
        if (!root) {
            errorOut << "Expected <marking> root node" << std::endl;
            return nullptr;
        }

        std::vector<TokenList> placeTokens(_tapn.getPlaces().size());

        auto addToken = [&](int idx, int age, int count) {
            if (idx != TAPN::TimedPlace::BottomIndex() && idx >= 0 && (size_t)idx < placeTokens.size()) {
                bool found = false;
                for (auto& tok : placeTokens[idx]) {
                    if (tok.getAge() == age) {
                        tok.setCount(tok.getCount() + count);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    placeTokens[idx].emplace_back(age, count);
                }
            }
        };

        for (auto* placeNode = root->first_node("place"); placeNode; placeNode = placeNode->next_sibling("place")) {
            auto* idAttr = placeNode->first_attribute("id");
            if (!idAttr) continue;
            std::string origPlace = idAttr->value();

            auto resolveAndAdd = [&](const std::string& colorName, int age, int count) {
                std::string unfoldedName;
                int idx = _builder.resolvePlace(origPlace, colorName, unfoldedName)
                    ? _tapn.getPlaceIndex(unfoldedName)
                    : _tapn.getPlaceIndex(origPlace);
                addToken(idx, age, count);
            };

            bool hasTokenElements = false;
            for (auto* tokenNode = placeNode->first_node("token"); tokenNode; tokenNode = tokenNode->next_sibling("token")) {
                hasTokenElements = true;
                int age = 0;
                int count = 1;
                if (auto* ageAttr   = tokenNode->first_attribute("age"))   age   = std::stoi(ageAttr->value());
                if (auto* countAttr = tokenNode->first_attribute("count")) count = std::stoi(countAttr->value());
                std::string colorName = "dot";
                if (auto* colorAttr = tokenNode->first_attribute("color")) colorName = colorAttr->value();
                resolveAndAdd(colorName, age, count);
            }

            if (hasTokenElements) continue;

            for (const auto& [colorName, count] : _parseStructureExpressions(placeNode)) {
                resolveAndAdd(colorName, 0, count);
            }
        }

        for (auto& list : placeTokens) {
            std::sort(list.begin(), list.end(), [](const Token& a, const Token& b) {
                return a.getAge() < b.getAge();
            });
        }

        return std::make_unique<NonStrictMarking>(_tapn, placeTokens);
    }

    std::optional<std::pair<const TAPN::TimedTransition*, TraceMapper::BindingList>>
    InteractiveMode::_parseTransition(const rapidxml::xml_document<>& transitionXml, std::ostream& errorOut) const {
        const auto* transitionNode = transitionXml.first_node();
        if (!transitionNode || std::string(transitionNode->name()) != "transition") {
            errorOut << "Expected <transition> tag" << std::endl;
            return std::nullopt;
        }
        const auto* idAttr = transitionNode->first_attribute("id");
        if (!idAttr) {
            errorOut << "Missing id attribute on <transition>" << std::endl;
            return std::nullopt;
        }
        std::string origTransId = idAttr->value();

        TraceMapper::BindingList parsedBindings;
        if (const auto* bindingNode = transitionNode->first_node("binding")) {
            for (const auto* varNode = bindingNode->first_node("variable"); varNode; varNode = varNode->next_sibling("variable")) {
                const auto* varIdAttr = varNode->first_attribute("id");
                const auto* colorNode = varNode->first_node("color");
                if (varIdAttr && colorNode && colorNode->value()) {
                    parsedBindings.emplace_back(varIdAttr->value(), colorNode->value());
                }
            }
        }
        std::sort(parsedBindings.begin(), parsedBindings.end());

        const TAPN::TimedTransition* matchedTransition = nullptr;
        for (const auto* t : _tapn.getTransitions()) {
            if (_mapper.mapTransition(t->getName()) == origTransId) {
                if (_mapper.getBindings(t->getName()) == parsedBindings) {
                    matchedTransition = t;
                    break;
                }
            }
        }

        if (!matchedTransition) {
            errorOut << "Could not find transition \"" << origTransId << "\" with given bindings" << std::endl;
            return std::nullopt;
        }

        return std::make_pair(matchedTransition, parsedBindings);
    }
}
