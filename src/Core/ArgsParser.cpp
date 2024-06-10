#include "Core/ArgsParser.hpp"

#include <iostream>
#include <iomanip>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

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
        size_t i = 0;
        std::set<size_t> res;
        for(; i < str.size(); ++i)
        {
            if(str[i] != ',') continue;
            std::string tmp = str.substr(last, i-last);
            auto n = parseInt(tmp);
            if(n <= 0)
            {
                std::cerr << "Query-indexes are 1-index, got a 0";
                std::exit(-1);
            }
            res.emplace(n-1); // 0-indexed in parser
            last = i+1; // skip comma
        }
        if(last < i)
        {
            std::string tmp = str.substr(last, i-last);
            auto n = parseInt(tmp);
            if(n <= 0)
            {
                std::cerr << "Query-indexes are 1-index, got a 0";
                std::exit(-1);
            }
            res.emplace(n-1); // 0-indexed in parser
        }
        return res;
    }


    VerificationOptions::SearchType toSearchType(const std::string& str) {
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


    void initialize(po::options_description& args, VerificationOptions& options) {
        args.add_options()
            ("help,h", "Displays this help messsage.")
            ("version,v", "Displays version information.")
            ("k-bound,k", po::value<uint32_t>(), "Max tokens to use during exploration.")
            ("xml-queries,x", po::value<std::string>(), "Parse XML query file and verify queries of a given comma-seperated list")
            ("search-strategy,s", po::value<std::string>(),
                "Specify the desired search strategy\n"
                 " BestFS\n"
                 " DFS\n"
                 " RDFS\n"
                 " MindelayFS\n"
                 " OverApprox\n"
                 " default")
            ("verification-method,m", po::value<uint32_t>(),
                "Specify the desired verification method.\n"
                    " 0: Discrete (default)\n "
                    " 1: Time Darts")
            ("memory-optimization,p", po::value<uint32_t>(),
                "Specify the desired memory optimization.\n"
                    " 0: None (default)\n"
                    " 1: PTrie")
            ("trace,t", po::value<uint32_t>(),
                "Specify the desired trace option.\n"
                  " 0: none (default)\n"
                  " 1: some\n"
                  " 2: fastest")
            ("keep-dead-tokens", "Do not discard dead tokens (used for boundedness checking)")
            ("global-max-constants", "Use global maximum constant for extrapolation (as opposed to local constants).")
            ("gcd-lower", "Enable lowering the guards by the greatest common divisor.")
            ("workflow,w", po::value<uint32_t>(),
                "Workflow mode.\n"
                  " 0: Disabled (default)\n"
                  " 1: Soundness (and min)\n"
                  " 2: Strong Soundness (and max)")
            ("strong-workflow-bound", po::value<size_t>(), "Maximum delay bound for strong workflow analysis")
            ("compute-cmax", "Calculate the place bounds.")
            ("disable-partial-order", "Disable partial order reduction")
            ("write-unfolded-net", po::value<std::string>(), "Outputs the model to the given file before structural reduction but after unfolding")
            ("bindings,b", "Print bindings to stderr in XML format (only for CPNs, default is not to print)")
            ("write-unfolded-queries", po::value<std::string>(), "Outputs the queries to the given file before query reduction but after unfolding")
            ("strategy-output", po::value<std::string>(), "File to write synthesized strategy to, use '_' (an underscore) for stdout")
            ("rate", po::value<float_t>(), "Default rate for unbounded transitions to use with SMC")
            ("confidence", po::value<float_t>(), "Confidence for SMC probability estimation")
            ("interval-width", po::value<float_t>(), "Interval width for SMC probability estimation")
            ("geq", po::value<float_t>(), "Performs a SMC test to compute if P(q) >= ARG")
            ("false-positives", po::value<float_t>(), "For probability comparison, probability of false positives")
            ("false-negatives", po::value<float_t>(), "For probability comparison, probability of false negatives")
            ("indifference", po::value<float_t>(), "For probability comparison, width of the indifference region")
            ("indifference-up", po::value<float_t>(), "For probability comparison, upper indifference region width")
            ("indifference-down", po::value<float_t>(), "For probability comparison, lower indifference region width");
    }


    VerificationOptions ArgsParser::parse(int argc, char *argv[]) {
        VerificationOptions opts;
        po::options_description args(VERIFYDTAPN_NAME " [options] model.pnml (query.xml)");
        initialize(args, opts);
        po::variables_map vm;

        auto parsed = po::command_line_parser(argc, argv).options(args).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);

        if(vm.count("help"))
        {
            std::cout << args << std::endl;
            std::exit(0);
        }

        if(vm.count("version"))
        {
            std::cout << "VerifyDTAPN " << VERIFYDTAPN_VERSION << std::endl;
            std::cout << "Licensed under BSD." << std::endl;
            std::exit(0);
        }

        if(vm.count("k-bound"))
            opts.setKBound(vm["k-bound"].as<uint32_t>());

        if(vm.count("xml-queries"))
            opts.setQueryNumbers(parseNumbers(vm["xml-queries"].as<std::string>()));

        if(vm.count("search-strategy"))
            opts.setSearchType(toSearchType(vm["search-strategy"].as<std::string>()));

        if(vm.count("verification-method"))
            opts.setVerificationType(toVerificationType(vm["verification-method"].as<uint32_t>()));

        if(vm.count("memory-optimization"))
            opts.setMemoryOptimization(toMemoryType(vm["memory-optimization"].as<uint32_t>()));

        if(vm.count("trace"))
            opts.setTrace(toTraceType(vm["trace"].as<uint32_t>()));

        if(vm.count("keep-dead-tokens"))
            opts.setKeepDeadTokens(true);

        if(vm.count("global-max-constants"))
            opts.setGlobalMaxConstantsEnabled(true);

        if(vm.count("gcd-lower"))
            opts.setGCDLowerGuardsEnabled(true);

        if(vm.count("workflow"))
            opts.setWorkflowMode(toWorkflowMode(vm["workflow"].as<uint32_t>()));

        if(vm.count("strong-workflow-bound"))
            opts.setWorkflowBound(vm["strong-workflow-bound"].as<size_t>());

        if(vm.count("calculate-cmax"))
            opts.setCalculateCmax(true);

        if(vm.count("disable-partial-order"))
            opts.setPartialOrderReduction(false);

        if(vm.count("write-unfolded-net"))
            opts.setOutputModelFile(vm["write-unfolded-net"].as<std::string>());

        if(vm.count("bindings"))
            opts.setPrintBindings(true);

        if(vm.count("write-unfolded-queries"))
            opts.setOutputQueryFile(vm["write-unfolded-queries"].as<std::string>());

        if(vm.count("strategy-output"))
            opts.setOutputModelFile(vm["strategy-output"].as<std::string>());

        std::vector<std::string> files = po::collect_unrecognized(parsed.options, po::include_positional);
        // remove everything that is just a space
        files.erase(std::remove_if(files.begin(), files.end(),
            [](auto& s ) {
                return std::all_of(s.begin(), s.end(),
                            [](auto c) {
                                return std::isspace(c);
                            });
            }
        ), files.end());
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
                std::cerr << "Expected exactly 1 trailing file (a model and a query), got [";
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
