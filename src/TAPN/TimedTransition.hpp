#ifndef VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_
#define VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_

#include <string>
#include <vector>
using std::string;

namespace VerifyTAPN {
	namespace TAPN {

		class TimedTransition
		{
		public: // typedefs
			typedef std::vector<TimedTransition> Vector; // Probably need to be pointers?
		public:
			TimedTransition(const string& name) : name(name) { };
			virtual ~TimedTransition() { /* empty */ }

		public: // inspectors
			const string& getName() const;

		private: // data
			const string name;
		};
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_ */
