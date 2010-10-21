#ifndef SUCCESSORGENERATOR_HPP_
#define SUCCESSORGENERATOR_HPP_

#include <vector>
#include "boost/smart_ptr.hpp"
#include "boost/numeric/ublas/matrix.hpp"
#include "boost/numeric/ublas/io.hpp"
#include "../TAPN/TimedArcPetriNet.hpp"

namespace VerifyTAPN {
	class SymMarking;

	class SuccessorGenerator {
	public: // construction
		SuccessorGenerator(const TAPN::TimedArcPetriNet& tapn, unsigned int kBound) : tapn(tapn), kBound(kBound)
		{
			nInputArcs = tapn.GetNumberOfInputArcs();
			arcsArray = new unsigned[nInputArcs];
			tokenIndices = new boost::numeric::ublas::matrix<int>(nInputArcs,kBound);
		};

		virtual ~SuccessorGenerator()
		{
			delete [] arcsArray;
			delete tokenIndices;
		};


	public:
		void GenerateDiscreteTransitionsSuccessors(const SymMarking* marking, std::vector<SymMarking*>& succ);

	public: // inspectors
		void Print(std::ostream& out) const;

	public: // modifiers
		void ClearAll();
		void ClearArcsArray();
		void ClearTokenIndices();

	private: // modifiers
		void CollectArcsAndAppropriateTokens(const TAPN::TimedTransition::Vector& transitions, const SymMarking* marking);
		void GenerateSuccessors(const TAPN::TimedTransition::Vector& transitions, const SymMarking* marking, std::vector<SymMarking*>& succ);
		void GenerateSuccessorForCurrentPermutation(const TAPN::TimedTransition& currTransition, const unsigned int* indices, const unsigned int currTransitionIndex, const unsigned int presetSize, const SymMarking* marking, std::vector<SymMarking*>& succ);

	private: // inspectors
		bool IsTransitionEnabled(unsigned int currTransitionIndex, unsigned int presetSize) const;


	private: // data
		const TAPN::TimedArcPetriNet& tapn;
		unsigned int* arcsArray;
		unsigned int nInputArcs;
		unsigned int kBound;
		boost::numeric::ublas::matrix<int>* tokenIndices;
	};

	inline std::ostream& operator<<(std::ostream& out, const VerifyTAPN::SuccessorGenerator& succGen)
	{
		succGen.Print( out );
		return out;
	}
}
#endif /* SUCCESSORGENERATOR_HPP_ */
