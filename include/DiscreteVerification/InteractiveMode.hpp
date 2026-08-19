#ifndef INTERACTIVEMODE_HPP
#define INTERACTIVEMODE_HPP

#include "Core/TAPN/TAPN.hpp"
#include "Core/TraceMapper.hpp"
#include "Core/VerificationOptions.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarkingBase.hpp"

#include <Colored/ColoredPetriNetBuilder.h>
#include <rapidxml.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace VerifyTAPN::DiscreteVerification {
    class NextEnabledGenerator;

    class InteractiveMode {
    public:
        struct DelayInterval {
            int low;
            int high;
        };

        static int run(
            TAPN::TimedArcPetriNet& tapn,
            const std::vector<TokenList>& initialTokens,
            const unfoldtacpn::ColoredPetriNetBuilder& builder,
            const TraceMapper& mapper,
            const VerificationOptions& options
        );

    private:
        InteractiveMode(
            TAPN::TimedArcPetriNet& tapn,
            const std::vector<TokenList>& initialTokens,
            const unfoldtacpn::ColoredPetriNetBuilder& builder,
            const TraceMapper& mapper,
            const VerificationOptions& options
        );

        int _run_internal();

        static std::string _readUntilDoubleNewline(std::istream& in);
        std::unique_ptr<NonStrictMarkingBase> _parseMarking(const rapidxml::xml_document<>& markingXml, std::ostream& errorOut);
        std::optional<std::pair<const TAPN::TimedTransition*, TraceMapper::BindingList>> _parseTransition(const rapidxml::xml_document<>& transitionXml, std::ostream& errorOut) const;
        void _printCurrentMarking(std::ostream& out, const NonStrictMarkingBase& currentMarking) const;
        void _printValidBindings(std::ostream& out, const NonStrictMarkingBase& currentMarking) const;

        void _fireTransition(const TAPN::TimedTransition* trans);

        std::optional<DelayInterval> _getTransitionDelayRange(const TAPN::TimedTransition* trans, const NonStrictMarkingBase& marking, int maxAllowedDelay, const NextEnabledGenerator& generator) const;
        std::optional<DelayInterval> _computeArcDelayWindow(const TAPN::TimedPlace& place, int weight, const TAPN::TimeInterval& interval, const NonStrictMarkingBase& marking) const;
        int _getMaxPossibleDelay(const NonStrictMarkingBase& marking, const NextEnabledGenerator& generator) const;
        bool _isDelayEnabled(const TAPN::TimedTransition* trans, const NonStrictMarkingBase& marking, int maxPossibleDelay, int& outMinDelay, const NextEnabledGenerator& generator) const;

        std::vector<std::pair<std::string, int>> _parseStructureExpressions(rapidxml::xml_node<>* placeNode) const;
        void _parseColorSubterms(rapidxml::xml_node<>* node, int count, std::vector<std::pair<std::string, int>>& out) const;

        TAPN::TimedArcPetriNet& _tapn;
        const unfoldtacpn::ColoredPetriNetBuilder& _builder;
        const TraceMapper& _mapper;
        const VerificationOptions& _options;
        std::unique_ptr<NonStrictMarkingBase> _currentMarking;
    };
}

#endif // INTERACTIVEMODE_HPP
