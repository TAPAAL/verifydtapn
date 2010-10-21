#ifndef VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_
#define VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_

#include <string>
#include <vector>
#include "TimedInputArc.hpp"
#include "OutputArc.hpp"
#include "boost/shared_ptr.hpp"

namespace VerifyTAPN {

class SymMarking;

	namespace TAPN {
		class TimedArcPetriNet;

		class TimedTransition
		{
		public: // typedefs
			typedef std::vector< boost::shared_ptr<TimedTransition> > Vector;
		public:
			TimedTransition(const std::string& name, const std::string& id) : name(name), id(id) { };
			TimedTransition() : name("*EMPTY*"), id("-1") { };
			virtual ~TimedTransition() { /* empty */ }

		public: // modifiers
			void AddToPreset(const boost::shared_ptr<TimedInputArc>& arc);
			void AddToPostset(const boost::shared_ptr<OutputArc>& arc);

		public: // inspectors
			const std::string& GetName() const;
			const std::string& GetId() const;
			void Print(std::ostream&) const;
			const TimedInputArc::WeakPtrVector& GetPreset() const { return preset; }
			const unsigned int GetPresetSize() const { return preset.size(); }
			const OutputArc::WeakPtrVector& GetPostset() const { return postset; }
			const unsigned int GetPostsetSize() const { return postset.size(); }
		//	bool isEnabledBy(const TimedArcPetriNet& tapn, const VerifyTAPN::SymMarking& marking) const;
			const bool isConservative() const { return preset.size() == postset.size(); }

		private: // data
			std::string name;
			std::string id;
			TimedInputArc::WeakPtrVector preset;
			OutputArc::WeakPtrVector postset;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedTransition& transition)
		{
			transition.Print(out);
			return out;
		}

		// TAPAAL does not allow multiple places with the same name,
		// thus it is enough to use the name to determine equality.
		inline bool operator==(TimedTransition const& a, TimedTransition const& b)
		{
			return a.GetName() == b.GetName();
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_ */
