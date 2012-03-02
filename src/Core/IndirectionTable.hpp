#ifndef INDIRECTIONTABLE_HPP_
#define INDIRECTIONTABLE_HPP_

#include "../typedefs.hpp"
#include <iosfwd>

namespace VerifyTAPN
{
	class IndirectionTable
	{
	public:
		IndirectionTable() : map() { };
		explicit IndirectionTable(const BiMap& map) : map(map) { };
		virtual ~IndirectionTable() { };

		unsigned int MapForward(unsigned int i) const
		{
			return map.left.at(i);
		};
		unsigned int MapBackwards(unsigned int i) const
		{
			return map.right.at(i);
		};

		void AddMapping(unsigned int i, unsigned int j)
		{
			BiMap::value_type val(i,j);
			std::pair<BiMap::const_iterator, bool> success = map.insert(val);

			assert(success.second);
		};

		void RemoveMapping(unsigned int i, unsigned int j)
		{
			BiMap::iterator it = map.find(BiMap::value_type(i,j));
			assert(it != map.end());
			map.erase(it);
		};

		void Swap(IndirectionTable& table)
		{
			if(&table != this)
			{
				map.swap(table.map);
			}
		}

		unsigned int Size() const { return map.size(); };
		void Print(std::ostream& out) const;
	private:
		BiMap map;
	};

	std::ostream& operator<<(std::ostream& out, const IndirectionTable& t);
}

#endif /* INDIRECTIONTABLE_HPP_ */
