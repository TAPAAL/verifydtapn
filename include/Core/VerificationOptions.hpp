#ifndef VERIFICATIONOPTIONS_HPP_
#define VERIFICATIONOPTIONS_HPP_

#include <string>
#include <iosfwd>
#include <utility>
#include <utility>
#include <vector>
#include <map>
#include <set>

namespace VerifyTAPN {

    class VerificationOptions {
    public:

        enum Trace {
            NO_TRACE, SOME_TRACE, FASTEST_TRACE
        };

        enum SearchType {
            BREADTHFIRST, DEPTHFIRST, RANDOM, COVERMOST, DEFAULT, MINDELAYFIRST, OverApprox
        };

        enum VerificationType {
            DISCRETE, TIMEDART
        };

        enum MemoryOptimization {
            NO_MEMORY_OPTIMIZATION, PTRIE
        };

        enum WorkflowMode {
            NOT_WORKFLOW, WORKFLOW_SOUNDNESS, WORKFLOW_STRONG_SOUNDNESS
        };

        VerificationOptions() = default;

    public: // inspectors

        const std::string& getInputFile() const {
            return inputFile;
        }

        void setInputFile(std::string input) {
            inputFile = std::move(input);
        }

        const std::string& getQueryFile() const {
            return queryFile;
        }

        void setQueryFile(std::string input) {
            queryFile = std::move(input);
        }

        const std::string& getOutputModelFile() const {
            return outputFile;
        }

        void setOutputModelFile(std::string input) {
            outputFile = std::move(input);
        }

        const std::string& getOutputQueryFile() const {
            return outputQuery;
        }

        void setQueryNumbers(std::set<size_t> numbers)
        {
            querynumbers = numbers;
        }

        const std::set<size_t>& getQueryNumbers() const {
            return querynumbers;
        }

        void setOutputQueryFile(std::string input) {
            outputQuery = std::move(input);
        }

        inline unsigned int getKBound() const {
            return k_bound;
        }

        void setKBound(unsigned int k) {
            k_bound = k;
        }

        inline Trace getTrace() const {
            return trace;
        };

        void setTrace(Trace t) {
            trace = t;
        };

        inline bool getXmlTrace() const {
            return xml_trace;
        };

        inline bool getGlobalMaxConstantsEnabled() const {
            return useGlobalMaxConstants;
        }

        inline void setGlobalMaxConstantsEnabled(bool c) {
            useGlobalMaxConstants = c;
        }

        inline SearchType getSearchType() const {
            return searchType;
        }

        inline void setSearchType(SearchType type) {
            searchType = type;
        }

        inline VerificationType getVerificationType() const {
            return verificationType;
        }

        inline void setVerificationType(VerificationType v) {
            verificationType = v;
        }

        inline MemoryOptimization getMemoryOptimization() const {
            return memOptimization;
        }

        inline void setMemoryOptimization(MemoryOptimization m) {
            memOptimization = m;
        }

        inline void setKeepDeadTokens(bool val) {
            keepDeadTokens = val;
        }

        inline bool getKeepDeadTokens() const {
            return keepDeadTokens;
        };

        inline void setGCDLowerGuardsEnabled(bool v) {
            enableGCDLowerGuards = v;
        }

        inline bool getGCDLowerGuardsEnabled() const {
            return enableGCDLowerGuards;
        }

        inline WorkflowMode getWorkflowMode() const {
            return workflow;
        };

        inline long long getWorkflowBound() const {
            return workflowBound;
        };

        inline void setWorkflowMode(WorkflowMode w) {
            workflow = w;
        };

        inline void setWorkflowBound(long long v) {
            workflowBound = v;
        };

        inline bool isWorkflow() const {
            return workflow != NOT_WORKFLOW;
        }

        inline bool getCalculateCmax() const {
            return calculateCmax;
        };

        inline void setCalculateCmax(bool v) {
            calculateCmax = v;
        };

        inline bool getPartialOrderReduction() const {
            return partialOrder;
        }

        inline void setPartialOrderReduction(bool v) {
            partialOrder = v;
        }

        inline const std::string& getStrategyFile() const {
            return strategy_output;
        }

        inline void setStrategyFile(const std::string& s) {
            strategy_output = s;
        }

    private:
        std::string inputFile;
        std::string queryFile;
        SearchType searchType = DEFAULT;
        VerificationType verificationType = DISCRETE;
        MemoryOptimization memOptimization = NO_MEMORY_OPTIMIZATION;
        unsigned int k_bound = 0;
        Trace trace  = NO_TRACE;
        bool xml_trace = true;
        bool useGlobalMaxConstants = false;
        bool keepDeadTokens = false;
        bool enableGCDLowerGuards = false;
        WorkflowMode workflow = NOT_WORKFLOW;
        long long workflowBound = 0;
        bool calculateCmax = false;
        bool partialOrder{};
        std::string outputFile;
        std::string outputQuery;
        std::set<size_t> querynumbers;
        std::string strategy_output = "";
    };

    std::ostream &operator<<(std::ostream &out, const VerificationOptions &options);
}

#endif /* VERIFICATIONOPTIONS_HPP_ */
