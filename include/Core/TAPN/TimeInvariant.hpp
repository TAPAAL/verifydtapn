#ifndef VERIFYTAPN_TAPN_TIMEINVARIANT_HPP_
#define VERIFYTAPN_TAPN_TIMEINVARIANT_HPP_

#include <string>
#include <limits>
#include <map>

namespace VerifyTAPN::TAPN {

        class TimeInvariant {
        public:
            static const TimeInvariant LS_INF;

        public: // construction/destruction
            TimeInvariant() : strictComparison(true), bound(std::numeric_limits<int>::max()) {};

            TimeInvariant(bool strictComparison, int bound) : strictComparison(strictComparison), bound(bound) {};

            TimeInvariant(const TimeInvariant &ti) = default;

            TimeInvariant &operator=(const TimeInvariant &ti) = default;

            virtual ~TimeInvariant() { /* Empty */ };
        public: // inspectors
            void print(std::ostream &out) const;

            inline int getBound() const { return bound; }

            inline bool isBoundStrict() const { return strictComparison; }

        public: // statics
            static TimeInvariant createFor(const std::string &invariant, std::map<std::string, int> replace);

        private: // data
            bool strictComparison;
            int bound;
        };

        inline bool operator==(const TimeInvariant &a, const TimeInvariant &b) {
            return a.getBound() == b.getBound() && a.isBoundStrict() == b.isBoundStrict();
        }

        inline bool operator!=(const TimeInvariant &a, const TimeInvariant &b) {
            return !(a == b);
        }

        inline std::ostream &operator<<(std::ostream &out, const TimeInvariant &invariant) {
            invariant.print(out);
            return out;
        }
    }

#endif /* VERIFYTAPN_TAPN_TIMEINVARIANT_HPP_ */
