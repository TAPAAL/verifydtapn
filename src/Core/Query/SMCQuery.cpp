#include "Core/Query/SMCQuery.hpp"

namespace VerifyTAPN::AST {

    namespace pql = unfoldtacpn::PQL;

    SMCSettings SMCSettings::fromPQL(pql::SMCSettings settings) {
        return {
            settings.boundType == pql::SMCSettings::TimeBound ? TimeBound : StepBound,
            settings.bound,
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

    SMCQuery *SMCQuery::clone() const {
        return new SMCQuery(*this);
    }

    void SMCQuery::accept(Visitor &visitor, Result &context) {
        visitor.visit(*this, context);
    }

}