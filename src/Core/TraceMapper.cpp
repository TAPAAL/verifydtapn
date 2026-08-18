#include "Core/TraceMapper.hpp"

#include <Colored/ColoredPetriNetBuilder.h>
#include <string_view>
#include <stdexcept>
#include <format>
#include <optional>

using namespace VerifyTAPN;

TraceMapper TraceMapper::fromBuilder(const unfoldtacpn::ColoredPetriNetBuilder& builder) {
    TraceMapper mapper;
    for (const auto& [origPlace, colorMap] : builder.getUnfoldedPlaceNames()) {
        for (const auto& [_, unfoldedName] : colorMap) {
            mapper.placeMap[unfoldedName] = origPlace;
        }
    }

    for (const auto& [origTrans, unfoldedList] : builder.getUnfoldedTransitionNames()) {
        for (const auto& unfoldedName : unfoldedList) {
            mapper.transMap[unfoldedName] = origTrans;
        }
    }

    return mapper;
}

std::optional<std::string_view> TraceMapper::mapPlace(std::string_view name) const {
    if (name.starts_with("__") && name.ends_with("__SUM")) {
        return std::nullopt;
    }

    const auto it = placeMap.find(name);
    if (it != placeMap.end()) {
        return it->second;
    }
    
    throw std::out_of_range(std::format("Place \"{}\" not found", name));
}

std::string_view TraceMapper::mapTransition(std::string_view name) const {
    const auto it = transMap.find(name);
    if (it != transMap.end()) {
        return it->second;
    }

    throw std::out_of_range(std::format("Transition \"{}\" not found", name));
}