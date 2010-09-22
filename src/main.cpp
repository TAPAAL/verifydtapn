#include <iostream>
#include "TAPN/TAPN.hpp"
#include "boost/smart_ptr.hpp"
#include "boost/make_shared.hpp"

using namespace std;
using namespace VerifyTAPN::TAPN;
using namespace boost;

shared_ptr<TimedArcPetriNet> CreateTAPN(){
	shared_ptr<TimedPlace::Vector> places = make_shared<TimedPlace::Vector>();

	shared_ptr<TimedPlace> place = make_shared<TimedPlace>("p0");
	places->push_back(place);

	shared_ptr<TimedTransition::Vector> transitions = make_shared<TimedTransition::Vector>();
	shared_ptr<TimedTransition> transition = make_shared<TimedTransition>("t0");
	transitions->push_back(transition);

	boost::shared_ptr<TimedArcPetriNet> tapn = boost::make_shared<TimedArcPetriNet>(places, transitions);
	return tapn;
}



int main(int argc, const char* argv[]) {
	boost::shared_ptr<TimedArcPetriNet> tapn = CreateTAPN();
	cout << (*tapn);

	return 0;
}


