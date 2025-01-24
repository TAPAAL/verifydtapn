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
    float timestamp = marking->getTotalAge();
    QueryVisitor<RealMarking> checker(*marking, *_tapn);
    IntResult res;
    _expr->accept(checker, res);
    double value = res.value;
    if(_values.size() == 0 || value != _values.back()) {
        _values.push_back(value);
        _timestamps.push_back(timestamp);
        _steps.push_back(_current_step);
    }
    _current_step++;
    return value;
}

void Watch::reset()
{
    _values.clear();
    _timestamps.clear();
    _steps.clear();
    _current_step = 0;
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
        auto& max_step = steps.back();
        if(max_step > longest) {
            longest = max_step;
        }
    }
    if(nBins == 0 || nBins >= longest) {
        nBins = longest;
    }
    step_avg = std::vector(nBins, std::numeric_limits<float>::quiet_NaN());
    step_min = step_avg;
    step_max = step_avg;
    
}

void WatchAggregator::aggregateTime(unsigned int nBins)
{
    if(timeBins == 0) {
        std::vector<size_t> indexs(time_values.size());
        std::iota(indexs.begin(), indexs.end(), 0);
        std::sort(indexs.begin(), indexs.end(), [this](size_t i, size_t j) {
            return timestamps[i] < timestamps[j];
        });
        std::vector<float> timestamps_bins(time_values.size());
        time_avg = std::vector(timestamps_bins);
        global_time_avg = 0;
        for(int i = 0 ; i < indexs.size() ; i++) {
            time_avg[i] = time_values[indexs[i]];
            timestamps_bins[i] = timestamps[indexs[i]];
            global_time_avg += time_avg[i];
        }
        time_min = time_avg;
        time_max = time_avg;
        global_time_avg /= time_avg.size();
        timestamps = timestamps_bins;
    } else {
        float timestampmax = timestamps.back();
        float binSize = timestampmax / timeBins;
        std::vector<float> timestamps_bins(timeBins, std::numeric_limits<float>::quiet_NaN());
        time_avg = std::vector(timestamps_bins); 
        time_min = std::vector(timestamps_bins);
        time_max = std::vector(timestamps_bins);
        std::vector<size_t> time_runs_count = std::vector(timeBins, (size_t) 0);
        for(int i = 0 ; i < time_values.size() ; i++) {
            float timestamp = timestamps[i];
            float value = time_values[i];
            size_t bin = floor(timestamp / binSize);
            bin = std::min(bin, (size_t) timeBins - 1);
            if(std::isnan(timestamps_bins[bin])) {
                timestamps_bins[bin] = bin * binSize;
                time_avg[bin] = value;
                time_max[bin] = value;
                time_min[bin] = value;
                time_runs_count[bin]++;
                continue;
            }
            time_avg[bin] += value;
            if(value > time_max[bin]) time_max[bin] = value;
            if(value < time_min[bin]) time_min[bin] = value;
            time_runs_count[bin]++;
        }
        timestamps = timestamps_bins;
        for(size_t i = 0 ; i < timeBins ; i++) {
            if(!std::isnan(timestamps[i])) {
                time_avg[i] /= time_runs_count[i];
                global_time_avg += time_avg[i] / timeBins;
            }   
        }
    }
    time_values.clear();
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

void VerifyTAPN::TAPN::printIntXPlot(std::stringstream &stream, const std::string &title, const std::vector<float> y)
{
    float prev = std::numeric_limits<float>::quiet_NaN();
    stream << title << std::endl;
    for(int i = 0 ; i < y.size() ; i++) {
        float value = y[i];
        if(!std::isnan(prev) && value == prev) continue;
        stream << i << ":" << value << ";";
        prev = value;
    }
    stream << std::endl;
}