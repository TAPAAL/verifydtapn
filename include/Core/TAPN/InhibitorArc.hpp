#ifndef INHIBITORARC_HPP_
#define INHIBITORARC_HPP_

#include <vector>
#include "TimeInterval.hpp"

namespace VerifyTAPN::TAPN {
        class TimedTransition;

        class TimedPlace;

        class InhibitorArc {
        public: // typedefs
            typedef std::vector<InhibitorArc *> Vector;
        public:
            InhibitorArc(TimedPlace &place, TimedTransition &transition, const int weight) : place(place),
                                                                                             transition(transition),
                                                                                             weight(weight) {};

            virtual ~InhibitorArc() { /* empty */ }

        public: // modifiers
            inline TimedPlace &getInputPlace() const { return place; }

            inline TimedTransition &getOutputTransition() const { return transition; }

        public: // Inspectors
            void print(std::ostream &out) const;

            inline int getWeight() const { return weight; }

        private:
            TimedPlace &place;
            TimedTransition &transition;
            const int weight;
        };

        inline std::ostream &operator<<(std::ostream &out, const InhibitorArc &arc) {
            arc.print(out);
            return out;
        }
    }

#endif /* INHIBITORARC_HPP_ */
