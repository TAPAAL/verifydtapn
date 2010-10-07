#include <iostream>
#include "TAPN/TAPN.hpp"
#include "boost/smart_ptr.hpp"
#include "boost/make_shared.hpp"
#include "Core/TAPNXmlParser.hpp"
#include "Core/VerificationOptions.hpp"
#include "Core/DiscretePart.hpp"
#include "Core/SymMarking.hpp"
#include "Verification/PWList.hpp"

using namespace std;
using namespace VerifyTAPN;
using namespace VerifyTAPN::TAPN;
using namespace boost;

int main(int argc, char* argv[]) {
	std::vector<int> vec;
	DiscretePart dp(vec);
	dbm::dbm_t dbm(5);
	SymMarking marking(dp, dbm);

	WaitingList* wl = new QueueWaitingList();
	PWList list(wl);
	list.Add(marking);

	return 0;
}

