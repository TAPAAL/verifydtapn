#ifndef WATCH_EXPR_HPP_
#define WATCH_EXPR_HPP_

#include "Core/Query/AST.hpp"

#include <cmath>
#include <string>
#include <sstream>

namespace VerifyTAPN::DiscreteVerification {
    class RealMarking;
}
using VerifyTAPN::DiscreteVerification::RealMarking;

using VerifyTAPN::AST::ArithmeticExpression;

namespace VerifyTAPN::TAPN {

    class Watch {
        friend class WatchAggregator;

        protected:
            ArithmeticExpression* _expr;
            std::vector<float> _values;
            std::vector<float> _timestamps;
            std::vector<unsigned short> _steps;
            TimedArcPetriNet* _tapn;

            float _max_time = 0;
            float _max_step = 0;

        public:
            Watch(TimedArcPetriNet* tapn, ArithmeticExpression* expr) : _expr(expr), _tapn(tapn) { }

            float new_marking(RealMarking* marking);
            void close();

            std::string get_plots(const std::string& name) const;

            void reset();

            size_t n_values() const;
    };

    class WatchAggregator {

        public:

            void new_watch(Watch* watch);

            void aggregate(unsigned int stepBins, unsigned int timeBins);
            void reset();

            std::string get_plots(const std::string& name) const;

            std::vector<float> step_bins;
            std::vector<float> step_avg;
            std::vector<float> step_min;
            std::vector<float> step_max;

            std::vector<float> timestamps;
            std::vector<float> time_avg;
            std::vector<float> time_min;
            std::vector<float> time_max;

            std::vector<std::vector<float>> watch_values;
            std::vector<std::vector<float>> watch_timestamps;
            std::vector<std::vector<unsigned short>> watch_steps;

            float global_steps_avg = 0.0;
            float global_time_avg = 0.0;

        private:

            void resetAggregation();
            void aggregateSteps(unsigned int stepBins);
            void infiniteTimeAggreg();
            void aggregateTime(unsigned int timeBins);

    };

    using Observable = std::tuple<std::string, ArithmeticExpression*>;

    template<typename T>
    void printPlot(std::stringstream& stream, const std::string& title, const std::vector<T>& x, const std::vector<float> y)
    {
        float prev = std::numeric_limits<float>::quiet_NaN();
        stream << title << std::endl;
        for(int i = 0 ; i < x.size() ; i++) {
            if(std::isnan(y[i])) {
                continue;
            }
            float value = y[i];
            //if(!std::isnan(prev) && value == prev && i < x.size() - 1) continue;
            stream << x[i] << ":" << value << ";";
            prev = value;
        }
        stream << std::endl;
    }

}

#endif