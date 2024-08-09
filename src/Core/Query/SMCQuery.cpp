#include "Core/Query/SMCQuery.hpp"

namespace VerifyTAPN::AST {

    namespace pql = unfoldtacpn::PQL;

    SMCSettings SMCSettings::fromPQL(pql::SMCSettings settings) {
        return {
            settings.timeBound,
            settings.stepBound,
            settings.falsePositives,
            settings.falseNegatives,
            settings.indifferenceRegionUp,
            settings.indifferenceRegionDown,
            settings.confidence,
            settings.estimationIntervalWidth,
            settings.compareToFloat,
            settings.geqThan,
        };
    }
    
    std::string distributionName(SMCDistributionType type) {
        switch(type) {
            case Constant:
                return "constant";
            case Uniform:
                return "uniform";
            case Exponential:
                return "exponential";
            case Normal:
                return "normal";
            case Gamma:
                return "gamma";
            case DiscreteUniform:
                return "discrete uniform";
        }
        return "";
    }

    SMCQuery *SMCQuery::clone() const {
        return new SMCQuery(*this);
    }

    void SMCQuery::accept(Visitor &visitor, Result &context) {
        visitor.visit(*this, context);
    }

}