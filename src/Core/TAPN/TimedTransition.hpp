#ifndef VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_
#define VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_

#include <string>
#include <vector>
#include "TimedInputArc.hpp"
#include "TransportArc.hpp"
#include "InhibitorArc.hpp"
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
			TimedTransition(const std::string& name, const std::string& id, bool urgent) : name(name), id(id), preset(), postset(), transportArcs(), index(-1), untimedPostset(true), urgent(urgent) { };
			TimedTransition() : name("*EMPTY*"), id("-1"), preset(), postset(), transportArcs(), index(-1), untimedPostset(true),urgent(false) { };
			virtual ~TimedTransition() { /* empty */ }

		public: // modifiers
			void addToPreset(const boost::shared_ptr<TimedInputArc>& arc);
			void addToPostset(const boost::shared_ptr<OutputArc>& arc);
			void addTransportArcGoingThrough(const boost::shared_ptr<TransportArc>& arc);
			void addIncomingInhibitorArc(const boost::shared_ptr<InhibitorArc>& arc);

			inline void setIndex(int i) { index = i; };
		public: // inspectors
			inline const std::string& getName() const { return name; };
			inline const std::string& getId() const { return id; };
			void print(std::ostream&) const;
			inline const TimedInputArc::WeakPtrVector& getPreset() const { return preset; }
			inline const TransportArc::WeakPtrVector& getTransportArcs() const { return transportArcs; }
			inline const InhibitorArc::WeakPtrVector& getInhibitorArcs() const { return inhibitorArcs; }
			inline const unsigned int getPresetSize() const { return getNumberOfInputArcs() + getNumberOfTransportArcs(); }
			inline const OutputArc::WeakPtrVector& getPostset() const { return postset; }
			inline const unsigned int getPostsetSize() const { return postset.size() + transportArcs.size(); }
			inline unsigned int getNumberOfInputArcs() const { return preset.size(); };
			inline unsigned int getNumberOfTransportArcs() const { return transportArcs.size(); };

			inline const bool isConservative() const { return preset.size() == postset.size(); }
			inline unsigned int getIndex() const { return index; }
			inline const bool hasUntimedPostset() const { return untimedPostset; }
			inline void setUntimedPostset(bool untimed){ untimedPostset = untimed; }
                        inline const bool isUrgent() const {
                            return urgent;
                        }

		private: // data
			std::string name;
			std::string id;
			TimedInputArc::WeakPtrVector preset;
			OutputArc::WeakPtrVector postset;
			TransportArc::WeakPtrVector transportArcs;
			InhibitorArc::WeakPtrVector inhibitorArcs;
			unsigned int index;
			bool untimedPostset;
                        bool urgent;
		};

		inline std::ostream& operator<<(std::ostream& out, const TimedTransition& transition)
		{
			transition.print(out);
			return out;
		}

		// TAPAAL does not allow multiple places with the same name,
		// thus it is enough to use the name to determine equality.
		inline bool operator==(TimedTransition const& a, TimedTransition const& b)
		{
			return a.getName() == b.getName();
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_ */
