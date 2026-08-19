#ifndef TRACE_MAPPER_HPP
#define TRACE_MAPPER_HPP

#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <string>
#include <Colored/ColoredPetriNetBuilder.h>

namespace VerifyTAPN {
    class TraceMapper {
    public:
        using VariableBinding = std::pair<std::string, std::string>;
        using BindingList = std::vector<VariableBinding>;

        static TraceMapper fromBuilder(const unfoldtacpn::ColoredPetriNetBuilder& builder);

        // map place is a partial function because __SUM places does not exist in the original net
        std::optional<std::string_view> mapPlace(const std::string& name) const;
        std::string_view mapTransition(const std::string& name) const;
        const BindingList& getBindings(const std::string& name) const;
        std::string_view getColor(const std::string& unfoldedPlaceName) const;

    private:
        using UnfoldedMapping = std::unordered_map<std::string, std::string>;
        using BindingsMapping = std::unordered_map<std::string, BindingList>;

        UnfoldedMapping placeMap;
        UnfoldedMapping placeColorMap;
        UnfoldedMapping transMap;
        BindingsMapping bindingsMap;
        static const BindingList emptyBindings;
    };
}

#endif