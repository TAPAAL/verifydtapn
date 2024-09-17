#ifndef VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_
#define VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_

#include <string>
#include <utility>
#include <vector>
#include "TimedInputArc.hpp"
#include "TransportArc.hpp"
#include "InhibitorArc.hpp"
#include "OutputArc.hpp"
#include "Core/Query/SMCQuery.hpp"
#include "StochasticStructure.hpp"

namespace VerifyTAPN {

    using SMC::Distribution;
    using SMC::FiringMode;

    class SymMarking;

    namespace TAPN {
        class TimedArcPetriNet;

        class TimedTransition {
        public: // typedefs
            typedef std::vector<TimedTransition *> Vector;
        public:
            TimedTransition(int index, std::string name, std::string id, bool urgent, bool controllable, double x, double y, Distribution distrib = Distribution::defaultDistribution(), double weight = 1, FiringMode firingMode = SMC::Oldest)
                    : index(index), name(std::move(name)), id(std::move(id)), preset(), postset(), transportArcs(),
                      untimedPostset(true),
                      urgent(urgent), controllable(controllable), _position({x,y}), 
                      _distribution(distrib), _weight(weight), _firingMode(firingMode) {};

            TimedTransition() : name("*EMPTY*"), id("-1"), preset(), postset(), transportArcs(), index(-1),
                                untimedPostset(true), urgent(false) {};

            virtual ~TimedTransition() { /* empty */ }

        public: // modifiers
            void addToPreset(TimedInputArc* arc);

            void addToPostset(OutputArc* arc);

            void addTransportArcGoingThrough(TransportArc* arc);

            void addIncomingInhibitorArc(InhibitorArc* arc);

        public: // inspectors
            inline const std::string &getName() const { return name; };

            inline const std::string &getId() const { return id; };

            void print(std::ostream &) const;

            const inline TimedInputArc::Vector &getPreset() const { return preset; }

            const inline TransportArc::Vector &getTransportArcs() const { return transportArcs; }

            const inline InhibitorArc::Vector &getInhibitorArcs() const { return inhibitorArcs; }

            inline unsigned int getPresetSize() const {
                return getNumberOfInputArcs() + getNumberOfTransportArcs();
            }

            const inline OutputArc::Vector &getPostset() const { return postset; }

            inline unsigned int getPostsetSize() const { return postset.size() + transportArcs.size(); }

            inline unsigned int getNumberOfInputArcs() const { return preset.size(); };

            inline unsigned int getNumberOfTransportArcs() const { return transportArcs.size(); };

            inline bool isConservative() const { return preset.size() == postset.size(); }

            inline unsigned int getIndex() const { return index; }

            inline bool hasUntimedPostset() const { return untimedPostset; }

            inline void setUntimedPostset(bool untimed) { untimedPostset = untimed; }

            inline bool isUrgent() const {
                return urgent;
            }

            inline bool isControllable() const {
                return controllable;
            }

            inline bool isEnvironment() const {
                return !isControllable();
            }

            inline void setControllable(bool value) {
                controllable = value;
            }

            uint32_t getProduced(const TimedPlace* place) const
            {
                // this could be precomputed
                for(auto& pre : getPostset())
                    if(&pre->getOutputPlace() == place)
                        return pre->getWeight();
                for(auto& pre : getTransportArcs())
                    if(&pre->getDestination() == place)
                        return pre->getWeight();
                return 0;
            }

            uint32_t getConsumed(const TimedPlace* place) const
            {
                // this could be precomputed
                for(auto& pre : getPreset())
                    if(&pre->getInputPlace() == place)
                        return pre->getWeight();
                for(auto& pre : getTransportArcs())
                    if(&pre->getSource() == place)
                        return pre->getWeight();
                return 0;
            }

            const auto& getPosition() const {
                return _position;
            }

            const Distribution& getDistribution() const {
                return _distribution;
            }

            inline void setDistribution(Distribution distrib) {
                _distribution = distrib;
            }

            const double& getWeight() const {
                return _weight;
            }

            inline void setWeight(double weight) {
                _weight = weight;
            }

            const FiringMode& getFiringMode() const {
                return _firingMode;
            }

            inline void setFiringMode(FiringMode firingMode) {
                _firingMode = firingMode;
            }

        private: // data
            const int index = 0;
            std::string name;
            std::string id;
            TimedInputArc::Vector preset;
            OutputArc::Vector postset;
            TransportArc::Vector transportArcs;
            InhibitorArc::Vector inhibitorArcs;
            bool untimedPostset = false;
            bool urgent = false;
            bool controllable{};
            std::pair<double,double> _position;
            Distribution _distribution;
            double _weight;
            FiringMode _firingMode = SMC::Oldest;
        };

        inline std::ostream &operator<<(std::ostream &out, const TimedTransition &transition) {
            transition.print(out);
            return out;
        }
    }
}

#endif /* VERIFYTAPN_TAPN_TIMEDTRANSITION_HPP_ */
