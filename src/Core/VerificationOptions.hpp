#ifndef VERIFICATIONOPTIONS_HPP_
#define VERIFICATIONOPTIONS_HPP_

#include <string>
#include <iosfwd>
#include <vector>

namespace VerifyTAPN {
	enum Trace { NONE, SOME };
	enum SearchType { BREADTHFIRST, DEPTHFIRST, RANDOM, COVERMOST };
	enum VerificationType { DISCRETE, TIMEDART };
        enum MemoryOptimization { NO_MEMORY_OPTIMIZATION, PDATA };

	class VerificationOptions {
		public:
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
			) :	inputFile(inputFile),
				queryFile(queryFile),
				searchType(searchType),
				verificationType(verificationType),
                                memOptimization(memOptimization),
				k_bound(k_bound),
				trace(trace),
				xml_trace(xml_trace),
				useGlobalMaxConstants(useGlobalMaxConstants),
				keepDeadTokens(keepDeadTokens)
			{ 
                        
                        };

		public: // inspectors
			const std::string GetInputFile() const { return inputFile; }
			const std::string QueryFile() const {return queryFile; }
			inline const unsigned int GetKBound() const { return k_bound; }
			inline const Trace GetTrace() const { return trace; };
			inline const bool XmlTrace() const { return xml_trace; };
			inline const bool GetGlobalMaxConstantsEnabled() const { return useGlobalMaxConstants; }
			inline const SearchType GetSearchType() const { return searchType; }
			inline const VerificationType GetVerificationType() const { return verificationType; }
                        inline const MemoryOptimization GetMemoryOptimization() const { return memOptimization; }
			inline const bool GetKeepDeadTokens() const { return keepDeadTokens; };
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
