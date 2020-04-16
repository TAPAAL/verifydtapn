#include "Core/VerificationOptions.hpp"
#include <iostream>

namespace VerifyTAPN {

    std::string enumToString(VerificationOptions::Trace trace) {
        switch (trace) {
            case VerificationOptions::SOME_TRACE:
                return "some";
            case VerificationOptions::FASTEST_TRACE:
                return "fastest";
            default:
                return "no";
        }
    }

    std::string SearchTypeEnumToString(VerificationOptions::SearchType s) {
        switch (s) {
            case VerificationOptions::COVERMOST:
                return "Heuristic Search";
            case VerificationOptions::RANDOM:
                return "Random Search";
            case VerificationOptions::DEPTHFIRST:
                return "Depth-First Search";
            case VerificationOptions::BREADTHFIRST:
                return "Breadth-First Search";
            case VerificationOptions::MINDELAYFIRST:
                return "Minimum-Delay-First Search";
            default:
                return "Breadth-First Search";
        }
    }

    std::string VerificationTypeEnumToString(VerificationOptions::VerificationType s) {
        switch (s) {
            case VerificationOptions::TIMEDART:
                return "Time darts";
            default:
                return "Default (discrete)";
        }
    }

    std::string MemoryOptimizationEnumToString(VerificationOptions::MemoryOptimization m) {
        switch (m) {
            case VerificationOptions::NO_TRACE:
                return "None";
            case VerificationOptions::PTRIE:
                return "PTrie ";
            default:
                return "None";
        }
    }

    std::ostream &operator<<(std::ostream &out, const VerificationOptions &options) {
        out << "Search type: " << SearchTypeEnumToString(options.getSearchType()) << std::endl;
        out << "Verification method: " << VerificationTypeEnumToString(options.getVerificationType()) << std::endl;
        out << "Memory optimization: " << MemoryOptimizationEnumToString(options.getMemoryOptimization()) << std::endl;
        out << "Partial Order Reduction: " << (options.getPartialOrderReduction() ? "Enabled" : "Disabled")
            << std::endl;
        out << "k-bound is: " << options.getKBound() << std::endl;
        out << "Generating " << enumToString(options.getTrace()) << " trace";
        if (options.getTrace() != VerificationOptions::NO_TRACE) out << " in " << (options.getXmlTrace() ? "xml format"
                                                                                                         : "human readable format");
        out << std::endl;
        out << "Using "
            << (options.getGlobalMaxConstantsEnabled() ? "global maximum constant" : "local maximum constants")
            << " for extrapolation" << std::endl;
        if (options.isWorkflow()) {
            out << "Workflow analysis type : ";
            if (options.getWorkflowMode() == VerificationOptions::WORKFLOW_SOUNDNESS) {
                out << "Soundness" << std::endl;
            } else {
                out << "Strong soundness" << std::endl;
                out << "Bound is : " << options.getWorkflowBound() << std::endl;
            }
        }
        if (options.getCalculateCmax()) {
            out << "Calculating C-max" << std::endl;
        }
        out << "Model file is: " << options.getInputFile() << std::endl;
        if (options.getQueryFile() != "")
            out << "Query file is: " << options.getQueryFile() << std::endl;
        return out;
    }
}
