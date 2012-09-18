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
			return "Heuristic Search";
		case RANDOM:
			return "Random Search";
		case DEPTHFIRST:
			return "Depth-First Search";
		default:
			return "Breadth-First Search";
		}
	}

	std::string VerificationTypeEnumToString(VerificationType s){
			switch(s){
			case TIMEDART:
				return "Time darts";
			default:
				return "Default (discrete)";
			}
		}

	std::ostream& operator<<(std::ostream& out, const VerificationOptions& options)
	{
		out << "Search type: " << SearchTypeEnumToString(options.GetSearchType()) << std::endl;
		out << "Verification method: " << VerificationTypeEnumToString(options.GetVerificationType()) << std::endl;
		out << "k-bound is: " << options.GetKBound() << std::endl;
		out << "Generating " << enumToString(options.GetTrace()) << " trace";
		if(options.GetTrace() != NONE) out << " in " << (options.XmlTrace() ? "xml format" : "human readable format");
		out << std::endl;
		out << "Using " << (options.GetGlobalMaxConstantsEnabled() ? "global maximum constant" : "local maximum constants") << " for extrapolation" << std::endl;
		out << "Model file is: " << options.GetInputFile() << std::endl;
		out << "Query file is: " << options.QueryFile() << std::endl;
		return out;
	}
}
