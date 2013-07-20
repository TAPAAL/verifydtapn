/*
 * NonStrictMarkingBase.hpp
 *
 *  Created on: 29/02/2012
 *      Author: MathiasGS
 */

#ifndef NonStrictMarkingBase_HPP_
#define NonStrictMarkingBase_HPP_

#include <assert.h>
#include <vector>
#include "boost/functional/hash.hpp"
#include <iostream>
#include "../../Core/TAPN/TAPN.hpp"
#include <iostream>

using namespace std;

namespace VerifyTAPN {
namespace DiscreteVerification {

class Place;
class Token;
typedef vector<Token> TokenList;

class Token {
private:
	int age;
	int count;
public:
	Token(int age, int count) : age(age), count(count) {  };
	Token(const Token& t) : age(t.age), count(t.count) {  };

	inline bool equals(const Token &t) const { return (this->age == t.age && this->count == t.count); };

	inline void add(int num){ count = count + num; };
	inline int getCount() const { return count; };
	inline int getAge() const { return age; };
	inline void setAge(int i) { age = i; };
	inline void setCount(int i) { count = i; };
	inline void remove(int num){ count = count - num; };

	// Ages all tokens by 1
	inline void incrementAge(){
		age++;
	}

	inline void incrementAge(int x){
		age += x;
	}

	inline void decrementAge(){
		age--;
	}

	friend std::size_t hash_value(Token const& t)
	{
		size_t seed = 0;
		boost::hash_combine(seed, t.getAge());
		boost::hash_combine(seed, t.getCount());
		return seed;
	}
};

class Place {
public:
	const TAPN::TimedPlace* place;
	TokenList tokens;

	Place(const TAPN::TimedPlace* place) : place(place){};
	Place(const Place& p) : place(p.place){
		for(TokenList::const_iterator it = p.tokens.begin(); it != p.tokens.end(); it++){
			tokens.push_back(*it);
		}
	};

	friend std::size_t hash_value(Place const& p)
	{
		std::size_t seed = boost::hash_range(p.tokens.begin(), p.tokens.end());
		boost::hash_combine(seed, p.place->getIndex());

		return seed;
	}

	inline int numberOfTokens() const{
		int count = 0;
		for(TokenList::const_iterator iter = tokens.begin(); iter != tokens.end(); iter++){
			count += iter->getCount();
		}
		return count;
	}

	inline int maxTokenAge() const{
		int max = -1;
		for(TokenList::const_iterator iter = tokens.begin(); iter != tokens.end(); iter++){
			if(iter->getAge() > max) max = iter->getAge();
		}
		return max;
	}

	// Ages all tokens by 1
	inline void incrementAge(){
		for(TokenList::iterator iter = tokens.begin(); iter != tokens.end(); iter++){
			iter->incrementAge();
		}
	}

	inline void incrementAge(int age){
		for(TokenList::iterator iter = tokens.begin(); iter != tokens.end(); iter++){
			iter->incrementAge(age);
		}
	}

	inline void decrementAge(){
		for(TokenList::iterator iter = tokens.begin(); iter != tokens.end(); iter++){
			iter->decrementAge();
		}
	}
        inline bool allMaximumConstant(const int delay) const {                                       
            // assume the list is sorted, first token must be lowest, 
            // and thus we only need to check this token to the max constant.
            // also assume no place-object is "empty"
            return (tokens[0].getAge() + delay) >= (place->getMaxConstant() + 1);
        }
};

typedef vector<Place> PlaceList;

class NonStrictMarkingBase {
public:
	NonStrictMarkingBase();
	NonStrictMarkingBase(const TAPN::TimedArcPetriNet& tapn, const std::vector<int>& v);
	NonStrictMarkingBase(const NonStrictMarkingBase& nsm);

public:
	friend std::ostream& operator<<(std::ostream& out, NonStrictMarkingBase& x );
	friend class DiscreteVerification;

	virtual ~NonStrictMarkingBase();

	public: // inspectors

                const bool canDeadlock(const TAPN::TimedArcPetriNet& tapn, const int maxDelay, bool ignoreCanDelay) const;
                inline const bool canDeadlock(const TAPN::TimedArcPetriNet& tapn, const int maxDelay) const {
                    return canDeadlock(tapn, maxDelay, false);
                };
		int numberOfTokensInPlace(int placeId) const;
		const TokenList& getTokenList(int placeId) const;
		inline const PlaceList& getPlaceList() const{ return places; }
		unsigned int size();
		inline const NonStrictMarkingBase* getParent() const { return parent; }
		inline const TAPN::TimedTransition* getGeneratedBy() const { return generatedBy; }
		bool equals(const NonStrictMarkingBase &m1) const;
                inline int getNumberOfChildren(){
                    return children;
                }
                virtual size_t getHashKey() const { return boost::hash_range(places.begin(), places.end()); };
                
	public: // modifiers

                int cut();
		bool removeToken(int placeId, int age);
		bool removeToken(Place& place, Token& token);
		void addTokenInPlace(TAPN::TimedPlace& place, int age);
		void addTokenInPlace(Place& place, Token& token);
		void addTokenInPlace(const TAPN::TimedPlace& place, Token& token);
                inline void incrementAge(){
                        for(PlaceList::iterator iter = places.begin(); iter != places.end(); iter++){
                                iter->incrementAge();
                        }
                }

                inline void incrementAge(int age){
                        for(PlaceList::iterator iter = places.begin(); iter != places.end(); iter++){
                                iter->incrementAge(age);
                        }
                }

                inline void decrementAge(){
                        for(PlaceList::iterator iter = places.begin(); iter != places.end(); iter++){
                                iter->decrementAge();
                        }
                }
		void removeRangeOfTokens(Place& place, TokenList::iterator begin, TokenList::iterator end);
		inline void setParent(NonStrictMarkingBase* parent) { this->parent = parent; }
		inline void setGeneratedBy(const TAPN::TimedTransition* generatedBy) { this->generatedBy = generatedBy; }
                inline void setNumberOfChildren(int i){
                    children = i;
                }
                inline  void incrementNumberOfChildren(){
                    ++children;
                }
                inline void decrementNumberOfChildren(){
                    --children;
                }
		inline void cleanUp() {
			for(unsigned int i = 0; i < places.size(); i++){
				if(places[i].tokens.empty()){
					places.erase(places.begin()+i);
					i--;
				}
			}
		}
                int getYoungest();
		int makeBase(TAPN::TimedArcPetriNet* tapn);
                virtual NonStrictMarkingBase& Clone()
                {
                        NonStrictMarkingBase* clone = new NonStrictMarkingBase(*this);
                        return *clone;
                };

	private:
		int children;
		PlaceList places;
		NonStrictMarkingBase* parent;
		const TAPN::TimedTransition* generatedBy;

        static TokenList emptyTokenList;
};

std::ostream& operator<<(std::ostream& out, NonStrictMarkingBase& x);

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */

#endif /* NonStrictMarkingBase_HPP_ */
