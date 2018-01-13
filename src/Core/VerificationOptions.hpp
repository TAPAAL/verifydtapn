#ifndef VERIFICATIONOPTIONS_HPP_
#define VERIFICATIONOPTIONS_HPP_

#include <string>
#include <iosfwd>
#include <vector>
#include <map>

namespace VerifyTAPN {

    class VerificationOptions {
    public:

        enum Trace {
            NO_TRACE, SOME_TRACE, FASTEST_TRACE
        };

        enum SearchType {
            BREADTHFIRST, DEPTHFIRST, RANDOM, COVERMOST, DEFAULT, MINDELAYFIRST
        };

        enum VerificationType {
            DISCRETE, TIMEDART
        };

        enum MemoryOptimization {
            NO_MEMORY_OPTIMIZATION, PTRIE
        };

        enum WorkflowMode{
        	NOT_WORKFLOW, WORKFLOW_SOUNDNESS, WORKFLOW_STRONG_SOUNDNESS
        };

        VerificationOptions() {
        }

        VerificationOptions(
                SearchType searchType,
                VerificationType verificationType,
                MemoryOptimization memOptimization,
                unsigned int k_bound,
                Trace trace,
                bool xml_trace,
                bool useGlobalMaxConstants,
                bool keepDeadTokens, 
                bool enableGCDLowerGuards,
                WorkflowMode workflow,
                long long workflowBound,
		bool calculateCmax,
                std::map<std::string, int> replace,
                bool order
                ) : inputFile(""),
        queryFile(""),
        searchType(searchType),
        verificationType(verificationType),
        memOptimization(memOptimization),
        k_bound(k_bound),
        trace(trace),
        xml_trace(xml_trace),
        useGlobalMaxConstants(useGlobalMaxConstants),
        keepDeadTokens(keepDeadTokens), 
        enableGCDLowerGuards(enableGCDLowerGuards),
        workflow(workflow),
        workflowBound(workflowBound),
        calculateCmax(calculateCmax),
        replace(replace),
        partialOrder(order)
        {
        };

    public: // inspectors

        const std::string getInputFile() const {
            return inputFile;
        }

        void setInputFile(std::string input) {
            inputFile = input;
        }
        
        const std::string getQueryFile() const {
            return queryFile;
        }
        
        void setQueryFile(std::string input) {
            queryFile = input;
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
        
        inline void setSearchType(SearchType type){
            searchType = type;
        }

        inline const VerificationType getVerificationType() const {
            return verificationType;
        }

        inline const MemoryOptimization getMemoryOptimization() const {
            return memOptimization;
        }

        inline void setKeepDeadTokens(bool val)
        {
            keepDeadTokens = val;
        }
        
        inline const bool getKeepDeadTokens() const {
            return keepDeadTokens;
        };

        inline const bool getGCDLowerGuardsEnabled() const {
            return enableGCDLowerGuards;
        }

        inline const WorkflowMode getWorkflowMode() const {
            return workflow;
        };

        inline const long long getWorkflowBound() const {
            return workflowBound;
        };

        inline bool isWorkflow() const{
            return workflow != NOT_WORKFLOW;
        }

        inline const bool getCalculateCmax() const {
            return calculateCmax;
        };
        
        inline const std::map<std::string, int>& getReplacements() const {
            return replace;
        }
        
        inline bool getPartialOrderReduction() const
        {
            return partialOrder;
        }



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
        bool enableGCDLowerGuards;
        WorkflowMode workflow;
        long long workflowBound;
        bool calculateCmax;
        std::map<std::string, int> replace;
        bool partialOrder;
    };

    std::ostream& operator<<(std::ostream& out, const VerificationOptions& options);
}

#endif /* VERIFICATIONOPTIONS_HPP_ */
