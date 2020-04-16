#ifndef TRANSPORTARC_HPP_
#define TRANSPORTARC_HPP_

#include "TimeInterval.hpp"
#include <vector>

namespace VerifyTAPN::TAPN {
    class TimedTransition;

    class TimedPlace;

    class TransportArc {
    public:
        typedef std::vector<TransportArc *> Vector;
    public:
        TransportArc(
                TimedPlace &source,
                TimedTransition &transition,
                TimedPlace &destination,
                const TAPN::TimeInterval &interval,
                const int weight
        ) : interval(interval), source(source), transition(transition), destination(destination), weight(weight) {};

        virtual ~TransportArc() = default;;
    public:
        inline TimedPlace &getSource() const { return source; }

        inline TimedTransition &getTransition() const { return transition; }

        inline TimedPlace &getDestination() const { return destination; }

        inline const TimeInterval &getInterval() const { return interval; }

        inline void divideIntervalBy(int divider) { interval.divideBoundsBy(divider); };

    public: // Inspectors
        void print(std::ostream &out) const;

        inline int getWeight() const { return weight; }

    private:
        TAPN::TimeInterval interval;
        TimedPlace &source;
        TimedTransition &transition;
        TimedPlace &destination;
        const int weight;
    };

    inline std::ostream &operator<<(std::ostream &out, const TransportArc &arc) {
        arc.print(out);
        return out;
    }
}

#endif /* TRANSPORTARC_HPP_ */
