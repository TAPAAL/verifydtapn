#ifndef CONCRETEMARKING_HPP_
#define CONCRETEMARKING_HPP_

#include <iosfwd>
#include <deque>
#include <vector>
#include <string>
#include <algorithm>
#include "assert.h"
#include "../../typedefs.hpp"

namespace VerifyTAPN
{
	class Token{
		friend bool operator==(const Token&, const Token&);
	public:
		Token(const std::string& place) : place(place), age(0) {};
		Token(const std::string& place, decimal age) : place(place), age(age) {};
		inline void Reset() { age = decimal(0); }
		inline void Delay(decimal amount) { age += amount; }
		inline void SetPlace(const std::string& place) { this->place = place; }
		inline const std::string& Place() const { return place; };
		inline decimal Age() const { return age; };
		std::ostream& Print(std::ostream& out) const;
	private:
		std::string place;
		decimal age;
	};

	inline std::ostream& operator<<( std::ostream& out, const Token& token )
	{
		return token.Print(out);
	}

	inline bool operator==( const Token& a, const Token& b)
	{
		return a.place == b.place && a.age == b.age;
	}

	inline bool operator!=( const Token& a, const Token& b)
	{
		return !(a == b);
	}


	class ConcreteMarking{
		friend std::ostream& operator<<( std::ostream&, const ConcreteMarking& );
	public:
		ConcreteMarking(const std::deque<Token>& tokens) : tokens(tokens) { };

		inline void Delay(decimal amount)
		{
			for(std::deque<Token>::iterator iter = tokens.begin(); iter != tokens.end(); ++iter)
				iter->Delay(amount);
		}

		inline Token& operator[](unsigned int i)
		{
			assert(i < tokens.size());
			return tokens[i];
		}

		inline unsigned int Size() const { return tokens.size(); };
		std::ostream& Print(std::ostream& out) const;
	private:
		std::deque<Token> tokens;
	};

	inline std::ostream& operator<<( std::ostream& out, const ConcreteMarking& marking )
	{
		return marking.Print(out);
	}
}

#endif /* CONCRETEMARKING_HPP_ */
