
#ifndef VERIFYYAPN_TAPN_TIMEDPLACE_HPP_
#define VERIFYYAPN_TAPN_TIMEDPLACE_HPP_

#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include "TimeInvariant.hpp"
#include "TimedInputArc.hpp"
#include "OutputArc.hpp"
#include "TransportArc.hpp"
#include "InhibitorArc.hpp"

namespace VerifyTAPN { namespace TAPN {
    enum PlaceType {
        Inv, Dead, Std
    };

    class TimedPlace {
    private:
        TimedPlace() : name(BOTTOM_NAME), timeInvariant(), index(BottomIndex()), untimed(false), maxConstant(0),
                       containsInhibitorArcs(false), inputArcs(), transportArcs(), inhibitorArcs() {};
    public: // static
        static const TimedPlace &Bottom() {
            static TimedPlace bottom;
            return bottom;
        }

        static int BottomIndex() {
            return -1;
        }

        static const std::string BOTTOM_NAME;

    public: // typedefs
        typedef std::vector<TimedPlace *> Vector;

    public: // construction / destruction
        TimedPlace(int index, std::string name, std::string id, const TimeInvariant &timeInvariant, double x = 0, double y = 0)
                : index(index), name(std::move(name)), id(std::move(id)), timeInvariant(timeInvariant), untimed(false),
                  maxConstant(0),
                  containsInhibitorArcs(false), inputArcs(), transportArcs(), inhibitorArcs(),
                  _position({x,y}) {};

        virtual ~TimedPlace() { /* empty */ };

    public: // modifiers
        inline void markPlaceAsUntimed() { untimed = true; }

        inline void setMaxConstant(int max) { maxConstant = max; }

        inline void addInputArc(TimedInputArc* inputArc) { inputArcs.push_back(inputArc); }

        inline void addTransportArc(TransportArc* transportArc) { transportArcs.push_back(transportArc); }

        inline void addProdTransportArc(TransportArc* transportArc) { prodTransportArcs.push_back(transportArc); }

        inline void addInhibitorArc(InhibitorArc* inhibitorArc) { inhibitorArcs.push_back(inhibitorArc); containsInhibitorArcs = true; }

        inline void addOutputArc(OutputArc* outputArc) { outputArcs.push_back(outputArc); }

        inline void setType(PlaceType type) { this->type = type; }

        void divideInvariantBy(int divider);

        const auto& getPosition() const { return _position; }

    public: // inspection
        inline PlaceType getType() const { return type; }

        const std::string &getName() const;

        const std::string &getId() const;

        void print(std::ostream &out) const;

        inline int getIndex() const { return index; };

        inline bool isUntimed() const { return untimed; }

        inline int getMaxConstant() const { return maxConstant; }

        inline const TAPN::TimeInvariant &getInvariant() const { return timeInvariant; };

        inline bool hasInhibitorArcs() const { return containsInhibitorArcs; };

        inline const TransportArc::Vector &getTransportArcs() const { return transportArcs; }

        inline const TransportArc::Vector &getProdTransportArcs() const { return prodTransportArcs; }

        inline const InhibitorArc::Vector &getInhibitorArcs() const { return inhibitorArcs; }

        inline const TimedInputArc::Vector &getInputArcs() const { return inputArcs; }

        inline const OutputArc::Vector &getOutputArcs() const { return outputArcs; }

    private: // data
        const int index;
        PlaceType type;
        std::string name;
        std::string id;
        TimeInvariant timeInvariant;
        bool untimed;
        int maxConstant;
        bool containsInhibitorArcs;
        TimedInputArc::Vector inputArcs{};
        TransportArc::Vector transportArcs{};
        TransportArc::Vector prodTransportArcs{};
        InhibitorArc::Vector inhibitorArcs{};
        OutputArc::Vector outputArcs{};
        std::pair<double,double> _position;
    };

    inline std::ostream &operator<<(std::ostream &out, const TimedPlace &place) {
        place.print(out);
        return out;
    }

} }
#endif /* VERIFYYAPN_TAPN_TIMEDPLACE_HPP_ */
