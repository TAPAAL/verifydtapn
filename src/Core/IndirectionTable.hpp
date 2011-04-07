#ifndef INDIRECTIONTABLE_HPP_
#define INDIRECTIONTABLE_HPP_

#include "../typedefs.hpp"

namespace VerifyTAPN
{
	class IndirectionTable
	{
	public:

	public:
		IndirectionTable() : map() { };
		explicit IndirectionTable(const BiMap& map) : map(map) { };
		virtual ~IndirectionTable() { };

	private:
		BiMap map;
	};
}

#endif /* INDIRECTIONTABLE_HPP_ */
