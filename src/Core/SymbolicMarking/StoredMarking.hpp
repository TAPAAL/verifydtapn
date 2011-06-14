#ifndef STOREDMARKING_HPP_
#define STOREDMARKING_HPP_

#include "DiscretePart.hpp"
#include <iosfwd>

namespace VerifyTAPN
{
	enum relation { DIFFERENT = 0, SUPERSET = 1, SUBSET = 2, EQUAL = 3 };

	class StoredMarking
	{
	public:
		virtual ~StoredMarking() { };

		virtual relation Relation(const StoredMarking& other) const = 0;
		virtual size_t HashKey() const = 0;

		virtual void Print(std::ostream& out) const = 0;
	};

	std::ostream& operator<<(std::ostream& out, const StoredMarking& m);
}

#endif /* STOREDMARKING_HPP_ */
