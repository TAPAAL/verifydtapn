#ifndef VERIFICATIONOPTIONS_HPP_
#define VERIFICATIONOPTIONS_HPP_

#include <string>
#include <iosfwd>
#include <vector>

namespace VerifyTAPN {

    class VerificationOptions {
    public:

        enum Trace {
            NONE, SOME
        };

        enum SearchType {
            BREADTHFIRST, DEPTHFIRST, RANDOM, COVERMOST
        };

        enum VerificationType {
            DISCRETE, TIMEDART
        };

        enum MemoryOptimization {
            NO_MEMORY_OPTIMIZATION, PTRIE
        };

        VerificationOptions() {
        }

        VerificationOptions(
                const std::string& inputFile,
                const std::string& queryFile,
                SearchType searchType,
                VerificationType verificationType,
                MemoryOptimization memOptimization,
                unsigned int k_bound,
                Trace trace,
                bool xml_trace,
                bool useGlobalMaxConstants,
                bool keepDeadTokens
                ) : inputFile(inputFile),
        queryFile(queryFile),
        searchType(searchType),
        verificationType(verificationType),
        memOptimization(memOptimization),
        k_bound(k_bound),
        trace(trace),
        xml_trace(xml_trace),
        useGlobalMaxConstants(useGlobalMaxConstants),
        keepDeadTokens(keepDeadTokens) {

        };

    public: // inspectors

        const std::string getInputFile() const {
            return inputFile;
        }

        const std::string getQueryFile() const {
            return queryFile;
        }

        inline const unsigned int getKBound() const {
            return k_bound;
        }

        inline const Trace getTrace() const {
            return trace;
        };

        inline const bool getXmlTrace() const {
            return xml_trace;
        };

        inline const bool getGlobalMaxConstantsEnabled() const {
            return useGlobalMaxConstants;
        }

        inline const SearchType getSearchType() const {
            return searchType;
        }

        inline const VerificationType getVerificationType() const {
            return verificationType;
        }

        inline const MemoryOptimization getMemoryOptimization() const {
            return memOptimization;
        }

        inline const bool getKeepDeadTokens() const {
            return keepDeadTokens;
        };
    private:
        std::string inputFile;
        std::string queryFile;
        SearchType searchType;
        VerificationType verificationType;
        MemoryOptimization memOptimization;
        unsigned int k_bound;
        Trace trace;
        bool xml_trace;
        bool useGlobalMaxConstants;
        bool keepDeadTokens;
    };

    std::ostream& operator<<(std::ostream& out, const VerificationOptions& options);
}

#endif /* VERIFICATIONOPTIONS_HPP_ */
