#include "ArgsParser.hpp"
#include <iostream>
#include <sstream>
#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/make_shared.hpp"
#include <iomanip>
#include "boost/tokenizer.hpp"

namespace VerifyTAPN {
	static const std::string KBOUND_OPTION = "k-bound";
	static const std::string SEARCH_OPTION = "search-type";
	static const std::string TRACE_OPTION = "trace";
	static const std::string MAX_CONSTANT_OPTION = "global-max-constants";
	static const std::string UNTIMED_PLACES_OPTION = "disable-untimed-places";
	static const std::string SYMMETRY_OPTION = "disable-symmetry";
	static const std::string FACTORY_OPTION = "factory";
	static const std::string XML_TRACE_OPTION = "xml-trace";
	static const std::string INCLUSION_PLACES = "inc-places";

	std::ostream& operator<<(std::ostream& out, const Switch& flag)
	{
		flag.Print(out);
		return out;
	}

	std::ostream& operator <<(std::ostream& out, const Version& version)
	{
		out << version.maj << "." << version.min << "." << version.build;
		return out;
	}

	void PrintIndentedDescription(std::ostream& out, const std::string& description)
	{
		typedef boost::tokenizer<boost::char_separator<char> > tokens;
		tokens lines(description.begin(), description.end(), boost::char_separator<char>("\n", "", boost::keep_empty_tokens));
		bool first = true;
		for(tokens::const_iterator it = lines.begin(); it != lines.end(); it++)
		{
			if(!first){
				out << std::setw(WIDTH) << " ";
			}
			out << *it << std::endl;
			first = false;
		}
	}

	void Switch::Print(std::ostream& out) const
	{
		std::stringstream s;
		s << "-" << ShortName();
		s << " [ --" << LongName() << " ]";
		out << std::setw(WIDTH) << std::left << s.str();
		PrintIndentedDescription(out, Description());
	};

	void SwitchWithArg::Print(std::ostream& out) const
	{
		std::stringstream s;
		s << "-" << ShortName();
		s << " [ --" << LongName() << " ] ";
		s << " arg (=" << default_value << ")";
		out << std::setw(WIDTH) << std::left << s.str();
		PrintIndentedDescription(out, Description());
	};

	void SwitchWithStringArg::Print(std::ostream& out) const
	{
		std::stringstream s;
		s << "-" << ShortName();
		s << " [ --" << LongName() << " ] ";
		s << " a1,a2,.. (=" << default_value << ")";
		out << std::setw(WIDTH) << std::left << s.str();
		PrintIndentedDescription(out, Description());
	};

	bool Switch::Handles(const std::string& flag) const
	{
		std::stringstream stream;
		stream << "-" << name << " ";
		if(flag.find(stream.str()) != std::string::npos) return true;
		if(flag.find(long_name) != std::string::npos) return true;
		return false;
	};

	option Switch::Parse(const std::string& flag)
	{
		assert(Handles(flag));
		handled_option = true;
		return option(LongName(), "1");
	};


	option SwitchWithArg::Parse(const std::string& flag)
	{
		assert(Handles(flag));
		handled_option = true;
		std::string copy(flag);
		std::stringstream stream;
		stream << "-" << ShortName() << " ";
		if(flag.find(stream.str()) != std::string::npos)
		{
			boost::erase_all(copy, stream.str());
		}
		else
		{
			std::stringstream long_name_stream;
			long_name_stream << "--" << LongName() << " ";
			boost::erase_all(copy, long_name_stream.str());
		}
		boost::trim(copy);
		return option(LongName(), copy);
	};

	option SwitchWithStringArg::Parse(const std::string& flag)
	{
		assert(Handles(flag));
		handled_option = true;
		std::string copy(flag);
		std::stringstream stream;
		stream << "-" << ShortName() << " ";
		if(flag.find(stream.str()) != std::string::npos)
		{
			boost::erase_all(copy, stream.str());
		}
		else
		{
			std::stringstream long_name_stream;
			long_name_stream << "--" << LongName() << " ";
			boost::erase_all(copy, long_name_stream.str());
		}
		boost::trim(copy);
		return option(LongName(), copy);
	};

	void ArgsParser::Initialize()
	{
		// NOTE: The Help() function only splits and indents descriptions based on newlines.
		//       Each line in the description is assumed to fit within the remaining width
		//       of the console, so keep descriptions short, or implement manual word-wrapping :).
		parsers.push_back(boost::make_shared<SwitchWithArg>("k", KBOUND_OPTION, "Max tokens to use during exploration.",0));
		parsers.push_back(boost::make_shared<SwitchWithArg>("o", SEARCH_OPTION, "Specify the desired search strategy.\n - 0: Breadth-First Search\n - 1: Depth-First Search\n - 2: Random Search\n - 3: Maximum Cover Search",3));
		parsers.push_back(boost::make_shared<SwitchWithArg>("t", TRACE_OPTION, "Specify the desired trace option.\n - 0: none\n - 1: some",0));

		parsers.push_back(boost::make_shared<Switch>("g",MAX_CONSTANT_OPTION, "Use global maximum constant for \nextrapolation (as opposed to local \nconstants)."));
		parsers.push_back(boost::make_shared<Switch>("u",UNTIMED_PLACES_OPTION, "Disables the untimed place optimization."));
		parsers.push_back(boost::make_shared<Switch>("s",SYMMETRY_OPTION, "Disables symmetry reduction."));

		parsers.push_back(boost::make_shared<Switch>("x",XML_TRACE_OPTION, "Output trace in xml format for TAPAAL."));

		parsers.push_back(boost::make_shared<SwitchWithArg>("f", FACTORY_OPTION, "Specify the desired marking factory.\n - 0: Default\n - 1: Discrete-inclusion\n - 2: Old factory",0));
		parsers.push_back(boost::make_shared<SwitchWithStringArg>("i", INCLUSION_PLACES, "Specify a list of places to consider \nfor discrete inclusion. No spaces after\nthe commas!\nSpecial values: *ALL*, *NONE*", "*ALL*"));
	};

	void ArgsParser::Help() const
	{
		std::cout << "Usage: verifytapn -k <number> [optional arguments] model-file query-file" << std::endl;
		std::cout << "Allowed Options:" << std::endl;
		std::cout << std::setw(WIDTH) << std::left << "-h [ --help ]" << "Displays this help message." << std::endl;
		std::cout << std::setw(WIDTH) << std::left << "-v [ --version ]" << "Displays version information." << std::endl;
		for(parser_vec::const_iterator arg = parsers.begin(); arg != parsers.end(); arg++)
		{
			std::cout << **arg;
		}
	};

	void ArgsParser::Version() const
	{
		std::cout << "VerifyTAPN " << version << std::endl;
		std::cout << "Licensed under GPL." << std::endl;
	}

	VerificationOptions ArgsParser::Parse(int argc, char* argv[]) const
	{
		if(argc == 1 || std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")
		{
			Help();
			exit(0);
		}

		if(std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version")
		{
			Version();
			exit(0);
		}

		if(argc < 3)
		{
			std::cout << "too few parameters." << std::endl;
			std::cout << "Use '-h' for help on correct usage." << std::endl;
			exit(1);
		}

		std::string model_file(argv[argc-2]);
		std::string query_file(argv[argc-1]);

		if(boost::iends_with(query_file,".xml"))
		{
			std::cout << "Missing query file." << std::endl;
			exit(1);
		}

		if(!boost::iends_with(model_file,".xml"))
		{
			std::cout << "Invalid model file specified." << std::endl;
			exit(1);
		}

		if(!boost::iends_with(query_file, ".q"))
		{
			std::cout << "Invalid query file specified." << std::endl;
			exit(1);
		}

		std::vector<std::string> flags;
		unsigned int i = 1;
		unsigned int size = static_cast<unsigned int>(argc);
		while(i < size-2)
		{
			std::string arg(argv[i]);
			if(boost::istarts_with(arg, "-"))
			{
				std::string arg2(argv[i+1]);
				if(!boost::istarts_with(arg2,"-") && i+1 < size-2)
				{
					flags.push_back(arg + " " + arg2);
					i++;
				}else{
					flags.push_back(arg + " ");
				}
			}
			i++;
		}

		option_map options;
		for(std::vector<std::string>::const_iterator flag = flags.begin(); flag != flags.end(); flag++)
		{
			bool handled = false;
			for(parser_vec::const_iterator it = parsers.begin(); it != parsers.end(); it++)
			{
				if((*it)->Handles(*flag))
				{
					options.insert((*it)->Parse(*flag));
					handled = true;
				}
			}
			if(!handled)
			{
				std::cout << "Unknown option flag '" << *flag << "'" << std::endl;
				std::cout << "Use '-h' to see a list of valid options." << std::endl;
				exit(1);
			}
		}

		// Put in default values for non-specified options
		for(parser_vec::const_iterator it = parsers.begin(); it != parsers.end(); it++)
		{
			if(!(*it)->HandledOption())
			{
				options.insert((*it)->DefaultOption());
			}
		}

		return CreateVerificationOptions(options, model_file, query_file);
	}

	SearchType intToSearchTypeEnum(int i) {
		switch(i){
		case 0:	return BREADTHFIRST;
		case 1:	return DEPTHFIRST;
		case 2: return RANDOM;
		case 3: return COVERMOST;
		default:
			std::cout << "Unknown search strategy specified." << std::endl;
			exit(1);
		}
	}

	Trace intToEnum(int i){
		switch(i)
		{
		case 0: return NONE;
		case 1:	return SOME;
		default:
			std::cout << "Unknown trace option specified." << std::endl;
			exit(1);
		}
	}

	Factory intToFactory(unsigned int i) {
		switch(i)
		{
		case 0: return DEFAULT;
		case 1:	return DISCRETE_INCLUSION;
		case 2:	return OLD_FACTORY;
		default:
			std::cout << "Unkown factory specified." << std::endl;
			exit(1);
		}
	}

	unsigned int ArgsParser::TryParseInt(const option& option) const
	{
		unsigned int result = 0;
		try
		{
			result = boost::lexical_cast<unsigned int>(option.second);
		}
		catch(boost::bad_lexical_cast & e)
		{
			std::cout << "Invalid value '" << option.second << "' for option '--" << option.first << "'" << std::endl;
			exit(1);
		}
		return result;
	}

	std::vector<std::string> ArgsParser::ParseIncPlaces(const std::string& string) const
	{
		std::vector<std::string> vec;
		boost::split( vec, string, boost::is_any_of(",") );
		return vec;
	}

	VerificationOptions ArgsParser::CreateVerificationOptions(const option_map& map, const std::string& modelFile, const std::string& queryFile) const
	{
		assert(map.find(KBOUND_OPTION) != map.end());
		unsigned int kbound = TryParseInt(*map.find(KBOUND_OPTION));

		assert(map.find(SEARCH_OPTION) != map.end());
		SearchType search = intToSearchTypeEnum(TryParseInt(*map.find(SEARCH_OPTION)));

		assert(map.find(TRACE_OPTION) != map.end());
		Trace trace = intToEnum(TryParseInt(*map.find(TRACE_OPTION)));

		assert(map.find(SYMMETRY_OPTION) != map.end());
		bool disable_symmetry = boost::lexical_cast<bool>(map.find(SYMMETRY_OPTION)->second);

		assert(map.find(MAX_CONSTANT_OPTION) != map.end());
		bool max_constant = boost::lexical_cast<bool>(map.find(MAX_CONSTANT_OPTION)->second);

		assert(map.find(UNTIMED_PLACES_OPTION) != map.end());
		bool disable_untimed_places = boost::lexical_cast<bool>(map.find(UNTIMED_PLACES_OPTION)->second);

		assert(map.find(FACTORY_OPTION) != map.end());
		Factory factory = intToFactory(TryParseInt(*map.find(FACTORY_OPTION)));

		assert(map.find(XML_TRACE_OPTION) != map.end());
		bool xml_trace = boost::lexical_cast<bool>(map.find(XML_TRACE_OPTION)->second);

		assert(map.find(INCLUSION_PLACES) != map.end());
		std::vector<std::string> inc_places = ParseIncPlaces(map.find(INCLUSION_PLACES)->second);
		return VerificationOptions(modelFile, queryFile, search, kbound, !disable_symmetry, trace, xml_trace, !disable_untimed_places, max_constant, factory, inc_places);
	}
}
