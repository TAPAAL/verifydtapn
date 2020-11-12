/*
 * NonStrictMarkingBase.hpp
 *
 *  Created on: 29/02/2012
 *      Author: MathiasGS
 */

#ifndef NonStrictMarkingBase_HPP_
#define NonStrictMarkingBase_HPP_

#include "Core/TAPN/TAPN.hpp"

#include "boost/functional/hash.hpp"

#include <cassert>
#include <vector>
#include <iostream>

namespace VerifyTAPN { namespace DiscreteVerification {

    class Place;

    class Token;

    typedef std::vector<Token> TokenList;

    class Token {
    private:
        int age;
        int count;
    public:
        Token(int age, int count) : age(age), count(count) {};

        Token(const Token &t) = default;

        inline int cmp(const Token &t) const {
            // TODO check for overflow!
            if (count != t.count) return count - t.count;
            return age - t.age;
        }

        inline bool equals(const Token &t) const { return (this->age == t.age && this->count == t.count); };

        inline void add(int num) { count = count + num; };

        inline int getCount() const { return count; };

        inline int getAge() const { return age; };

        inline void setAge(int i) { age = i; };

        inline void setCount(int i) { count = i; };

        inline void remove(int num) { count = count - num; };

        // Ages all tokens by 1
        inline void incrementAge() {
            age++;
        }

        inline void incrementAge(int x) {
            age += x;
        }

        inline void decrementAge() {
            age--;
        }

        friend std::size_t hash_value(Token const &t) {
            size_t seed = 0;
            boost::hash_combine(seed, t.getAge());
            boost::hash_combine(seed, t.getCount());
            return seed;
        }
    };

    class Place {
    public:
        const TAPN::TimedPlace *place;
        TokenList tokens;

        explicit Place(const TAPN::TimedPlace *place) : place(place) {};

        Place(const Place &p) : place(p.place) {
            for (const auto &token : p.tokens) {
                tokens.push_back(token);
            }
        };

        friend std::size_t hash_value(Place const &p) {
            std::size_t seed = boost::hash_range(p.tokens.begin(), p.tokens.end());
            boost::hash_combine(seed, p.place->getIndex());

            return seed;
        }

        inline int numberOfTokens() const {
            int count = 0;
            for (const auto &token : tokens) {
                count += token.getCount();
            }
            return count;
        }

        inline int maxTokenAge() const {
            int max = -1;
            for (const auto &token : tokens) {
                if (token.getAge() > max) max = token.getAge();
            }
            return max;
        }

        // Ages all tokens by 1
        inline void incrementAge() {
            for (auto &token : tokens) {
                token.incrementAge();
            }
        }

        inline void incrementAge(int age) {
            for (auto &token : tokens) {
                token.incrementAge(age);
            }
        }

        inline void decrementAge() {
            for (auto &token : tokens) {
                token.decrementAge();
            }
        }

        inline bool allMaximumConstant(const int delay) const {
            // assume the list is sorted, first token must be lowest,
            // and thus we only need to check this token to the max constant.
            // also assume no place-object is "empty"
            return (tokens[0].getAge() + delay) >= (place->getMaxConstant() + 1);
        }
    };

    typedef std::vector<Place> PlaceList;

    class NonStrictMarkingBase {
    public:
        NonStrictMarkingBase();

        NonStrictMarkingBase(const TAPN::TimedArcPetriNet &tapn, const std::vector<int> &v);

        NonStrictMarkingBase(const NonStrictMarkingBase &nsm);

    public:
        friend std::ostream &operator<<(std::ostream &out, NonStrictMarkingBase &x);

        friend class DiscreteVerification;

        virtual ~NonStrictMarkingBase();

    public: // inspectors

        bool canDeadlock(const TAPN::TimedArcPetriNet &tapn, int maxDelay, bool ignoreCanDelay) const;

        inline bool canDeadlock(const TAPN::TimedArcPetriNet &tapn, const int maxDelay) const {
            return canDeadlock(tapn, maxDelay, false);
        };

        uint32_t numberOfTokensInPlace(int placeId) const;

        const TokenList &getTokenList(int placeId) const;

        inline const PlaceList &getPlaceList() const { return places; }

        inline PlaceList &getPlaceList() {
            return places;
        }

        uint32_t size();

        inline NonStrictMarkingBase *getParent() const { return parent; }

        inline const TAPN::TimedTransition *getGeneratedBy() const { return generatedBy; }

        bool equals(const NonStrictMarkingBase &m1) const;

        int cmp(const NonStrictMarkingBase &m1) const;

        inline int getNumberOfChildren() {
            return children;
        }

        virtual size_t getHashKey() const { return boost::hash_range(places.begin(), places.end()); };

    public: // modifiers

        int cut(std::vector<int> &);

        bool removeToken(int placeId, int age);

        bool removeToken(Place &place, Token &token);

        void addTokenInPlace(TAPN::TimedPlace &place, int age);

        void addTokenInPlace(Place &place, Token &token);

        void addTokenInPlace(const TAPN::TimedPlace &place, Token &token);

        inline void incrementAge() {
            for (auto &place : places) {
                place.incrementAge();
            }
        }

        inline void incrementAge(int age) {
            for (auto &place : places) {
                place.incrementAge(age);
            }
        }

        inline void decrementAge() {
            for (auto &place : places) {
                place.decrementAge();
            }
        }

        void removeRangeOfTokens(Place &place, TokenList::iterator begin, TokenList::iterator end);

        inline void setParent(NonStrictMarkingBase *parent) { this->parent = parent; }

        inline void setGeneratedBy(const TAPN::TimedTransition *generatedBy) { this->generatedBy = generatedBy; }

        inline void setNumberOfChildren(int i) {
            children = i;
        }

        inline void incrementNumberOfChildren() {
            ++children;
        }

        inline void decrementNumberOfChildren() {
            --children;
        }

        inline void cleanUp() {
            for (unsigned int i = 0; i < places.size(); i++) {
                if (places[i].tokens.empty()) {
                    places.erase(places.begin() + i);
                    i--;
                }
            }
        }

        int getYoungest();

        int makeBase();

        virtual NonStrictMarkingBase &Clone() {
            auto *clone = new NonStrictMarkingBase(*this);
            return *clone;
        };

    private:
        int children;
        PlaceList places;
        NonStrictMarkingBase *parent;
        const TAPN::TimedTransition *generatedBy;

        static TokenList emptyTokenList;
    };

    std::ostream &operator<<(std::ostream &out, NonStrictMarkingBase &x);

} } /* namespace VerifyTAPN */

#endif /* NonStrictMarkingBase_HPP_ */
