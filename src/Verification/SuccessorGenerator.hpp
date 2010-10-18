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
		SuccessorGenerator(boost::shared_ptr<TAPN::TimedArcPetriNet> tapn, unsigned kBound) : tapn(tapn), kBound(kBound)
		{
			nInputArcs = tapn->GetNumberOfInputArcs();
			arcsArray = new unsigned[nInputArcs];
			tokenIndices = new boost::numeric::ublas::matrix<int>(nInputArcs,kBound);

			ClearAll();
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

	private: // data
		boost::shared_ptr<TAPN::TimedArcPetriNet> tapn;
		unsigned* arcsArray;
		unsigned nInputArcs;
		unsigned kBound;
		boost::numeric::ublas::matrix<int>* tokenIndices;
	};

	inline std::ostream& operator<<(std::ostream& out, const VerifyTAPN::SuccessorGenerator& succGen)
	{
		succGen.Print( out );
		return out;
	}
}
#endif /* SUCCESSORGENERATOR_HPP_ */
