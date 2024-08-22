#ifndef STOCHASTIC_STRUCTURE
#define STOCHASTIC_STRUCTURE

#include <random>
#include <sstream>
#include <string>

namespace VerifyTAPN::SMC {

    enum FiringMode {
        Oldest,
        Youngest,
        Random
    };

    std::string firingModeName(FiringMode type);

    enum DistributionType {
        Constant,
        Uniform,
        Exponential,
        Normal,
        Gamma,
        Erlang,
        DiscreteUniform,
        Geometric
    };

    std::string distributionName(DistributionType type);

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
    struct SMCGeometricParameters {
        double p;
    };

    union DistributionParameters {
        SMCUniformParameters uniform;
        SMCExponentialParameters exp;
        SMCNormalParameters normal;
        SMCConstantParameters constant;
        SMCGammaParameters gamma;
        SMCDiscreteUniformParameters discreteUniform;
        SMCGeometricParameters geometric;
    };

    struct Distribution {
        DistributionType type;
        DistributionParameters parameters;

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
                case Erlang:
                    date = std::gamma_distribution(parameters.gamma.shape, parameters.gamma.scale)(engine);
                    break;
                case DiscreteUniform:
                    date = std::uniform_int_distribution(parameters.discreteUniform.a, parameters.discreteUniform.b)(engine);
                    break;
                case Geometric:
                    date = std::geometric_distribution(parameters.geometric.p)(engine);
                    break;
            }
            return std::max(date, 0.0);
        }

        static Distribution urgent();

        static Distribution defaultDistribution();

        static Distribution fromParams(int distrib_id, double param1, double param2);

        std::string toXML() const;

    };

}

#endif