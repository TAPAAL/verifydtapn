#ifndef INTERACTIVEMODE_HPP
#define INTERACTIVEMODE_HPP

#include "Core/TAPN/TAPN.hpp"
#include "Core/TraceMapper.hpp"
#include "Core/VerificationOptions.hpp"
#include "DiscreteVerification/DataStructures/RealMarking.hpp"
#include "DiscreteVerification/Util/ClockValue.hpp"
#include "DiscreteVerification/Util/IntervalOps.hpp"

#include <Colored/ColoredPetriNetBuilder.h>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <rapidxml.hpp>
#include <string>
#include <utility>
#include <vector>

namespace VerifyTAPN::DiscreteVerification {
    class InteractiveMode {
    public:
        [[nodiscard]] static int run(TAPN::TimedArcPetriNet& tapn, const std::vector<TokenList>& initialTokens,
            const unfoldtacpn::ColoredPetriNetBuilder& builder, const TraceMapper& mapper,
            const VerificationOptions& options);

    private:
        using Clock = Util::clockValue;
        using DelayInterval = Util::interval<Clock>;

        InteractiveMode(TAPN::TimedArcPetriNet& tapn, const std::vector<TokenList>& initialTokens,
            const unfoldtacpn::ColoredPetriNetBuilder& builder, const TraceMapper& mapper,
            const VerificationOptions& options);

        [[nodiscard]] int runInternal();

        [[nodiscard]] static std::string readUntilDoubleNewline(std::istream& in);
        [[nodiscard]] Clock parseTime(const std::string& value) const;
        [[nodiscard]] std::string formatTime(Clock value) const;
        [[nodiscard]] Clock bound(int value) const;
        [[nodiscard]] DelayInterval ageInterval(const TAPN::TimeInterval& interval) const;
        [[nodiscard]] Clock invariantUpper(const TAPN::TimeInvariant& invariant) const;
        [[nodiscard]] bool validMarking(const RealMarking& marking) const;

        [[nodiscard]] std::unique_ptr<RealMarking> parseMarking(
            const rapidxml::xml_document<>& markingXml, std::ostream& errorOut) const;
        [[nodiscard]] const TAPN::TimedTransition* parseTransition(
            const rapidxml::xml_document<>& transitionXml, std::ostream& errorOut) const;
        void printCurrentMarking(std::ostream& out, const RealMarking& currentMarking) const;
        void printValidBindings(std::ostream& out, const RealMarking& currentMarking) const;
        void fireTransition(const TAPN::TimedTransition* trans);

        [[nodiscard]] std::vector<DelayInterval> getTransitionDelayRanges(
            const TAPN::TimedTransition* trans, const RealMarking& marking, Clock maxAllowedDelay) const;
        [[nodiscard]] static std::vector<DelayInterval> computeArcDelayWindows(
            const TAPN::TimedPlace& place, uint32_t weight, DelayInterval ages, const RealMarking& marking);
        [[nodiscard]] Clock getMaxPossibleDelay(const RealMarking& marking) const;
        static std::vector<RealToken> consumeTokens(
            RealMarking& marking, const TAPN::TimedPlace& place, uint32_t weight, DelayInterval ages);

        [[nodiscard]] static std::vector<std::pair<std::string, int>> parseStructureExpressions(
            const rapidxml::xml_node<>* placeNode);
        static void parseColorSubterms(
            const rapidxml::xml_node<>* node, int count, std::vector<std::pair<std::string, int>>& out);

        TAPN::TimedArcPetriNet& tapn_;
        const unfoldtacpn::ColoredPetriNetBuilder& builder_;
        const TraceMapper& mapper_;
        uint32_t precision_;
        Clock scale_ = 1;
        std::unique_ptr<RealMarking> currentMarking_;
    };
}

#endif // INTERACTIVEMODE_HPP
