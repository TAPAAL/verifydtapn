#ifndef SMCQUERIES_H
#define SMCQUERIES_H

#include "AST.hpp"
#include <PQL/SMCExpressions.h>

namespace VerifyTAPN::AST {

    enum SMCSemantics { Weak, Strong };

    struct SMCSettings {
        enum SMCBoundType { TimeBound, StepBound };
        SMCBoundType boundType;
        int bound;
        float falsePositives;
        float falseNegatives;
        float indifferenceRegionUp;
        float indifferenceRegionDown;
        float defaultRate;
        float confidence;
        float estimationIntervalWidth;
        bool compareToFloat;
        float geqThan;
        SMCSemantics semantics;

        static SMCSettings fromPQL(unfoldtacpn::PQL::SMCSettings settings);
    };

    class SMCQuery : public Query {
        public:

            SMCQuery(Quantifier quantifier, SMCSettings settings, Expression *expr) 
                : Query(quantifier, expr), smcSettings(settings)
            { };

            SMCQuery(const SMCQuery &other) 
                : Query(other.quantifier, other.expr->clone()), smcSettings(other.smcSettings)
            { };

            SMCQuery &operator=(const SMCQuery &other) {
                if (&other != this) {
                    delete expr;
                    expr = other.expr->clone();
                    smcSettings = other.smcSettings;
                }
                return *this;
            }

            virtual SMCQuery *clone() const;

            void accept(Visitor &visitor, Result &context) override;


            void setSMCSettings(SMCSettings newSettings) {
                smcSettings = newSettings;
            }

            SMCSettings& getSmcSettings() {
                return smcSettings;
            }

        private:
            Quantifier quantifier;
            Expression *expr;
            SMCSettings smcSettings;
        };

}

#endif