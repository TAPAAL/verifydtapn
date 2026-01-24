#ifndef RealMarking_HPP
#define RealMarking_HPP

#include <vector>
#include "Core/TAPN/TAPN.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarkingBase.hpp"
#include "DiscreteVerification/Util/ClockValue.hpp"

using namespace VerifyTAPN::DiscreteVerification::Util;

namespace VerifyTAPN::DiscreteVerification {

    class RealToken {

        private:

            clockValue age;
            int count;

        public:

            RealToken(clockValue value, int count) : age(value), count(count) { }
            RealToken(const RealToken&) = default;
            RealToken(const Token& t) : count(t.getCount()) {
                age = t.getAge();
            }

            inline int cmp(const RealToken &t) const {
                if (count != t.count) return count - t.count;
                return age - t.age > 0 ? 1 : -1;
            }

            inline bool equals(const RealToken &t) const { return (this->age == t.age && this->count == t.count); };

            inline void add(int num) { count = count + num; };

            inline int getCount() const { return count; };

            inline clockValue getAge() const { return age; };

            inline void setAge(clockValue i) { age = i; };

            inline void setCount(int i) { count = i; };

            inline void remove(int num) { count = count - num; };

            inline void deltaAge(clockValue x) {
                age += x;
            }

    };

    typedef std::vector<RealToken> RealTokenList;

    class RealPlace {

        public:

            const TAPN::TimedPlace* place;
            RealTokenList tokens;

            explicit RealPlace(const TAPN::TimedPlace* place) : place(place) { }
            
            RealPlace(const RealPlace& p) : place(p.place) {
                tokens = p.tokens;
            };

            RealPlace(const Place& p) : place(p.place) {
                for(const auto& token : p.tokens) {
                    tokens.push_back(RealToken(token));
                }
            }

            inline Place generateImagePlace() const {
                Place img(place);
                img.tokens.push_back(Token(0, numberOfTokens()));
                return img;
            }

            inline int numberOfTokens() const {
                int count = 0;
                for(const auto& token : tokens) {
                    count += token.getCount();
                }
                return count;
            }

            inline void deltaAge(clockValue x) {
                for(auto& token : tokens) {
                    token.deltaAge(x);
                }
            }

            inline clockValue maxTokenAge() const {
                if(tokens.size() == 0) {
                    return 0;
                }
                return tokens.back().getAge();
            }

            void add(RealToken new_token);

            void add(clockValue age = 0) {
                add(RealToken(age, 1));
            }

            bool remove(RealToken to_remove);

            clockValue availableDelay(const uint32_t precision) const {
                if(tokens.size() == 0) return std::numeric_limits<clockValue>::max();
                clockValue bound = toClock(place->getInvariant().getBound(), precision);
                clockValue maxAge = maxTokenAge();
                if(bound < maxAge) {
                    return 0;
                }
                return bound - maxAge;
            }

            inline int placeId() const {
                return place->getIndex();
            }

            inline bool isEmpty() const {
                return tokens.size() == 0;
            }

    };

    typedef std::vector<RealPlace> RealPlaceList;

    class RealMarking {

        public:

            explicit RealMarking(TAPN::TimedArcPetriNet* net, NonStrictMarkingBase& base);
            RealMarking(const RealMarking& other);

            uint32_t size() const;
    
            RealPlaceList& getPlaceList();
            RealTokenList& getTokenList(int placeId);

            void deltaAge(clockValue x);

            NonStrictMarkingBase generateImage();

            uint32_t numberOfTokensInPlace(int placeId) const;

            bool canDeadlock(const TAPN::TimedArcPetriNet &tapn, int maxDelay, bool ignoreCanDelay) const;

            inline bool canDeadlock(const TAPN::TimedArcPetriNet &tapn, const int maxDelay) const {
                return canDeadlock(tapn, maxDelay, false);
            };

            bool removeToken(int placeId, clockValue age);

            bool removeToken(int placeId, RealToken &token);

            bool removeToken(RealPlace &place, RealToken &token);

            void addTokenInPlace(TAPN::TimedPlace &place, clockValue age = 0);

            void addTokenInPlace(RealPlace &place, RealToken &token);

            void addTokenInPlace(const TAPN::TimedPlace &place, RealToken &token);

            clockValue availableDelay(const uint32_t precision) const;

            void setDeadlocked(const bool dead);

            inline const TAPN::TimedTransition *getGeneratedBy() const { return generatedBy; }

            inline void setGeneratedBy(const TAPN::TimedTransition *generatedBy) { this->generatedBy = generatedBy; }

            inline clockValue getPreviousDelay() const { return fromDelay; }

            inline clockValue getTotalAge() const { return totalAge; }

            inline void setPreviousDelay(const clockValue delay) { this->fromDelay = delay; }

            bool enables(TAPN::TimedTransition* transition, const uint32_t precision);

            unsigned int _thread_id = 0;

        private:

            RealPlaceList places;
            bool deadlocked;

            const TAPN::TimedTransition *generatedBy = nullptr;
            clockValue fromDelay = 0;
            clockValue totalAge = 0;

            static RealTokenList emptyTokenList;

    };

}

#endif