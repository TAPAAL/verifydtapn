#ifndef STOREDMARKING_HPP_
#define STOREDMARKING_HPP_

#include "DiscretePart.hpp"

namespace VerifyTAPN
{
	enum relation { DIFFERENT = 0, SUPERSET = 1, SUBSET = 2, EQUAL = 3 };

	class StoredMarking
	{
	public:
		virtual ~StoredMarking() { };

		virtual relation Relation(const StoredMarking& other) const = 0;
		virtual size_t HashKey() const = 0;
		// virtual Save() // see below
		// virtual Load() // not sure what these should do exactly
	};
}

#endif /* STOREDMARKING_HPP_ */
