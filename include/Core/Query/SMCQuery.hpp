#ifndef SMCQUERIES_H
#define SMCQUERIES_H

#include "AST.hpp"
#include <PQL/SMCExpressions.h>
#include <random>
#include <sstream>
#include <string>

namespace VerifyTAPN::AST {

    enum SMCDistributionType {
        Constant,
        Uniform,
        Exponential,
        Normal,
        Gamma,
        DiscreteUniform,
    };

    std::string distributionName(SMCDistributionType type);

    struct SMCUniformParameters {
        double a;
        double b;
    };
    struct SMCExponentialParameters {
        double rate;
    };
    struct SMCNormalParameters {
        double mean;
        double stddev;
    };
    struct SMCConstantParameters {
        double value;
    };
    struct SMCGammaParameters {
        double shape;
        double scale;
    };
    struct SMCDiscreteUniformParameters {
        int a;
        int b;
    };

    union SMCDistributionParameters {
        SMCUniformParameters uniform;
        SMCExponentialParameters exp;
        SMCNormalParameters normal;
        SMCConstantParameters constant;
        SMCGammaParameters gamma;
        SMCDiscreteUniformParameters discreteUniform;
    };

    struct SMCDistribution {
        SMCDistributionType type;
        SMCDistributionParameters parameters;

        template<typename T>
        double sample(T& engine) const {
            double date = 0;
            switch(type) {
                case Constant:
                    date = parameters.constant.value;
                    break;
                case Uniform:
                    date = std::uniform_real_distribution(parameters.uniform.a, parameters.uniform.b)(engine);
                    break;
                case Exponential:
                    date = std::exponential_distribution(parameters.exp.rate)(engine);
                    break;
                case Normal:
                    date = std::normal_distribution(parameters.normal.mean, parameters.normal.stddev)(engine);
                    break;
                case Gamma:
                    date = std::gamma_distribution(parameters.gamma.shape, parameters.gamma.scale)(engine);
                    break;
                case DiscreteUniform:
                    date = std::uniform_int_distribution(parameters.discreteUniform.a, parameters.discreteUniform.b)(engine);
            }
            return std::max(date, 0.0);
        }

        static SMCDistribution urgent() {
            SMCDistributionParameters params;
            params.constant.value = 0;
            return SMCDistribution { Constant, params };
        }

        static SMCDistribution defaultDistribution() {
            SMCDistributionParameters params;
            params.constant.value = 1;
            return SMCDistribution { Constant, params };
        }

        static SMCDistribution fromParams(int distrib_id, double param1, double param2) {
            SMCDistributionType distrib = static_cast<SMCDistributionType>(distrib_id);
            SMCDistributionParameters params;
            switch(distrib){
                case Constant:
                    params.constant.value = param1;
                    break; 
                case Uniform:
                    params.uniform.a = param1;
                    params.uniform.b = param2;
                    break;
                case Exponential:
                    params.exp.rate = param1;
                    break;
                case Normal:
                    params.normal.mean = param1;
                    params.normal.stddev = param2;
                    break;
                case Gamma:
                    params.gamma.shape = param1;
                    params.gamma.scale = param2;
                    break;
                case DiscreteUniform:
                    params.discreteUniform.a = (int) param1;
                    params.discreteUniform.b = (int) param2;
                    break;
                default:
                    break;
            }
            return SMCDistribution { distrib, params };
        }

        std::string toXML() const {
            std::string endField = "\" ";
            std::stringstream res;
            res << "distribution=\"";
            res << distributionName(type) << endField;
            switch(type) {
                case Constant:
                    res << "value=\"" << parameters.constant.value << endField;
                    break;
                case Uniform:
                    res << "a=\"" << parameters.uniform.a << endField;
                    res << "b=\"" << parameters.uniform.b << endField;
                    break;
                case Exponential:
                    res << "rate=\"" << parameters.exp.rate << endField;
                    break;
                case Normal:
                    res << "mean=\"" << parameters.normal.mean << endField;
                    res << "stddev=\"" << parameters.normal.stddev << endField;
                    break;
                case Gamma:
                    res << "shape=\"" << parameters.gamma.shape << endField;
                    res << "scale=\"" << parameters.gamma.scale << endField;
                    break;
                case DiscreteUniform:
                    res << "a=\"" << parameters.discreteUniform.a << endField;
                    res << "b=\"" << parameters.discreteUniform.b << endField;
                    break;
            }
            return res.str();
        }

    };

    struct SMCSettings {
        int timeBound;
        int stepBound;
        float falsePositives;
        float falseNegatives;
        float indifferenceRegionUp;
        float indifferenceRegionDown;
        float confidence;
        float estimationIntervalWidth;
        bool compareToFloat;
        float geqThan;

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