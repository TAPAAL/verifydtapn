#include "DiscretePartInclusionMarking.hpp"
#include <iostream>
#include "dbm/print.h"

namespace VerifyTAPN
{
	void DiscretePartInclusionMarking::Print(std::ostream& out) const
	{
		out << "eq: ";
		for(unsigned int i = 0; i < eq.size(); i++)
		{
			out << eq[i] << ", ";
		}
		out << std::endl;
		out << "inc (placeIndex:count): ";
		for(unsigned int i = 0; i < inc.size(); i++)
		{
			out << i << ":" << inc[i] << ", ";
		}
		out << std::endl;
		out << "Mapping (token:clock): ";
		for(unsigned int i = 0; i < mapping.size(); i++)
		{
			out << i << ":" << mapping.GetMapping(i) << ", ";
		}
		out << std::endl;
		out << "DBM:" << std::endl;
		out << dbm;
	}
}
