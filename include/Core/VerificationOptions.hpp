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

        inline void setPrintBindings(bool val) {
            printBindings = val;
        }
       
        inline bool getPrintBindings() const {
            return printBindings;
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

        inline float getSmcConfidence() const {
            return smcConfidence;
        }

        inline void setSmcConfidence(float conf) {
            smcConfidence = conf;
        }

        inline float getSmcIntervalWidth() const {
            return smcIntervalWidth;
        }

        inline void setSmcIntervalWidth(float width) {
            smcIntervalWidth = width;
        }

        inline float getDefaultRate() const {
            return defaultRate;
        }

        inline void setDefaultRate(float rate) {
            defaultRate = rate;
        }

        inline float getFalsePositives() const {
            return falsePositives;
        }

        inline void setFalsePositives(float proba) {
            falsePositives = proba;
        }

        inline float getFalseNegatives() const {
            return falseNegatives;
        }

        inline void setFalseNegatives(float proba) {
            falseNegatives = proba;
        }

        inline float getTargetProbability() const {
            return targetProbability;
        }

        inline void setTargetProbability(float proba) {
            targetProbability = proba;
        }

        inline float getIndifferenceUp() const {
            return indifferenceUp;
        }

        inline void setIndifferenceUp(float proba) {
            indifferenceUp = proba;
        }

        inline float getIndifferenceDown() const {
            return indifferenceDown;
        }

        inline void setIndifferenceDown(float proba) {
            indifferenceDown = proba;
        }

    protected:
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
        bool printBindings = false;
        WorkflowMode workflow = NOT_WORKFLOW;
        long long workflowBound = 0;
        bool calculateCmax = false;
        bool partialOrder{};
        std::string outputFile;
        std::string outputQuery;
        std::set<size_t> querynumbers;
        std::string strategy_output = "";
        float smcConfidence = 0.95;
        float smcIntervalWidth = 0.05;
        float defaultRate = 0.1;
        float falsePositives = 0.1;
        float falseNegatives = 0.1;
        float targetProbability = -1;
        float indifferenceUp = 0.1;
        float indifferenceDown = 0.1;
        friend class ArgsParser;
    };

    std::ostream &operator<<(std::ostream &out, const VerificationOptions &options);
}

#endif /* VERIFICATIONOPTIONS_HPP_ */
