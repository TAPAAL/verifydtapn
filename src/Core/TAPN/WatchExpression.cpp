#include "Core/TAPN/WatchExpression.hpp"
#include "DiscreteVerification/QueryVisitor.hpp"
#include "DiscreteVerification/DataStructures/RealMarking.hpp"

#include <numeric>

using namespace VerifyTAPN::TAPN;
using VerifyTAPN::DiscreteVerification::RealMarking;
using VerifyTAPN::DiscreteVerification::QueryVisitor;

using std::vector;

float Watch::new_marking(RealMarking *marking)
{
    if(_max_step > 0 && marking->getGeneratedBy() == nullptr) {
        return _values.back();
    }
    float timestamp = marking->getTotalAge();
    QueryVisitor<RealMarking> checker(*marking, *_tapn);
    IntResult res;
    _expr->accept(checker, res);
    double value = res.value;
    if(_values.size() == 0 || value != _values.back()) {
        _values.push_back(value);
        _timestamps.push_back(timestamp);
        _steps.push_back(_max_step);
    }
    _max_step++;
    _max_time = timestamp;
    return value;
}

void Watch::close() 
{
    if(_steps.size() == 0 || _steps.back() == _max_step) {
        return;
    }
    _values.push_back(_values.back());
    _timestamps.push_back(_max_time);
    _steps.push_back(_max_step);
}

void Watch::reset()
{
    _values.clear();
    _timestamps.clear();
    _steps.clear();
    _max_step = 0;
    _max_time = 0;
}

size_t Watch::n_values() const {
    return _values.size();
}

std::string Watch::get_plots(const std::string& name) const 
{
    std::stringstream plots;
    std::vector<float> bins;
    std::string id = "(" + name + ")";
    printPlot(plots, id + " value/step", _steps, _values);
    printPlot(plots, id + " value/time", _timestamps, _values);
    return plots.str();
}

void WatchAggregator::new_watch(Watch* watch)
{
    watch_values.push_back(watch->_values);
    watch_timestamps.push_back(watch->_timestamps);
    watch_steps.push_back(watch->_steps);
}

void WatchAggregator::aggregateSteps(unsigned int nBins)
{
    unsigned short longest = 0;
    for(const auto& steps : watch_steps) {
        unsigned short max_step = steps.back();
        if(max_step > longest) {
            longest = max_step;
        }
    }
    if(nBins == 0 || nBins >= longest) {
        nBins = longest;
    }
    float delta = longest / nBins;
    step_avg = std::vector(nBins + 1, std::numeric_limits<float>::quiet_NaN());
    step_min = std::vector(step_avg);
    step_max = std::vector(step_avg);
    std::vector<float> current_values(watch_steps.size());
    for(int j = 0 ; j < watch_values.size() ; j++) {
        current_values[j] = watch_values[j][0];
    }
    std::vector<size_t> current_indexs(watch_steps.size(), 0);
    for(int i = 0 ; i <= nBins ; i++) {
        float current_x = i * delta;
        float avg = 0;
        float min = std::numeric_limits<float>::quiet_NaN();
        float max = std::numeric_limits<float>::quiet_NaN();
        int n_runs = 0;
        for(int j = 0 ; j < watch_steps.size() ; j++) {
            size_t idx = current_indexs[j];
            if(idx >= watch_steps[j].size()) {
                continue;
            }
            if(idx < watch_steps[j].size() && watch_steps[j][idx] > current_x) {
                float value = current_values[j];
                avg += value;
                n_runs++;
                if(std::isnan(max) || value > max) {
                    max = value;
                }
                if(std::isnan(min) || value < min) {
                    min = value;
                }
            }
            while(idx < watch_steps[j].size() && watch_steps[j][idx] <= current_x) {
                float value = watch_values[j][idx];;
                avg += value;
                n_runs++;
                if(std::isnan(max) || value > max) {
                    max = value;
                }
                if(std::isnan(min) || value < min) {
                    min = value;
                }
                current_values[j] = value;
                idx++;
            }
            current_indexs[j] = idx;
        }
        step_avg[i] = avg / n_runs;
        step_min[i] = min;
        step_max[i] = max;
        step_bins.push_back(i * delta);
        global_steps_avg += step_avg[i];
    }
    global_steps_avg /= (nBins + 1);
    watch_steps.clear();
}

void WatchAggregator::infiniteTimeAggreg() 
{
    std::vector<float> current_values(watch_values.size());
    for(int j = 0 ; j < watch_values.size() ; j++) {
        current_values[j] = watch_values[j][0];
    }
    std::vector<size_t> current_indexs(watch_timestamps.size(), 0);
    float next_x = 0;
    while(next_x != std::numeric_limits<float>::infinity()) {
        float avg = 0;
        float min = std::numeric_limits<float>::quiet_NaN();
        float max = std::numeric_limits<float>::quiet_NaN();
        int n_runs = 0;
        next_x = std::numeric_limits<float>::infinity();
        for(int j = 0 ; j < watch_timestamps.size() ; j++) {
            size_t idx = current_indexs[j];
            if(idx >= watch_timestamps[j].size()) {
                continue;
            }
            if(watch_timestamps[j][idx] < next_x) {
                next_x = watch_timestamps[j][idx];
            }
        }
        if(next_x == std::numeric_limits<float>::infinity()) {
            break;
        }
        for(int j = 0 ; j < watch_timestamps.size() ; j++) {
            size_t idx = current_indexs[j];
            if(idx >= watch_timestamps[j].size()) {
                continue;
            }
            while(idx < watch_timestamps[j].size() && watch_timestamps[j][idx] == next_x) {
                float value = watch_values[j][idx];
                avg += value;
                if(std::isnan(min) || value < min) {
                    min = value;
                }
                if(std::isnan(max) || value > max) {
                    max = value;
                }
                n_runs++;
                idx++;
            }
            current_indexs[j] = idx;
        }
        time_avg.push_back(avg / n_runs);
        time_min.push_back(min);
        time_max.push_back(max);
        timestamps.push_back(next_x);
        global_time_avg += time_avg.back();
    }
    global_time_avg /= time_avg.size();
    watch_timestamps.clear();
}

void WatchAggregator::aggregateTime(unsigned int nBins)
{
    float longest = 0;
    for(const auto& times : watch_timestamps) {
        float max_time = times.back();
        if(max_time > longest) {
            longest = max_time;
        }
    }
    if(nBins == 0) {
        infiniteTimeAggreg();
        return;
    }
    float delta = longest / (float) nBins;
    time_avg = std::vector(nBins + 1, std::numeric_limits<float>::quiet_NaN());
    time_min = std::vector(time_avg);
    time_max = std::vector(time_avg);
    std::vector<float> current_values(watch_timestamps.size());
    for(int j = 0 ; j < current_values.size() ; j++) {
        current_values[j] = watch_values[j][0];
    }
    std::vector<size_t> current_indexs(watch_timestamps.size(), (size_t) 0);
    for(int i = 0 ; i <= nBins ; i++) {
        float current_x = i * delta;
        float avg = 0;
        float min = std::numeric_limits<float>::quiet_NaN();
        float max = std::numeric_limits<float>::quiet_NaN();
        int n_runs = 0;
        for(int j = 0 ; j < watch_timestamps.size() ; j++) {
            size_t idx = current_indexs[j];
            if(idx >= watch_timestamps[j].size()) {
                continue;
            }
            if(idx < watch_timestamps[j].size() && watch_timestamps[j][idx] > current_x) {
                float value = current_values[j];
                avg += value;
                n_runs++;
                if(std::isnan(max) || value > max) {
                    max = value;
                }
                if(std::isnan(min) || value < min) {
                    min = value;
                }
            }
            while(idx < watch_timestamps[j].size() && watch_timestamps[j][idx] <= current_x) {
                float value = watch_values[j][idx];;
                avg += value;
                n_runs++;
                if(std::isnan(max) || value > max) {
                    max = value;
                }
                if(std::isnan(min) || value < min) {
                    min = value;
                }
                current_values[j] = value;
                idx++;
            }
            current_indexs[j] = idx;
        }
        time_avg[i] = avg / n_runs;
        time_min[i] = min;
        time_max[i] = max;
        timestamps.push_back(i * delta);
        global_time_avg += time_avg[i];
    }
    global_time_avg /= (float) (nBins + 1);
    watch_timestamps.clear();
}

void WatchAggregator::resetAggregation() 
{
    step_bins.clear();
    step_avg.clear();
    step_min.clear();
    step_max.clear();

    timestamps.clear();
    time_avg.clear();
    time_min.clear();
    time_max.clear();

    global_steps_avg = 0.0;
    global_time_avg = 0.0;
}

void WatchAggregator::aggregate(unsigned int stepBins, unsigned int timeBins)
{
    resetAggregation();
    aggregateSteps(stepBins);
    aggregateTime(timeBins);
    watch_values.clear();
}

void WatchAggregator::reset()
{
    watch_values.clear();
    watch_timestamps.clear();
    watch_steps.clear();
    resetAggregation();
}

std::string WatchAggregator::get_plots(const std::string& name) const
{
    std::stringstream plots;
    std::string id = "(" + name + ")";
    printPlot(plots, id + " avg/step", step_bins, step_avg);
    printPlot(plots, id + " min/step", step_bins, step_min);
    printPlot(plots, id + " max/step", step_bins, step_max);
    printPlot(plots, id + " avg/time", timestamps, time_avg);
    printPlot(plots, id + " min/time", timestamps, time_min);
    printPlot(plots, id + " max/time", timestamps, time_max);
    plots << id << " Global steps avg.: " << global_steps_avg << std::endl;
    plots << id << " Global time avg.: " << global_time_avg << std::endl;
    return plots.str();
}