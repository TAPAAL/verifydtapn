#ifndef VERIFICATIONOPTIONS_HPP_
#define VERIFICATIONOPTIONS_HPP_


#include <string>


namespace VerifyTAPN {
	class VerificationOptions {
		private:
			VerificationOptions(std::string inputFile, std::string queryFile, int k_bound) : inputFile(inputFile), queryFile(queryFile), k_bound(k_bound) {};

		public: // static
			static VerificationOptions ParseVerificationOptions(int argc, char* argv[]);

		public: // inspectors
			const std::string GetInputFile() const;
			const std::string QueryFile() const;
			const int GetKBound() const;
		private:
			std::string inputFile;
			std::string queryFile;
			int k_bound;
	};

}

#endif /* VERIFICATIONOPTIONS_HPP_ */
