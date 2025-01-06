#ifndef WATCH_EXPR_HPP_
#define WATCH_EXPR_HPP_

#include "DiscreteVerification/DataStructures/RealMarking.hpp"

using VerifyTAPN::DiscreteVerification::RealMarking;

namespace VerifyTAPN::TAPN {

    class WatchExpression {
        public:
            virtual double eval(RealMarking* marking) = 0;
    };

    class WatchConstant : public WatchExpression {
        protected:
            double _value;
        public:
            WatchConstant(double value) 
                : _value(value) { }

            double eval(RealMarking* marking) override { return _value; }
    };

    class WatchPlace : public WatchExpression {
        protected:
            std::vector<int> _ids;
        public:
            WatchPlace(std::vector<int> ids) 
                : _ids(ids) { }

            double eval(RealMarking* marking) override;
    };

    class WatchAdd : public WatchExpression {
        protected:
            std::unique_ptr<WatchExpression> _left;
            std::unique_ptr<WatchExpression> _right;
        public:
            WatchAdd(WatchExpression* left, WatchExpression* right) 
                : _left(left), _right(right) { }

            double eval(RealMarking* marking) override;
    };

    class WatchSubtract : public WatchExpression {
        protected:
            std::unique_ptr<WatchExpression> _left;
            std::unique_ptr<WatchExpression> _right;
        public:
            WatchSubtract(WatchExpression* left, WatchExpression* right) 
                : _left(left), _right(right) { }

            double eval(RealMarking* marking) override;
    };

    class WatchMultiply : public WatchExpression {
        protected:
            std::unique_ptr<WatchExpression> _left;
            std::unique_ptr<WatchExpression> _right;
        public:
            WatchMultiply(WatchExpression* left, WatchExpression* right) 
                : _left(left), _right(right) { }

            double eval(RealMarking* marking) override;
    };

    class WatchDivide : public WatchExpression {
        protected:
            std::unique_ptr<WatchExpression> _left;
            std::unique_ptr<WatchExpression> _right;
        public:
            WatchDivide(WatchExpression* left, WatchExpression* right) 
                : _left(left), _right(right) { }

            double eval(RealMarking* marking) override;
    };

    class Watch {
        friend class WatchAggregator;

        protected:
            WatchExpression* _expr;
            std::vector<float> _values;
            std::vector<float> _timestamps;

        public:
            Watch(WatchExpression* expr) : _expr(expr) { }

            float new_marking(RealMarking* marking);

            std::string get_plots(const std::string& name) const;

            void reset();
    };

    class WatchAggregator {

        public:

            void new_watch(Watch* watch);

            void aggregate(unsigned int stepBins, unsigned int timeBins);
            void reset();

            std::string get_plots(const std::string& name) const;

            int n_watchs = 0;
            std::vector<float> step_bins;
            std::vector<float> step_avg;
            std::vector<float> step_min;
            std::vector<float> step_max;
            std::vector<float> time_values;
            std::vector<float> time_avg;
            std::vector<float> time_min;
            std::vector<float> time_max;
            std::vector<float> timestamps;
    };

    using Observable = std::tuple<std::string, WatchExpression*>;

    void printPlot(std::stringstream& stream, const std::string& title, const std::vector<float>& x, const std::vector<float> y);
    void printIntXPlot(std::stringstream& stream, const std::string& title, const std::vector<float> y);

}

#endif