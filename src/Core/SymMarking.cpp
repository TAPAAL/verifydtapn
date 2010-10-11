#include "SymMarking.hpp"
#include "../TAPN/TimedArcPetriNet.hpp"
#include "Pairing.hpp"

namespace VerifyTAPN {

using namespace VerifyTAPN::TAPN;

	/////////////////////////////////////////////
	// Construction
	/////////////////////////////////////////////
	SymMarking::SymMarking(const DiscretePart& dp, const dbm::dbm_t& dbm) : dp(dp), dbm(dbm)
	{
		std::vector<int> pVector = dp.GetTokenPlacementVector();
		std::vector<int> map;
		int i = 0;
		map.push_back(-2); // token 0 in the dbm is always the 0 clock, thus it does not map to a token in the net.

		for(std::vector<int>::const_iterator iter = pVector.begin(); iter != pVector.end(); ++iter)
		{
			if((*iter) != -1) // if not BOTTOM
			{
				map.push_back(i);

			}
			i++;
		}

		mapping = map;
	}

	/////////////////////////////////////////////
	// Initializers
	/////////////////////////////////////////////
	void SymMarking::initMapping(const std::vector<int> & placement)
	{

	}

	/////////////////////////////////////////////
	// Inspectors
	/////////////////////////////////////////////
	const DiscretePart* SymMarking::GetDiscretePart() const
	{
		return &dp;
	}

	const dbm::dbm_t& SymMarking::GetZone() const
	{
		return dbm;
	}

	const TokenMapping& SymMarking::GetTokenMapping() const
	{
		return mapping;
	}

	void SymMarking::GenerateDiscreteTransitionSuccessors(const VerifyTAPN::TAPN::TimedArcPetriNet& tapn, std::vector<SymMarking*>& succ) const
	{
		TimedTransition::Vector transitions = tapn.GetTransitions();

		for (TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); ++iter) {
			boost::shared_ptr<TimedTransition> t = (*iter);

			if(t->isEnabledBy(tapn,*this))
			{
				GenerateDiscreteTransitionSuccessorFor(*t,succ);
			}

		}

		// For each transition
		//   if enabled
		//      create successor(s) (if choice between multiple tokens)


	}

	void SymMarking::GenerateDiscreteTransitionSuccessorFor(const TimedTransition& t, std::vector<SymMarking*>& succ) const
	{
		Pairing pairing(t);



	}

	void SymMarking::Delay()
	{
		dbm.up();
	}
}
