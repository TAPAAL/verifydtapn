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
		case DEPTHFIRST:
			return "Depth-First Search";
		default:
			return "Breadth-First Search";
		}
	}

	std::string FactoryEnumToString(Factory factory){
		switch(factory)
		{
		case DISCRETE_INCLUSION:
			return "discrete inclusion";
		case DEFAULT:
			return "default";
		}
	}


	std::ostream& operator<<(std::ostream& out, const VerificationOptions& options)
	{
		out << "k-bound is: " << options.GetKBound() << std::endl;
		out << "Generating " << enumToString(options.GetTrace()) << " trace";
		if(options.GetTrace() != NONE) std::cout << " in " << (options.XmlTrace() ? "xml format" : "human readable format");
		std::cout << std::endl;
		return out;
	}


	//VerificationOptions VerificationOptions::ParseVerificationOptions(std::map<std::string, unsigned int>& args)
	//{
//		bool error = false;
//
//		boost::program_options::options_description desc("Usage: verifytapn -k <number> [-t <number>] model-file query-file \nAllowed Options");
//		desc.add_options()
//				("help,h", "Show this help message")
//				("k-bound,k", boost::program_options::value<unsigned int>(), "Maximum number of tokens to use during exploration")
//				("search-type,o", boost::program_options::value<unsigned int>()->default_value(0), "Specify the desired search strategy. \n - 0: BFS\n - 1: DFS" )
//				("trace,t", boost::program_options::value<unsigned int>()->default_value(0), "Specify the desired trace option. \n - 0: none\n - 1: Some")
//				("global-max-constant,g", "Use a global max constant for extrapolation (as opposed to local constants)")
//				("disable-untimed-places,u", "Disable the untimed place optimization")
//				("disable-symmetry,s", "Disable symmetry reduction")
//				("factory,f", boost::program_options::value<unsigned int>()->default_value(0), "Specify marking factory.\n - 0: default\n - 1: discrete-inclusion")
//				("xml-trace,x","Output trace in xml format for reconstruction")
//				("model-file", boost::program_options::value<std::string>(), "model file")
//				("query-file", boost::program_options::value<std::string>(), "query file")
//		;
//
//		boost::program_options::positional_options_description p;
//		p.add("model-file", 1);
//		p.add("query-file", 2);
//
//		boost::program_options::variables_map vm;
//		boost::program_options::store(boost::program_options::command_line_parser(argc,argv).options(desc).positional(p).run(), vm);
//		boost::program_options::notify(vm);
//
//		//if(argc == 1 || vm.count("help")) {
//			std::cout << desc << "\n";
//			exit(0);
//		//}
//
//		SearchType search = intToSearchTypeEnum(vm["search-type"].as<unsigned int>());
//		std::cout << "Using " << SearchTypeEnumToString(search) << "\n";
//
//		if(vm.count("k-bound")) {
//			std::cout << "k-bound is: " << vm["k-bound"].as<unsigned int>() << "\n";
//		}
//		else {
//			std::cout << "Error! You must specify a bound k on the TAPN model" << "\n\n";
//			error = true;
//		}
//
//		Trace trace = intToEnum(vm["trace"].as<unsigned int>());
//		bool xml_trace = false;
//		if(vm.count("xml-trace") && trace != NONE)
//		{
//			xml_trace = true;
//		}
//
//		std::cout << "Generating " << enumToString(trace) << " trace";
//		if(trace != NONE) std::cout << " in " << (xml_trace ? "xml format" : "human readable format");
//		std::cout << std::endl;
//
//		bool symmetry = true;
//		if(vm.count("symmetry")) {
//			std::cout << "Symmetry Reduction is OFF\n";
//			symmetry = false;
//		}
//		else
//		{
//			std::cout << "Symmetry Reduction is ON\n";
//		}
//
//		bool untimedPlaces = true;
//		if(vm.count("untimed-places")) {
//			std::cout << "Untimed place optimization is OFF\n";
//			untimedPlaces = false;
//		}
//		else
//		{
//			std::cout << "Untimed place optimization is ON\n";
//		}
//
//		bool globalConstants = false;
//		if(vm.count("global-max-constant"))
//		{
//			std::cout << "Using global maximum constant for extrapolation\n";
//			globalConstants = true;
//		}
//		else
//		{
//			std::cout << "Using local maximum constants for extrapolation\n";
//			globalConstants = false;
//		}
//
//		Factory factory = intToFactory(vm["factory"].as<unsigned int>());
//		std::cout << "Using " << FactoryEnumToString(factory) << " marking factory" << std::endl;
//
//		if(vm.count("model-file")) {
//			std::cout << "model file is: " << vm["model-file"].as<std::string>() << "\n";
//		}
//		else{
//			std::cout << "Error! You must specify an model file" << "\n\n";
//			error = true;
//		}
//
//		if(vm.count("query-file")) {
//			std::cout << "query file is: " << vm["query-file"].as<std::string>() << "\n";
//		}
//		else{
//			std::cout << "Error! You must specify an query file" << "\n\n";
//			error = true;
//		}
//
//		if(error) {
//			std::cout << desc << "\n";
//			exit(0);
//		}

//		return VerificationOptions("","", BREADTHFIRST, 0, false, NONE, false, false, false, DEFAULT);
//	}
}
