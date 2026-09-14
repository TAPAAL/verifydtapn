#include "Core/TraceMapper.hpp"

#include <algorithm>
#include <Colored/ColoredPetriNetBuilder.h>
#include <stdexcept>
#include <optional>
#include <string_view>

using namespace VerifyTAPN;

const TraceMapper::BindingList TraceMapper::emptyBindings{};

TraceMapper TraceMapper::fromBuilder(const unfoldtacpn::ColoredPetriNetBuilder& builder) {
    TraceMapper mapper;
    std::unordered_map<std::string, std::unordered_map<uint32_t, std::string>> placeColorIdToName;
    for (const auto& [origPlace, colorMap] : builder.getPlaceColorIds()) {
        for (const auto& [colorName, colorId] : colorMap) {
            placeColorIdToName[origPlace][colorId] = colorName;
        }
    }

    for (const auto& [origPlace, colorMap] : builder.getUnfoldedPlaceNames()) {
        const auto colorIdIt = placeColorIdToName.find(origPlace);
        for (const auto& [colorId, unfoldedName] : colorMap) {
            mapper.placeMap[unfoldedName] = origPlace;
            if (colorIdIt != placeColorIdToName.end()) {
                const auto nameIt = colorIdIt->second.find(colorId);
                if (nameIt != colorIdIt->second.end()) {
                    mapper.placeColorMap[unfoldedName] = nameIt->second;
                    continue;
                }
            }
            mapper.placeColorMap[unfoldedName] = "dot";
        }
    }

    for (const auto& [origTrans, unfoldedList] : builder.getUnfoldedTransitionNames()) {
        for (const auto& unfoldedName : unfoldedList) {
            mapper.transMap[unfoldedName] = origTrans;
        }
    }

    for (const auto& [transId, bindings] : builder.getTransitionBindings()) {
        auto sortedBindings = bindings;
        std::sort(sortedBindings.begin(), sortedBindings.end());
        mapper.bindingsMap[transId] = std::move(sortedBindings);
    }

    return mapper;
}

std::optional<std::string_view> TraceMapper::mapPlace(const std::string& name) const {
    if (name.size() >= 7 && name.rfind("__", 0) == 0 && name.compare(name.size() - 5, 5, "__SUM") == 0) {
        return std::nullopt;
    }

    const auto it = placeMap.find(name);
    if (it != placeMap.end()) {
        return std::string_view{it->second};
    }

    throw std::out_of_range("Place \"" + name + "\" not found");
}

std::string_view TraceMapper::mapTransition(const std::string& name) const {
    const auto it = transMap.find(name);
    if (it != transMap.end()) {
        return std::string_view{it->second};
    }

    throw std::out_of_range("Transition \"" + name + "\" not found");
}

const TraceMapper::BindingList& TraceMapper::getBindings(const std::string& name) const {
    const auto it = bindingsMap.find(name);
    if (it != bindingsMap.end()) {
        return it->second;
    }

    return emptyBindings;
}

std::string_view TraceMapper::getColor(const std::string& name) const {
    const auto it = placeColorMap.find(name);
    if (it != placeColorMap.end()) {
        return std::string_view{it->second};
    }

    static const std::string dot = "dot";
    return std::string_view{dot};
}