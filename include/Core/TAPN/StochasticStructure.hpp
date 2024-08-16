#ifndef STOCHASTIC_STRUCTURE
#define STOCHASTIC_STRUCTURE

#include <random>
#include <sstream>
#include <string>

namespace VerifyTAPN::SMC {

    enum SMCFiringMode {
        Oldest,
        Youngest,
        Random
    };

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

        static SMCDistribution urgent();

        static SMCDistribution defaultDistribution();

        static SMCDistribution fromParams(int distrib_id, double param1, double param2);

        std::string toXML() const;

    };

}

#endif