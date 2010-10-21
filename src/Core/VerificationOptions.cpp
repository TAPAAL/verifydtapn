#include "VerificationOptions.hpp"
#include <iostream>
#include "boost/program_options.hpp"

namespace VerifyTAPN {

	const std::string VerificationOptions::GetInputFile() const
	{
		return inputFile;
	}

	const std::string VerificationOptions::QueryFile() const
	{
		return queryFile;
	}

	const int VerificationOptions::GetKBound() const
	{
		return k_bound;
	}

	VerificationOptions VerificationOptions::ParseVerificationOptions(int argc, char* argv[])
	{
		bool error = false;

		boost::program_options::options_description desc("Usage: verifytapn -k <number> model-file query-file \nAllowed Options:");
		desc.add_options()
				("help,h", "Produce help message")
				("k-bound,k", boost::program_options::value<int>(), "Specify the bound of the TAPN model")
				("model-file", boost::program_options::value<std::string>(), "model file")
				("query-file", boost::program_options::value<std::string>(), "query file")
		;

		boost::program_options::positional_options_description p;
		p.add("model-file", 1);
		p.add("query-file", 2);

		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::command_line_parser(argc,argv).options(desc).positional(p).run(), vm);
		boost::program_options::notify(vm);

		if(vm.count("help")) {
			std::cout << desc << "\n";
			exit(0);
		}

		if(vm.count("k-bound")) {
			std::cout << "k-bound is: " << vm["k-bound"].as<int>() << "\n";
		}
		else {
			std::cout << "Error! You must specify a bound k on the TAPN model" << "\n\n";
			error = true;

		}

		if(vm.count("model-file")) {
			std::cout << "model file is: " << vm["model-file"].as<std::string>() << "\n";
		}
		else{
			std::cout << "Error! You must specify an model file" << "\n\n";
			error = true;
		}

		if(vm.count("query-file")) {
			std::cout << "query file is: " << vm["query-file"].as<std::string>() << "\n";
		}
		else{
			std::cout << "Error! You must specify an query file" << "\n\n";
			error = true;
		}

		if(error) {
			std::cout << desc << "\n";
			exit(0);
		}

		return VerificationOptions(vm["model-file"].as<std::string>(), vm["query-file"].as<std::string>(), vm["k-bound"].as<int>());
	}

}
