#ifndef VERIFICATIONOPTIONS_HPP_

#define VERIFICATIONOPTIONS_HPP_


#include <string>


namespace VerifyTAPN {
	enum Trace { NONE, SOME };
	enum SearchType { BREADTHFIRST, DEPTHFIRST };

	class VerificationOptions {
		private:
			VerificationOptions(
				const std::string& inputFile,
				const std::string& queryFile,
				SearchType searchType,
				int k_bound,
				bool symmetry,
				Trace trace,
				bool useUntimedPlaces,
				bool useGlobalMaxConstants,
				const std::string& workingdir
			) :
				inputFile(inputFile),
				queryFile(queryFile),
				searchType(searchType),
				k_bound(k_bound),
				symmetry(symmetry),
				trace(trace),
				useUntimedPlaces(useUntimedPlaces),
				useGlobalMaxConstants(useGlobalMaxConstants),
				workingdir(workingdir)
			{};

		public: // static
			static VerificationOptions ParseVerificationOptions(int argc, char* argv[]);

		public: // inspectors
			const std::string GetInputFile() const { return inputFile; }
			const std::string QueryFile() const {return queryFile; }
			inline const int GetKBound() const { return k_bound; }
			inline const Trace GetTrace() const { return trace; };
			inline const bool GetSymmetryEnabled() const { return symmetry; }
			inline const bool GetUntimedPlacesEnabled() const { return useUntimedPlaces; }
			inline const bool GetGlobalMaxConstantsEnabled() const { return useGlobalMaxConstants; }
			inline const std::string& GetWorkingDirPath() const { return workingdir; }
			inline const SearchType GetSearchType() const { return searchType; }
		private:
			std::string inputFile;
			std::string queryFile;
			SearchType searchType;
			int k_bound;
			bool symmetry;
			Trace trace;
			bool useUntimedPlaces;
			bool useGlobalMaxConstants;
			std::string workingdir;
	};

}

#endif /* VERIFICATIONOPTIONS_HPP_ */
