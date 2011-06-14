#include "VerificationOptions.hpp"
#include <iostream>

namespace VerifyTAPN {
	std::string enumToString(Trace trace){
		switch(trace)
		{
		case SOME:
			return "some";
		default:
			return "no";
		}
	}

	std::string SearchTypeEnumToString(SearchType s){
		switch(s){
		case COVERMOST:
			return "Cover Most Search";
		case RANDOM:
			return "Random Search";
		case DEPTHFIRST:
			return "Depth-First Search";
		default:
			return "Breadth-First Search";
		}
	}

	std::string FactoryEnumToString(Factory factory){
		switch(factory)
		{
		case OLD_FACTORY:
			return "old DBM";
		case DISCRETE_INCLUSION:
			return "discrete inclusion";
		default:
			return "default";
		}
	}


	std::ostream& operator<<(std::ostream& out, const VerificationOptions& options)
	{
		out << "Using " << SearchTypeEnumToString(options.GetSearchType()) << std::endl;
		out << "k-bound is: " << options.GetKBound() << std::endl;
		out << "Generating " << enumToString(options.GetTrace()) << " trace";
		if(options.GetTrace() != NONE) out << " in " << (options.XmlTrace() ? "xml format" : "human readable format");
		out << std::endl;
		out << "Symmetry Reduction is " << (options.GetSymmetryEnabled() ? "ON" : "OFF") << std::endl;
		out << "Untimed place optimization is " << (options.GetUntimedPlacesEnabled() ? "ON" : "OFF") << std::endl;
		out << "Using " << (options.GetGlobalMaxConstantsEnabled() ? "global maximum constant" : "local maximum constants") << " for extrapolation" << std::endl;
		out << "Using " << FactoryEnumToString(options.GetFactory()) << " marking factory" << std::endl;
		if(options.GetFactory() == DISCRETE_INCLUSION){
			out << "Considering the places ";
			bool first = true;
			for(std::vector<std::string>::const_iterator it = options.GetIncPlaces().begin(); it != options.GetIncPlaces().end(); it++)
			{
				if(!first) out << ", ";
				out << *it;
				first = false;
			}
			out << " for discrete inclusion." << std::endl;
		}
		out << "Model file is: " << options.GetInputFile() << std::endl;
		out << "Query file is: " << options.QueryFile() << std::endl;
		return out;
	}
}
