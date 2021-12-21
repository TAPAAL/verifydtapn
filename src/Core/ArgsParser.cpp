#include "Core/ArgsParser.hpp"

#include <iostream>
#include <iomanip>

#include <argparse/argparse.hpp>

namespace VerifyTAPN {

    // TODO clean this up, move to boost and move this code to
    // the options directly

    ArgsParser::ArgsParser() { };

    uint64_t parseInt(const std::string& str)
    {
        if(std::any_of(str.begin(), str.end(), [](char a) { return !std::isdigit(a); }))
        {
            std::cout << "Not a number: " << str << std::endl;
        }
        std::stringstream ss(str);
        uint64_t a;
        ss >> a;
        return a;
    }

    std::set<size_t> parseNumbers(const std::string& str)
    {
        size_t last = 0;
        std::set<size_t> res;
        for(size_t i = 0; i < str.size(); ++i)
        {
            if(str[i] != ',') continue;
            std::string tmp = str.substr(last, i-last);
            auto n = parseInt(tmp);
            if(n <= 0)
            {
                std::cerr << "Query-indexes are 1-index, got a 0";
                std::exit(-1);
            }
            res.emplace(n);
            last = i+1; // skip comma
        }
        return res;
    }


    VerificationOptions::SearchType toSearchType(const std::string& str) {
        if(std::any_of(str.begin(), str.end(), [](char a) { return !std::isdigit(a); }))
        {
            // to be backwards compatible w. gui
            switch(parseInt(str))
            {
                case 0:
                    return VerificationOptions::BREADTHFIRST;
                case 1:
                    return VerificationOptions::DEPTHFIRST;
                case 2:
                    return VerificationOptions::RANDOM;
                case 3:
                    return VerificationOptions::COVERMOST;
                case 4:
                    return VerificationOptions::DEFAULT;
                default:
                    std::cout << "Unknown search strategy '" << str << "' specified." << std::endl;
                    std::exit(-1);
            }
        }
        else
        {
            if(str == "BestFS")
                return VerificationOptions::COVERMOST;
            if(str == "BFS")
                return VerificationOptions::BREADTHFIRST;
            if(str == "DFS")
                return VerificationOptions::DEPTHFIRST;
            if(str == "RDFS")
                return VerificationOptions::RANDOM;
            if(str == "MindelayFS")
                return VerificationOptions::MINDELAYFIRST;
            if(str == "DEFAULT" || str == "default")
                return VerificationOptions::DEFAULT;
            if(str == "OverApprox")
                return VerificationOptions::OverApprox;
            std::cout << "Unknown search strategy '" << str << "' specified." << std::endl;
            std::exit(1);
        }
    }

    VerificationOptions::VerificationType toVerificationType(unsigned int i) {
        switch (i) {
            case 0:
                return VerificationOptions::DISCRETE;
            case 1:
                return VerificationOptions::TIMEDART;
            default:
                std::cout << "Unknown verification method specified." << std::endl;
                std::exit(1);
        }
    }

    VerificationOptions::MemoryOptimization toMemoryType(int i) {
        switch (i) {
            case 0:
                return VerificationOptions::NO_MEMORY_OPTIMIZATION;
            case 1:
                return VerificationOptions::PTRIE;
            default:
                std::cout << "Unknown memory optimization specified." << std::endl;
               std::exit(1);
        }
    }

    VerificationOptions::Trace toTraceType(int i) {
        switch (i) {
            case 0:
                return VerificationOptions::NO_TRACE;
            case 1:
                return VerificationOptions::SOME_TRACE;
            case 2:
                return VerificationOptions::FASTEST_TRACE;
            default:
                std::cout << "Unknown trace option specified." << std::endl;
                std::exit(1);
        }
    }

    VerificationOptions::WorkflowMode toWorkflowMode(int i) {
        switch (i) {
            case 0:
                return VerificationOptions::NOT_WORKFLOW;
            case 1:
                return VerificationOptions::WORKFLOW_SOUNDNESS;
            case 2:
                return VerificationOptions::WORKFLOW_STRONG_SOUNDNESS;
            default:
                std::cout << "Unknown workflow option specified." << std::endl;
               std::exit(1);
        }
    }


    void ArgsParser::initialize(argparse::ArgumentParser& args, VerificationOptions& options) {

        args.add_argument("-h", "--help")
            .action([&](const std::string& s) {
              std::cout << args.help().str();
              std::exit(0);
            })
            .help("Displays this help messsage.")
            .implicit_value(true)
            .nargs(0);
        args.add_argument("-v", "--version")
            .action([=](const std::string& s) {
                    std::cout << "VerifyDTAPN " << VERIFYDTAPN_VERSION << std::endl;
                    std::cout << "Licensed under BSD." << std::endl;
                    std::exit(0);
                })
                .help("Displays version information.")
                .implicit_value(true)
                .nargs(0);

        args.add_argument("-k", "--k-bound")
            .help("Max tokens to use during exploration.")
            .action([&options](const std::string& s) {
                options.setKBound(parseInt(s));
            })
            .nargs(1);

        args.add_argument("-x", "--xml-queries")
            .help("Parse XML query file and verify queries of a given comma-seperated list")
            .action([&options](const std::string& s) {
                options.setQueryNumbers(parseNumbers(s));
            })
            .nargs(1);

        args.add_argument("-s", "--search-strategy")
            .help("Specify the desired search strategy\n"
                  "\t\t\t\t- BestFS\n"
                  "\t\t\t\t- DFS\n"
                  "\t\t\t\t- RDFS\n"
                  "\t\t\t\t- MindelayFS\n"
                  "\t\t\t\t- OverApprox\n"
                  "\t\t\t\t- default")
            .action([&options](const std::string& s) {
                options.setSearchType(toSearchType(s));
            })
            .nargs(1);

        args.add_argument("-m", "--verification-method")
            .help("Specify the desired verification method.\n"
                    "\t\t\t\t- 0: Discrete (default)\n "
                    "\t\t\t\t- 1: Time Darts")
            .action([&options](const std::string& s) {
                options.setVerificationType(toVerificationType(parseInt(s)));
            })
            .nargs(1);

        args.add_argument(("-p", "--memory-optimization"))
            .help("Specify the desired memory optimization.\n"
                    "\t\t\t\t- 0: None (default)\n"
                    "\t\t\t\t- 1: PTrie")
            .action([&options](const std::string& s) {
                options.setMemoryOptimization(toMemoryType(parseInt(s)));
            })
           .nargs(1);

        args.add_argument("-t", "--trace")
            .help("Specify the desired trace option.\n"
                  "\t\t\t\t- 0: none (default)\n"
                  "\t\t\t\t- 1: some\n"
                  "\t\t\t\t- 2: fastest")
            .action([&options](const std::string& s) {
                options.setTrace(toTraceType(parseInt(s)));
            })
            .nargs(1);

        args.add_argument("--keep-dead-tokens")
            .action([&](auto&) {
                options.setKeepDeadTokens(true);
            })
            .nargs(0);

        args.add_argument("--global-max-constants")
            .action([&](auto&) {
                options.setGlobalMaxConstantsEnabled(true);
            })
            .nargs(0);

        args.add_argument("--gcd-lower")
            .action([&](auto&) {
                options.setGCDLowerGuardsEnabled(true);
            })
            .nargs(0);

        args.add_argument("-w", "--workflow")
            .help("Workflow mode.\n"
                  "\t\t\t\t- 0: Disabled (default)\n"
                  "\t\t\t\t- 1: Soundness (and min)\n"
                  "\t\t\t\t- 2: Strong Soundness (and max)")
            .action([&options](const std::string& s) {
                options.setWorkflowMode(toWorkflowMode(parseInt(s)));
            })
            .nargs(1);

        args.add_argument("-b", "--strong-workflow-bound")
            .help("Maximum delay bound for strong workflow analysis")
            .action([&options](const std::string& s) {
                options.setWorkflowBound(parseInt(s));
            })
            .nargs(1);

        args.add_argument("--compute-cmax")
            .help("Calculate the place bounds (and exit)")
            .action([&](auto&) {
                options.setCalculateCmax(true);
            })
            .nargs(0);

        args.add_argument("--disable-partial-order")
            .help("Disable partial order reduction")
            .action([&](auto&) {
                options.setPartialOrderReduction(false);
            })
            .nargs(0);

        args.add_argument("--write-unfolded-net")
            .help("Outputs the model to the given file before structural reduction but after unfolding")
            .action([&options](const std::string& s) {
                options.setOutputModelFile(s);
            })
            .nargs(1);

        args.add_argument("--write-unfolded-queries")
            .help("Outputs the queries to the given file before query reduction but after unfolding")
            .action([&options](const std::string& s) {
                options.setOutputQueryFile(s);
            })
            .nargs(1);

        args.add_argument("--strategy-output")
            .help("File to write synthesized strategy to, use '_' (an underscore) for stdout")
            .action([&options](const std::string& s) {
                options.setStrategyFile(s);
            })
            .nargs(1);
        args.add_argument("files")
            .help("A model-file followed by a query-file. The query-file must be omitted for workflow analysis)")
            .remaining();
    }


    VerificationOptions ArgsParser::parse(int argc, char *argv[]) {
        VerificationOptions opts;
        argparse::ArgumentParser args("VerifyDTAPN", VERIFYDTAPN_VERSION, argparse::default_arguments::none);
        initialize(args, opts);
        try {
            args.parse_args(argc, argv);
        } catch (const std::runtime_error& err) {
            std::cerr << err.what() << std::endl;
            std::cerr << args;
            std::exit(-1);
        }

        auto files = args.get<std::vector<std::string>>("files");
        if (opts.getWorkflowMode() == VerificationOptions::WORKFLOW_SOUNDNESS ||
            opts.getWorkflowMode() == VerificationOptions::WORKFLOW_STRONG_SOUNDNESS) {
            if(files.size() != 1)
            {
                std::cerr << "Expected exactly 1 trailing file (the model) for workflow verification, got [";
                for(size_t i = 0; i < files.size(); ++i)
                {
                    if(i != 0) std::cerr << ", ";
                    std::cerr << files[i];
                }
                std::cerr << "]" << std::endl;
                std::exit(-1);
            }
            opts.setInputFile(files[0]);
        }
        else
        {
            if(files.size() != 2)
            {
                std::cerr << "Expected exactly 1 trailing file (a model and a query)";
                for(size_t i = 0; i < files.size(); ++i)
                {
                    if(i != 0) std::cerr << ", ";
                    std::cerr << files[i];
                }
                std::cerr << "]" << std::endl;
                std::exit(-1);
            }
            opts.setInputFile(files[0]);
            opts.setQueryFile(files[1]);
        }

        return opts;
    }
}
