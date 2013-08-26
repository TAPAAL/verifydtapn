#ifndef VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_
#define VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_

#include <string>
#include <vector>
#include "TimedInputArc.hpp"
#include "TransportArc.hpp"
#include "InhibitorArc.hpp"
#include "OutputArc.hpp"

namespace VerifyTAPN {

class SymMarking;

	namespace TAPN {
		class TimedArcPetriNet;

		class TimedTransition
		{
		public: // typedefs
			typedef std::vector<TimedTransition*> Vector;
		public:
			TimedTransition(const std::string& name, const std::string& id, bool urgent) : name(name), id(id), preset(), postset(), transportArcs(), index(-1), untimedPostset(true), urgent(urgent) { };
			TimedTransition() : name("*EMPTY*"), id("-1"), preset(), postset(), transportArcs(), index(-1), untimedPostset(true),urgent(false) { };
			virtual ~TimedTransition() { /* empty */ }

		public: // modifiers
			void addToPreset(TimedInputArc& arc);
			void addToPostset(OutputArc& arc);
			void addTransportArcGoingThrough(TransportArc& arc);
			void addIncomingInhibitorArc(InhibitorArc& arc);

			inline void setIndex(int i) { index = i; };
		public: // inspectors
			inline const std::string& getName() const { return name; };
			inline const std::string& getId() const { return id; };
			void print(std::ostream&) const;
			inline TimedInputArc::Vector& getPreset() { return preset; }
			const inline TimedInputArc::Vector& getPreset() const { return preset; }
			inline TransportArc::Vector& getTransportArcs() { return transportArcs; }
			const inline TransportArc::Vector& getTransportArcs() const { return transportArcs; }
			inline InhibitorArc::Vector& getInhibitorArcs() { return inhibitorArcs; }
			const inline InhibitorArc::Vector& getInhibitorArcs() const { return inhibitorArcs; }
			inline const unsigned int getPresetSize() const { return getNumberOfInputArcs() + getNumberOfTransportArcs(); }
			inline OutputArc::Vector& getPostset() { return postset; }
			const inline OutputArc::Vector& getPostset() const { return postset; }
			inline const unsigned int getPostsetSize() const { return postset.size() + transportArcs.size(); }
			inline const unsigned int getNumberOfInputArcs() const { return preset.size(); };
			inline const unsigned int getNumberOfTransportArcs() const { return transportArcs.size(); };

			inline const bool isConservative() const { return preset.size() == postset.size(); }
			inline const unsigned int getIndex() const { return index; }
			inline const bool hasUntimedPostset() const { return untimedPostset; }
			inline void setUntimedPostset(bool untimed){ untimedPostset = untimed; }
                        inline const bool isUrgent() const {
                            return urgent;
                        }

		private: // data
			std::string name;
			std::string id;
			TimedInputArc::Vector preset;
			OutputArc::Vector postset;
			TransportArc::Vector transportArcs;
			InhibitorArc::Vector inhibitorArcs;
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
