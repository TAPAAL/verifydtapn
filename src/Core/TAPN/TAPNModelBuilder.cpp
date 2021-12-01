#include "Core/TAPN/TAPNModelBuilder.hpp"

#include <string>
#include <algorithm>

namespace VerifyTAPN {

    void TAPNModelBuilder::addPlace(const std::string& name,
            int tokens,
            bool strict,
            int bound,
            double,
            double)
    {
        TimeInvariant timeInvariant = TimeInvariant(strict, bound);
        _places.emplace_back(new TimedPlace(name, name, timeInvariant));
        _initialMarking.emplace_back(tokens);

    }

    void TAPNModelBuilder::addTransition(const std::string &name, bool urgent,
                                        double, double)
    {
        _transitions.emplace_back(new TimedTransition(name, name, urgent, true));
    }

    void TAPNModelBuilder::addInputArc(const std::string &place_name,
            const std::string &transition_name,
            bool inhibitor,
            int weight,
            bool lstrict, bool ustrict, int lower, int upper)
    {
        auto place = find_place(place_name);
        auto transition = find_transition(transition_name);
        if(inhibitor)
        {
            if(lstrict != false ||
               ustrict != true ||
               lower != 0 ||
               upper != std::numeric_limits<typeof(upper)>::max())
            {
                std::cerr << "Inhibitor-arcs must have unrestricted guards! (between " << place_name
                    << " and " << transition_name << ")\n";
                std::cerr << std::endl;
                std::exit(-1);
            }
            _inhibitorArcs.emplace_back(new InhibitorArc(*place, *transition, weight));
        }
        else
        {
            _inputArcs.emplace_back(new TimedInputArc(*place, *transition, weight, TimeInterval(lstrict, lower, upper, ustrict)));
        }
    }

    void TAPNModelBuilder::addOutputArc(const std::string& transition_name,
            const std::string& place_name,
            int weight)
    {
        auto place = find_place(place_name);
        auto transition = find_transition(transition_name);
        _outputArcs.emplace_back(new OutputArc(*transition, *place, weight));

    }

    void TAPNModelBuilder::addTransportArc(const std::string& source,
                const std::string& transition_name,
                const std::string& target, int weight,
                bool lstrict, bool ustrict, int lower, int upper)
    {
        auto in_place = find_place(source);
        auto out_place = find_place(target);
        auto transition = find_transition(transition_name);
        _transportArcs.emplace_back(new TransportArc(*in_place, *transition, *out_place, TimeInterval(lstrict, lower, upper, ustrict), weight));
    }

    TimedPlace* TAPNModelBuilder::find_place(const std::string& pid) {
        for(auto* p : _places)
            if(p->getName().compare(pid) == 0)
                return p;
        std::cerr << "Could not find place \"" << pid << "\". It must be defined before use." << std::endl;
        exit(-1);
        return nullptr;
    }

    TimedTransition* TAPNModelBuilder::find_transition(const std::string& tid) {
        for(auto* t : _transitions)
            if(t->getName().compare(tid) == 0)
                return t;
        std::cerr << "Could not find transition \"" << tid << "\". It must be defined before use." << std::endl;
        exit(-1);
        return nullptr;
    }

    TAPN::TimedArcPetriNet* TAPNModelBuilder::make_tapn()
    {
        return new TAPN::TimedArcPetriNet(_places, _transitions, _inputArcs, _outputArcs, _transportArcs, _inhibitorArcs);
    }
}
