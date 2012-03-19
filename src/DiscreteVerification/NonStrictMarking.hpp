/*
 * NonStrictMarking.hpp
 *
 *  Created on: 29/02/2012
 *      Author: MathiasGS
 */

#ifndef NONSTRICTMARKING_HPP_
#define NONSTRICTMARKING_HPP_

#include <assert.h>
#include <vector>
#include "boost/functional/hash.hpp"
#include <iostream>

using namespace std;

namespace VerifyTAPN {
namespace DiscreteVerification {

class Token {
private:
	int age;
	int count;
public:
	Token(int age, int count) : age(age), count(count) {  };

	bool equals(const Token &t){ return (this->age == t.age && this->count == t.count); };

	void add(int num){ count = count + num; };
	int getCount() const { return count; };
	int getAge() const { return age; };
	void remove(int num){ count = count - num; };

	// Ages all tokens by 1
	void incrementAge(){
		age++;
	}

	friend std::size_t hash_value(Token const& t)
		{
			size_t seed = 0;
			boost::hash_combine(seed, t.getAge());
			boost::hash_combine(seed, t.getCount());
			return seed;
		}
};

typedef vector<Token> TokenList;

class Place {
	public:
		int id;
		TokenList tokens;

	Place(int id) : id(id){};

	friend std::size_t hash_value(Place const& p)
	{
		std::size_t seed = boost::hash_range(p.tokens.begin(), p.tokens.end());
		boost::hash_combine(seed, p.id);

		return seed;
	}

	// Ages all tokens by 1
	void incrementAge(){
		for(TokenList::iterator iter = tokens.begin(); iter != tokens.end(); iter++){
			iter->incrementAge();
		}
	}
};

typedef vector<Place> PlaceList;

class NonStrictMarking {
public:
	NonStrictMarking();
	NonStrictMarking(const std::vector<int>& v);
	NonStrictMarking(const NonStrictMarking& nsm) { places = nsm.places; };

public:
	friend std::ostream& operator<<(std::ostream& out, NonStrictMarking& x );
	friend class DiscreteVerification;

	virtual ~NonStrictMarking();

	virtual size_t HashKey() const { return boost::hash_range(places.begin(), places.end()); };

	virtual NonStrictMarking& Clone()
		{
			NonStrictMarking* clone = new NonStrictMarking(*this);
			return *clone;
		};

	public: // inspectors
		int NumberOfTokensInPlace(Place& palce) const;
		const PlaceList& GetTokenPlacementVector() { return places; }
		unsigned int size();


	public: // modifiers
		bool RemoveToken(Place& place, Token& token);
		void AddTokenInPlace(Place& place, Token& token);
		void incrementAge();	// increment

	public:
		bool equals(const NonStrictMarking &m1);

	public:
		PlaceList places;
};

std::ostream& operator<<(std::ostream& out, NonStrictMarking& x);

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */

#endif /* NONSTRICTMARKING_HPP_ */
