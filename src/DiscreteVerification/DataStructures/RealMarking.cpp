#include "DiscreteVerification/DataStructures/RealMarking.hpp"

namespace VerifyTAPN::DiscreteVerification {

void RealPlace::add(RealToken new_token)
{
    size_t index = 0;
    for(auto& token : tokens) {
        if(token.getAge() == new_token.getAge()) {
            token.add(new_token.getCount());
            return;
        }
        if(token.getAge() > new_token.getAge()) break;
        index++;
    }
    tokens.insert(tokens.begin() + index, new_token);
}

bool RealPlace::remove(RealToken to_remove)
{
    size_t index = 0;
    for(auto it = tokens.begin() ; it != tokens.end() ; it++) {
        if(it->getAge() == to_remove.getAge()) {
            it->remove(to_remove.getCount());
            if(it->getCount() == 0) {
                tokens.erase(it);
            }
            return true;
        }
    }
    return false;
}

RealTokenList RealMarking::emptyTokenList = RealTokenList();

RealMarking::RealMarking(NonStrictMarkingBase& base)
{
    auto placeList = base.getPlaceList();
    for(const auto& place : placeList) {
        places.push_back(RealPlace(place));
    }
}

RealMarking::RealMarking(const RealMarking& other)
{
    for(const auto& place : other.places) {
        places.push_back(place);
    }
}

uint32_t RealMarking::size() const
{
    uint32_t size = 0;
    for(const auto& place : places) {
        size += place.numberOfTokens();
    }
    return size;
}

RealPlaceList& RealMarking::getPlaceList()
{
    return places;
}

RealTokenList& RealMarking::getTokenList(int placeId)
{
    for(auto& place : places) {
        if(place.placeId() == placeId) {
            return place.tokens;
        }
    }
    return emptyTokenList;
}

void RealMarking::deltaAge(double x)
{
    for(auto& place : places) {
        place.deltaAge(x);
    }
}

NonStrictMarkingBase RealMarking::generateImage()
{
    NonStrictMarkingBase marking;
    for(const auto& place : places) {
        Place new_place = place.generateImagePlace();
        marking.addTokenInPlace(*new_place.place, new_place.tokens.front());
    }
    return marking;
}

uint32_t RealMarking::numberOfTokensInPlace(int placeId) const
{
    for(const auto& place : places) {
        if(place.placeId() == placeId) {
            return place.numberOfTokens();
        }
    }
    return 0;
}

bool RealMarking::canDeadlock(const TAPN::TimedArcPetriNet &tapn, int maxDelay, bool ignoreCanDelay) const
{
    return deadlocked;
}

bool RealMarking::removeToken(int placeId, double age)
{
    RealToken token(age, 1);
    return removeToken(placeId, token);
}

bool RealMarking::removeToken(int placeId, RealToken& token)
{
    for(auto& place : places) {
        if(place.placeId() == placeId) {
            return place.remove(token);
        }
    }
    return false;
}

bool RealMarking::removeToken(RealPlace &place, RealToken &token)
{
    return place.remove(token);
}

void RealMarking::addPlace(RealPlace &place) 
{
    int id = place.placeId();
    int i;
    for(i = 0 ; i < places.size() ; i++) {
        int curr_id = places[i].placeId();

        if(id == curr_id) return;
        if(curr_id > id) break;
    }
    places.insert(places.begin() + i, place);
}

void RealMarking::addTokenInPlace(TAPN::TimedPlace &place, double age)
{
    RealToken token(age, 1);
    addTokenInPlace(place, token);
}

void RealMarking::addTokenInPlace(RealPlace &place, RealToken &token)
{
    place.add(token);
    addPlace(place);
}

void RealMarking::addTokenInPlace(const TAPN::TimedPlace &place, RealToken &token)
{
    size_t index = 0;
    for(auto& pit : places) {
        if(pit.placeId() != place.getIndex()) {
            index++;
            continue;
        };
        addTokenInPlace(pit, token);
        return;
    }
    RealPlace new_place(&place);
    new_place.add(token);
    places.insert(places.begin() + index, new_place);
}

double RealMarking::availableDelay() const
{
    double available = std::numeric_limits<double>::infinity();
    for(const auto& place : places) {
        double delay = place.availableDelay();
        if(delay < available) {
            available = delay;
        }
    }
    return available;
}

void RealMarking::setDeadlocked(const bool dead) 
{
    deadlocked = dead;
}

}