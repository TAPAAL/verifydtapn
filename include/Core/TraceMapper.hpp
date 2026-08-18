#ifndef TRACE_MAPPER_HPP
#define TRACE_MAPPER_HPP

#include <unordered_map>
#include <string>
#include <Colored/ColoredPetriNetBuilder.h>
#include <string_view>
#include <functional>
#include <optional>

namespace VerifyTAPN {
    class TraceMapper {
    public:
    
    static TraceMapper fromBuilder(const unfoldtacpn::ColoredPetriNetBuilder& builder);
    
    // map place is a partial function because __SUM places does not exist in the original net
    std::optional<std::string_view> mapPlace(std::string_view name) const;
    std::string_view mapTransition(std::string_view name) const;
    
    private:
        struct StringHash {
            using is_transparent = void;
            [[nodiscard]] size_t operator()(const char *txt) const {
                return std::hash<std::string_view>{}(txt);
            }

            [[nodiscard]] size_t operator()(std::string_view txt) const {
                return std::hash<std::string_view>{}(txt);
            }

            [[nodiscard]] size_t operator()(const std::string &txt) const {
                return std::hash<std::string>{}(txt);
            }
        };

        using UnfoldedMapping = std::unordered_map<std::string, std::string, StringHash, std::equal_to<>>;
        UnfoldedMapping placeMap;
        UnfoldedMapping transMap;
    };
}

#endif