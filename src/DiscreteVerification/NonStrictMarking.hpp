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

namespace VerifyTAPN {
namespace DiscreteVerification {

struct Token{
	int place;
	int age;

	friend std::size_t hash_value(Token const& p)
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, p.place);
		boost::hash_combine(seed, p.age);

		return seed;
	}
};

class NonStrictMarking {
public:
	typedef std::vector<Token> Vector;
public:
	NonStrictMarking();
	NonStrictMarking(const std::vector<int>& v);
	NonStrictMarking(const NonStrictMarking* nsm) : placement(nsm->placement) { };

	friend std::ostream& operator<<(std::ostream& out, NonStrictMarking& x );

	virtual ~NonStrictMarking();

	virtual size_t HashKey() const { return boost::hash_range(placement.begin(), placement.end()); };

	virtual NonStrictMarking& Clone() const
		{
			NonStrictMarking* clone = new NonStrictMarking(*this);
			return *clone;
		};

	public: // inspectors
		inline int GetTokenPlacement(unsigned int tokenIndex) const { return placement[tokenIndex].place; }
		int NumberOfTokensInPlace(int placeIndex) const;
		const std::vector<Token>& GetTokenPlacementVector() const { return placement; }
		inline const unsigned int size() const { return placement.size(); }


	public: // modifiers
		void MoveToken(unsigned int tokenIndex, int newPlaceIndex);
		void RemoveToken(int index) { placement.erase(placement.begin() + index); }
		void AddTokenInPlace(Token placeIndex) { placement.push_back(placeIndex); }

	public:
		bool equals(const NonStrictMarking &m1);

private:
	Vector placement;
};

std::ostream& operator<<(std::ostream& out, NonStrictMarking& x );

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */

#endif /* NONSTRICTMARKING_HPP_ */
