#include "DiscreteVerification/InteractiveMode.hpp"
#include "Core/TAPN/TimeInterval.hpp"
#include "Core/TAPN/TimedArcPetriNet.hpp"
#include "Core/TAPN/TimedTransition.hpp"
#include "Core/TraceMapper.hpp"
#include "Core/VerificationOptions.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarkingBase.hpp"
#include "DiscreteVerification/DataStructures/RealMarking.hpp"
#include "DiscreteVerification/Util/IntervalOps.hpp"

#include <Colored/ColoredPetriNetBuilder.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <rapidxml.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace VerifyTAPN::DiscreteVerification {
    int InteractiveMode::run(TAPN::TimedArcPetriNet& tapn, const std::vector<TokenList>& initialTokens,
        const unfoldtacpn::ColoredPetriNetBuilder& builder, const TraceMapper& mapper,
        const VerificationOptions& options) {
        try {
            InteractiveMode self(tapn, initialTokens, builder, mapper, options);
            return self.runInternal();
        } catch (const std::exception& e) {
            std::cerr << "Interactive simulation failed: " << e.what() << '\n';
            return 1;
        }
    }

    InteractiveMode::InteractiveMode(TAPN::TimedArcPetriNet& tapn, const std::vector<TokenList>& initialTokens,
        const unfoldtacpn::ColoredPetriNetBuilder& builder, const TraceMapper& mapper,
        const VerificationOptions& options)
        : tapn_(tapn), builder_(builder), mapper_(mapper),
          precision_(options.getSMCNumericPrecision() == 0 ? 10 : options.getSMCNumericPrecision()) {
        for (uint32_t i = 0; i < precision_; ++i) {
            if (scale_ > (std::numeric_limits<Clock>::max() - 1) / 10) {
                throw std::out_of_range("Interactive time precision exceeds the clock range");
            }

            scale_ *= 10;
        }

        for (const auto* place : tapn_.getPlaces()) {
            static_cast<void>(bound(place->getInvariant().getBound()));
        }

        for (const auto* arc : tapn_.getInputArcs()) {
            static_cast<void>(ageInterval(arc->getInterval()));
        }

        for (const auto* arc : tapn_.getTransportArcs()) {
            static_cast<void>(ageInterval(arc->getInterval()));
        }

        NonStrictMarkingBase initial(tapn, initialTokens);
        currentMarking_ = std::make_unique<RealMarking>(&tapn, initial, precision_);
        currentMarking_->setDeadlocked(false);
    }

    InteractiveMode::Clock InteractiveMode::parseTime(const std::string& value) const {
        const Clock largest = std::numeric_limits<Clock>::max() - 1;
        Clock whole = 0;
        Clock fraction = 0;
        uint32_t digits = 0;
        bool decimal = false;
        bool hasDigit = false;
        for (const char c : value) {
            if (c == '.' && !decimal) {
                decimal = true;
                continue;
            }

            if (c < '0' || c > '9') {
                throw std::invalid_argument("Expected a nonnegative decimal time: " + value);
            }

            hasDigit = true;
            const Clock digit = c - '0';
            if (!decimal) {
                if (whole > (largest - digit) / 10) {
                    throw std::out_of_range("Time is too large: " + value);
                }
                whole = whole * 10 + digit;
            } else if (digits < precision_) {
                fraction = fraction * 10 + digit;
                ++digits;
            } else if (digit != 0) {
                throw std::invalid_argument("Time exceeds interactive precision of " + std::to_string(precision_) +
                                            " decimal places: " + value);
            }
        }

        if (!hasDigit) {
            throw std::invalid_argument("Expected a nonnegative decimal time: " + value);
        }

        while (digits++ < precision_) {
            fraction *= 10;
        }

        if (whole > (largest - fraction) / scale_) {
            throw std::out_of_range("Time is too large: " + value);
        }

        return whole * scale_ + fraction;
    }

    std::string InteractiveMode::formatTime(Clock value) const {
        std::string result = std::to_string(value / scale_);
        if (value % scale_ != 0) {
            auto fraction = std::to_string(value % scale_);
            fraction.insert(0, precision_ - fraction.size(), '0');
            while (fraction.back() == '0') {
                fraction.pop_back();
            }

            result += "." + fraction;
        }

        return result;
    }

    InteractiveMode::Clock InteractiveMode::bound(int value) const {
        if (value == std::numeric_limits<int>::max()) {
            return std::numeric_limits<Clock>::max();
        }

        if (value < 0 || static_cast<Clock>(value) > (std::numeric_limits<Clock>::max() - 1) / scale_) {
            throw std::out_of_range("Model bound exceeds the interactive clock range");
        }

        return static_cast<Clock>(value) * scale_;
    }

    InteractiveMode::DelayInterval InteractiveMode::ageInterval(const TAPN::TimeInterval& interval) const {
        Clock low = bound(interval.getLowerBound());
        Clock high = bound(interval.getUpperBound());
        if (low == std::numeric_limits<Clock>::max() || (high == 0 && interval.isUpperBoundStrict())) {
            return {1, 0};
        }

        if (interval.isLowerBoundStrict()) {
            ++low;
        }

        if (high != std::numeric_limits<Clock>::max() && interval.isUpperBoundStrict()) {
            --high;
        }

        return {low, high};
    }

    InteractiveMode::Clock InteractiveMode::invariantUpper(const TAPN::TimeInvariant& invariant) const {
        Clock high = bound(invariant.getBound());
        if (high != 0 && high != std::numeric_limits<Clock>::max() && invariant.isBoundStrict()) {
            --high;
        }

        return high;
    }

    bool InteractiveMode::validMarking(const RealMarking& marking) const {
        const auto& places = marking.getPlaceList();
        return std::all_of(places.begin(), places.end(), [this](const auto& place) {
            if (place.isEmpty()) {
                return true;
            }

            const auto& invariant = place.place->getInvariant();
            return !(invariant.getBound() == 0 && invariant.isBoundStrict()) &&
                   place.maxTokenAge() <= invariantUpper(invariant);
        });
    }

    int InteractiveMode::runInternal() {
        auto initialMarkingInput = readUntilDoubleNewline(std::cin);
        if (initialMarkingInput.empty()) {
            return 0;
        }

        rapidxml::xml_document<> initialMarkingXml;
        try {
            initialMarkingXml.parse<0>(initialMarkingInput.data());
            auto newMarking = parseMarking(initialMarkingXml, std::cerr);
            if (!newMarking) {
                return 1;
            }

            currentMarking_ = std::move(newMarking);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing initial marking XML: " << e.what() << '\n';
            return 1;
        }

        printValidBindings(std::cout, *currentMarking_);
        std::cout << "\n\n" << std::flush;

        while (true) {
            auto input = readUntilDoubleNewline(std::cin);
            if (input.empty()) {
                return 0;
            }
            const size_t start = input.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) {
                continue;
            }

            const size_t end = input.find_last_not_of(" \t\r\n");
            if (input.substr(start, end - start + 1) == "EXIT") {
                return 0;
            }

            try {
                rapidxml::xml_document<> inputXml;
                inputXml.parse<0>(input.data());
                const auto* root = inputXml.first_node();
                if (root == nullptr) {
                    throw std::invalid_argument("No XML element found");
                }
                const std::string rootName = root->name();
                if (rootName == "marking") {
                    auto newMarking = parseMarking(inputXml, std::cerr);
                    if (newMarking) {
                        currentMarking_ = std::move(newMarking);
                    }
                } else if (rootName == "transition") {
                    if (const auto* selected = parseTransition(inputXml, std::cerr)) {
                        fireTransition(selected);
                    }
                } else if (rootName == "delay") {
                    Clock amount = scale_;
                    if (const auto* attr = root->first_attribute("value")) {
                        amount = parseTime(attr->value());
                    } else if ((root->value() != nullptr) && strlen(root->value()) > 0) {
                        amount = parseTime(root->value());
                    }
                    if (amount <= getMaxPossibleDelay(*currentMarking_)) {
                        currentMarking_->deltaAge(amount);
                    } else {
                        std::cerr << "Delay is blocked by an invariant or urgent transition" << '\n';
                    }
                } else {
                    throw std::invalid_argument("Unknown interactive command: " + rootName);
                }
            } catch (const std::exception& e) {
                std::cerr << "Error executing interactive command: " << e.what() << '\n';
            }

            printCurrentMarking(std::cout, *currentMarking_);
            std::cout << "\n\n";
            printValidBindings(std::cout, *currentMarking_);
            std::cout << "\n\n" << std::flush;
        }
    }

    std::string InteractiveMode::readUntilDoubleNewline(std::istream& in) {
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

    void InteractiveMode::printCurrentMarking(std::ostream& out, const RealMarking& currentMarking) const {
        out << "<marking>\n";
        std::map<std::string, std::vector<std::tuple<std::string, Clock, int>>> placeTokens;
        for (const auto& placeObj : currentMarking.getPlaceList()) {
            if (placeObj.isEmpty()) {
                continue;
            }
            const auto& unfoldedName = placeObj.place->getName();
            auto origPlace = mapper_.mapPlace(unfoldedName);
            if (!origPlace) {
                continue;
            }
            auto colorName = mapper_.getColor(unfoldedName);
            for (const auto& token : placeObj.tokens) {
                placeTokens[std::string(*origPlace)].emplace_back(colorName, token.getAge(), token.getCount());
            }
        }
        for (const auto& [origPlace, tokens] : placeTokens) {
            out << "\t<place id=\"" << origPlace << "\">\n";
            for (const auto& [colorName, age, count] : tokens) {
                out << "\t\t<token age=\"" << formatTime(age) << "\" count=\"" << count << "\" color=\"" << colorName
                    << "\"/>\n";
            }
            out << "\t</place>\n";
        }
        out << "</marking>";
    }

    std::vector<InteractiveMode::DelayInterval> InteractiveMode::computeArcDelayWindows(
        const TAPN::TimedPlace& place, uint32_t weight, DelayInterval ages, const RealMarking& marking) {
        if (ages.empty()) {
            return {};
        }
        if (weight <= 0) {
            return {{0, std::numeric_limits<Clock>::max()}};
        }

        std::map<Clock, int64_t> events;
        for (const auto& token : marking.getTokenList(place.getIndex())) {
            const Clock age = token.getAge();
            if (age > ages.high) {
                continue;
            }
            const Clock low = ages.low > age ? ages.low - age : 0;
            const Clock high = ages.high == std::numeric_limits<Clock>::max() ? ages.high : ages.high - age;
            events[low] += token.getCount();
            if (high != std::numeric_limits<Clock>::max()) {
                events[high + 1] -= token.getCount();
            }
        }

        std::vector<DelayInterval> result;
        int64_t count = 0;
        Clock low = 0;
        for (const auto& [date, delta] : events) {
            const bool wasEnabled = count >= weight;
            count += delta;
            if (!wasEnabled && count >= weight) {
                low = date;
            } else if (wasEnabled && count < weight) {
                result.emplace_back(low, date - 1);
            }
        }
        if (count >= weight) {
            result.emplace_back(low, std::numeric_limits<Clock>::max());
        }
        return result;
    }

    std::vector<InteractiveMode::DelayInterval> InteractiveMode::getTransitionDelayRanges(
        const TAPN::TimedTransition* trans, const RealMarking& marking, Clock maxAllowedDelay) const {
        for (const auto* arc : trans->getInhibitorArcs()) {
            if (marking.numberOfTokensInPlace(arc->getInputPlace().getIndex()) >= arc->getWeight()) {
                return {};
            }
        }
        std::vector<DelayInterval> ranges{{0, maxAllowedDelay}};
        for (const auto* arc : trans->getPreset()) {
            ranges = Util::setIntersection(ranges, computeArcDelayWindows(arc->getInputPlace(), arc->getWeight(),
                                                       ageInterval(arc->getInterval()), marking));
            if (ranges.empty()) {
                return ranges;
            }
        }
        for (const auto* arc : trans->getTransportArcs()) {
            const auto& invariant = arc->getDestination().getInvariant();
            if (invariant.getBound() == 0 && invariant.isBoundStrict()) {
                return {};
            }
            auto ages = Util::intersect(ageInterval(arc->getInterval()), DelayInterval(0, invariantUpper(invariant)));
            ranges = Util::setIntersection(
                ranges, computeArcDelayWindows(arc->getSource(), arc->getWeight(), ages, marking));
            if (ranges.empty()) {
                return ranges;
            }
        }
        for (const auto* arc : trans->getPostset()) {
            const auto& invariant = arc->getOutputPlace().getInvariant();
            if (invariant.getBound() == 0 && invariant.isBoundStrict()) {
                return {};
            }
        }

        return ranges;
    }

    InteractiveMode::Clock InteractiveMode::getMaxPossibleDelay(const RealMarking& marking) const {
        Clock maxDelay = std::numeric_limits<Clock>::max() - 1 - marking.getTotalAge();
        for (const auto& place : marking.getPlaceList()) {
            if (place.isEmpty()) {
                continue;
            }
            const Clock age = place.maxTokenAge();
            const Clock upper =
                std::min(invariantUpper(place.place->getInvariant()), std::numeric_limits<Clock>::max() - 1);
            if (upper < age) {
                return 0;
            }
            maxDelay = std::min(maxDelay, upper - age);
        }

        for (const auto* trans : tapn_.getTransitions()) {
            if (!trans->isUrgent()) {
                continue;
            }
            auto ranges = getTransitionDelayRanges(trans, marking, maxDelay);
            if (!ranges.empty()) {
                maxDelay = std::min(maxDelay, ranges.front().low);
            }
        }
        return maxDelay;
    }

    std::vector<RealToken> InteractiveMode::consumeTokens(
        RealMarking& marking, const TAPN::TimedPlace& place, uint32_t weight, DelayInterval ages) {
        std::vector<RealToken> consumed;
        auto& tokens = marking.getTokenList(place.getIndex());
        for (auto it = tokens.begin(); it != tokens.end() && weight > 0;) {
            if (ages.empty() || it->getAge() < ages.low || it->getAge() > ages.high) {
                ++it;
                continue;
            }
            const int count = static_cast<int>(std::min(weight, static_cast<uint32_t>(it->getCount())));
            consumed.emplace_back(it->getAge(), count);
            it->remove(count);
            weight -= count;
            if (it->getCount() == 0) {
                it = tokens.erase(it);
            } else {
                ++it;
            }
        }
        if (weight > 0) {
            throw std::invalid_argument("Insufficient eligible tokens to fire transition");
        }
        return consumed;
    }

    void InteractiveMode::fireTransition(const TAPN::TimedTransition* trans) {
        auto ranges = getTransitionDelayRanges(trans, *currentMarking_, getMaxPossibleDelay(*currentMarking_));
        if (ranges.empty()) {
            std::cerr << "Transition " << trans->getName() << " is not enabled" << '\n';
            return;
        }
        auto next = std::make_unique<RealMarking>(*currentMarking_);
        next->deltaAge(ranges.front().low);
        for (const auto* arc : trans->getPreset()) {
            consumeTokens(*next, arc->getInputPlace(), arc->getWeight(), ageInterval(arc->getInterval()));
        }
        std::vector<std::pair<const TAPN::TimedPlace*, RealToken>> transported;
        for (const auto* arc : trans->getTransportArcs()) {
            auto ages = Util::intersect(ageInterval(arc->getInterval()),
                DelayInterval(0, invariantUpper(arc->getDestination().getInvariant())));
            for (auto token : consumeTokens(*next, arc->getSource(), arc->getWeight(), ages)) {
                transported.emplace_back(&arc->getDestination(), token);
            }
        }

        for (const auto* arc : trans->getPostset()) {
            if (arc->getWeight() > static_cast<uint32_t>(std::numeric_limits<int>::max())) {
                throw std::out_of_range("Output arc weight exceeds the token count range");
            }
            RealToken token(0, static_cast<int>(arc->getWeight()));
            next->addTokenInPlace(arc->getOutputPlace(), token);
        }

        for (auto& [place, token] : transported) {
            next->addTokenInPlace(*place, token);
        }
        if (!validMarking(*next)) {
            throw std::invalid_argument("Firing violates a place invariant");
        }

        currentMarking_ = std::move(next);
    }

    void InteractiveMode::printValidBindings(std::ostream& out, const RealMarking& currentMarking) const {
        const Clock maxDelay = getMaxPossibleDelay(currentMarking);
        using ReportedBinding = std::pair<TraceMapper::BindingList, Clock>;
        std::map<std::string, std::vector<ReportedBinding>> bindingsByOrigTrans;
        for (const auto* trans : tapn_.getTransitions()) {
            auto ranges = getTransitionDelayRanges(trans, currentMarking, maxDelay);
            if (!ranges.empty()) {
                bindingsByOrigTrans[std::string(mapper_.mapTransition(trans->getName()))].emplace_back(
                    mapper_.getBindings(trans->getName()), ranges.front().low);
            }
        }
        out << "<valid-bindings>\n";
        for (const auto& [origTrans, bindingLists] : bindingsByOrigTrans) {
            out << "\t<transition id=\"" << origTrans << "\">\n";
            for (const auto& [binding, minDelay] : bindingLists) {
                out << "\t\t<binding";
                if (minDelay != 0) {
                    out << " min-delay=\"" << formatTime(minDelay) << "\"";
                }
                out << ">\n";
                for (const auto& [varId, colorVal] : binding) {
                    out << "\t\t\t<variable id=\"" << varId << "\">\n";
                    out << "\t\t\t\t<color>" << colorVal << "</color>\n";
                    out << "\t\t\t</variable>\n";
                }
                out << "\t\t</binding>\n";
            }
            out << "\t</transition>\n";
        }
        out << "</valid-bindings>";
    }

    void InteractiveMode::parseColorSubterms(
        const rapidxml::xml_node<>* node, int count, std::vector<std::pair<std::string, int>>& out) {
        if (node == nullptr) {
            return;
        }
        const std::string name = node->name();
        if (name == "structure" || name == "subterm" || name == "add") {
            for (const auto* child = node->first_node(); child != nullptr; child = child->next_sibling()) {
                parseColorSubterms(child, count, out);
            }
        } else if (name == "numberof") {
            int currentCount = 1;
            const rapidxml::xml_node<>* colorSubterm = nullptr;
            for (const auto* sub = node->first_node("subterm"); sub != nullptr; sub = sub->next_sibling("subterm")) {
                if (const auto* numConst = sub->first_node("numberconstant")) {
                    if (const auto* valAttr = numConst->first_attribute("value")) {
                        currentCount = std::stoi(valAttr->value());
                    }
                } else {
                    colorSubterm = sub;
                }
            }
            if (colorSubterm != nullptr) {
                parseColorSubterms(colorSubterm, currentCount, out);
            }
        } else if (name == "useroperator") {
            if (const auto* decl = node->first_attribute("declaration")) {
                out.emplace_back(decl->value(), count);
            }
        } else if (name == "dotconstant") {
            out.emplace_back("dot", count);
        } else if (name == "finiteintrangeconstant") {
            if (const auto* val = node->first_attribute("value")) {
                out.emplace_back(val->value(), count);
            }
        } else if (name == "tuple") {
            std::string tupleStr = "(";
            bool first = true;
            for (const auto* sub = node->first_node("subterm"); sub != nullptr; sub = sub->next_sibling("subterm")) {
                const auto* child = sub->first_node();
                if (child == nullptr) {
                    continue;
                }
                if (!first) {
                    tupleStr += ",";
                }
                first = false;
                if (std::string(child->name()) == "dotconstant") {
                    tupleStr += "dot";
                } else if (const auto* decl = child->first_attribute("declaration")) {
                    tupleStr += decl->value();
                } else if (const auto* val = child->first_attribute("value")) {
                    tupleStr += val->value();
                }
            }
            tupleStr += ")";
            out.emplace_back(tupleStr, count);
        }
    }

    std::vector<std::pair<std::string, int>> InteractiveMode::parseStructureExpressions(
        const rapidxml::xml_node<>* placeNode) {
        std::vector<std::pair<std::string, int>> result;
        for (const auto* child = placeNode->first_node(); child != nullptr; child = child->next_sibling()) {
            parseColorSubterms(child, 1, result);
        }
        return result;
    }

    std::unique_ptr<RealMarking> InteractiveMode::parseMarking(
        const rapidxml::xml_document<>& markingXml, std::ostream& errorOut) const {
        const auto* root = markingXml.first_node("marking");
        if (root == nullptr) {
            errorOut << "Expected <marking> root node" << '\n';
            return nullptr;
        }

        const std::vector<TokenList> emptyTokens(tapn_.getPlaces().size());
        NonStrictMarkingBase empty(tapn_, emptyTokens);
        auto marking = std::make_unique<RealMarking>(&tapn_, empty, precision_);
        marking->setDeadlocked(false);

        auto addToken = [&](int idx, Clock age, int count) {
            if (count <= 0) {
                throw std::invalid_argument("Token count must be positive");
            }
            if (idx != TAPN::TimedPlace::BottomIndex() && idx >= 0 &&
                static_cast<size_t>(idx) < tapn_.getPlaces().size()) {
                RealToken token(age, count);
                marking->addTokenInPlace(tapn_.getPlace(idx), token);
            }
        };

        for (const auto* placeNode = root->first_node("place"); placeNode != nullptr;
            placeNode = placeNode->next_sibling("place")) {
            const auto* idAttr = placeNode->first_attribute("id");
            if (idAttr == nullptr) {
                continue;
            }
            std::string origPlace = idAttr->value();

            auto resolveAndAdd = [&](const std::string& colorName, Clock age, int count) {
                std::string unfoldedName;
                const int idx = builder_.resolvePlace(origPlace, colorName, unfoldedName)
                                    ? tapn_.getPlaceIndex(unfoldedName)
                                    : tapn_.getPlaceIndex(origPlace);
                addToken(idx, age, count);

                const int sumIdx = tapn_.getPlaceIndex("__" + origPlace + "__SUM");
                addToken(sumIdx, age, count);
            };

            bool hasTokenElements = false;
            for (const auto* tokenNode = placeNode->first_node("token"); tokenNode != nullptr;
                tokenNode = tokenNode->next_sibling("token")) {
                hasTokenElements = true;
                Clock age = 0;
                int count = 1;
                if (const auto* ageAttr = tokenNode->first_attribute("age")) {
                    age = parseTime(ageAttr->value());
                }
                if (const auto* countAttr = tokenNode->first_attribute("count")) {
                    count = std::stoi(countAttr->value());
                }
                std::string colorName = "dot";
                if (const auto* colorAttr = tokenNode->first_attribute("color")) {
                    colorName = colorAttr->value();
                }
                resolveAndAdd(colorName, age, count);
            }

            if (hasTokenElements) {
                continue;
            }

            for (const auto& [colorName, count] : parseStructureExpressions(placeNode)) {
                resolveAndAdd(colorName, 0, count);
            }
        }

        if (!validMarking(*marking)) {
            errorOut << "Marking violates a place invariant" << '\n';
            return nullptr;
        }
        return marking;
    }

    const TAPN::TimedTransition* InteractiveMode::parseTransition(
        const rapidxml::xml_document<>& transitionXml, std::ostream& errorOut) const {
        const auto* transitionNode = transitionXml.first_node();
        if ((transitionNode == nullptr) || std::string(transitionNode->name()) != "transition") {
            errorOut << "Expected <transition> tag" << '\n';
            return nullptr;
        }
        const auto* idAttr = transitionNode->first_attribute("id");
        if (idAttr == nullptr) {
            errorOut << "Missing id attribute on <transition>" << '\n';
            return nullptr;
        }
        const std::string origTransId = idAttr->value();

        TraceMapper::BindingList parsedBindings;
        if (const auto* bindingNode = transitionNode->first_node("binding")) {
            for (const auto* varNode = bindingNode->first_node("variable"); varNode != nullptr;
                varNode = varNode->next_sibling("variable")) {
                const auto* varIdAttr = varNode->first_attribute("id");
                const auto* colorNode = varNode->first_node("color");
                if ((varIdAttr != nullptr) && (colorNode != nullptr) && (colorNode->value() != nullptr)) {
                    parsedBindings.emplace_back(varIdAttr->value(), colorNode->value());
                }
            }
        }
        std::sort(parsedBindings.begin(), parsedBindings.end());

        for (const auto* t : tapn_.getTransitions()) {
            if (mapper_.mapTransition(t->getName()) == origTransId &&
                mapper_.getBindings(t->getName()) == parsedBindings) {
                return t;
            }
        }

        errorOut << "Could not find transition \"" << origTransId << "\" with given bindings" << '\n';
        return nullptr;
    }
}
