#ifndef VERIFICATIONOPTIONS_HPP_
#define VERIFICATIONOPTIONS_HPP_


#include <string>


namespace VerifyTAPN {
	class VerificationOptions {
		private:
			VerificationOptions(std::string inputFile, std::string queryFile, int k_bound, bool symmetry) : inputFile(inputFile), queryFile(queryFile), k_bound(k_bound), symmetry(symmetry) {};

		public: // static
			static VerificationOptions ParseVerificationOptions(int argc, char* argv[]);

		public: // inspectors
			const std::string GetInputFile() const { return inputFile; }
			const std::string QueryFile() const {return queryFile; }
			const int GetKBound() const { return k_bound; }
			const bool GetSymmetry() const { return symmetry; }
		private:
			std::string inputFile;
			std::string queryFile;
			int k_bound;
			bool symmetry;
	};

}

#endif /* VERIFICATIONOPTIONS_HPP_ */
