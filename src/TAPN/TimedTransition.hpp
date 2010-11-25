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
			TimedTransition(const std::string& name, const std::string& id) : name(name), id(id), preset(), postset(), index(-1) { };
			TimedTransition() : name("*EMPTY*"), id("-1"), preset(), postset(), index(-1) { };
			virtual ~TimedTransition() { /* empty */ }

		public: // modifiers
			void AddToPreset(const boost::shared_ptr<TimedInputArc>& arc);
			void AddToPostset(const boost::shared_ptr<OutputArc>& arc);

			inline void SetIndex(int i) { index = i; };
		public: // inspectors
			inline const std::string& GetName() const { return name; };
			inline const std::string& GetId() const { return id; };
			void Print(std::ostream&) const;
			inline const TimedInputArc::WeakPtrVector& GetPreset() const { return preset; }
			inline const unsigned int GetPresetSize() const { return preset.size(); }
			inline const OutputArc::WeakPtrVector& GetPostset() const { return postset; }
			inline const unsigned int GetPostsetSize() const { return postset.size(); }
		//	bool isEnabledBy(const TimedArcPetriNet& tapn, const VerifyTAPN::SymMarking& marking) const;
			inline const bool isConservative() const { return preset.size() == postset.size(); }
			inline unsigned int GetIndex() const { return index; };

		private: // data
			std::string name;
			std::string id;
			TimedInputArc::WeakPtrVector preset;
			OutputArc::WeakPtrVector postset;
			unsigned int index;
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
