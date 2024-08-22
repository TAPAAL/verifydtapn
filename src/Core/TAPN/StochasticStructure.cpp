#include "Core/TAPN/StochasticStructure.hpp"

namespace VerifyTAPN::SMC {

    std::string distributionName(DistributionType type) {
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
            case Erlang:
                return "erlang";
            case DiscreteUniform:
                return "discrete uniform";
            case Geometric:
                return "geometric";
        }
        return "";
    }

    std::string firingModeName(FiringMode mode) {
        switch(mode) {
            case Oldest:
                return "Oldest";
            case Youngest:
                return "Youngest";
            case Random:
                return "Random";
        }
        return "";
    }

    Distribution Distribution::urgent() {
        DistributionParameters params;
        params.constant.value = 0;
        return Distribution { Constant, params };
    }
    Distribution Distribution::defaultDistribution() {
        DistributionParameters params;
        params.constant.value = 1;
        return Distribution { Constant, params };
    }
    Distribution Distribution::fromParams(int distrib_id, double param1, double param2) {
        DistributionType distrib = static_cast<DistributionType>(distrib_id);
        DistributionParameters params;
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
            case Erlang:
                params.gamma.shape = param1;
                params.gamma.scale = param2;
                break;
            case DiscreteUniform:
                params.discreteUniform.a = (int) param1;
                params.discreteUniform.b = (int) param2;
                break;
            case Geometric:
                params.geometric.p = param1;
                break;
            default:
                break;
        }
        return Distribution { distrib, params };
    }
    std::string Distribution::toXML() const {
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
            case Erlang:
                res << "shape=\"" << parameters.gamma.shape << endField;
                res << "scale=\"" << parameters.gamma.scale << endField;
                break;
            case DiscreteUniform:
                res << "a=\"" << parameters.discreteUniform.a << endField;
                res << "b=\"" << parameters.discreteUniform.b << endField;
                break;
            case Geometric:
                res << "p=\"" << parameters.geometric.p << endField;
                break;
        }
        return res.str();
    }

}