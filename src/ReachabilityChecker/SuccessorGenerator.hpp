#ifndef SUCCESSORGENERATOR_HPP_
#define SUCCESSORGENERATOR_HPP_

#include <vector>
#include "boost/smart_ptr.hpp"
#include "boost/numeric/ublas/matrix.hpp"
#include "boost/numeric/ublas/io.hpp"
#include "../Core/TAPN/TimedArcPetriNet.hpp"
#include "Successor.hpp"
#include "../Core/VerificationOptions.hpp"
#include "../Core/SymbolicMarking/MarkingFactory.hpp"

namespace VerifyTAPN {
	class SymbolicMarking;

	class SuccessorGenerator {
	public:
	    SuccessorGenerator(const TAPN::TimedArcPetriNet & tapn, const MarkingFactory & factory, const VerificationOptions & options)
	    :tapn(tapn), factory(factory), options(options)
	    {
	        nInputArcs = tapn.GetNumberOfConsumingArcs();
	        arcsArray = new unsigned [nInputArcs];
	        tokenIndices = new boost::numeric::ublas::matrix<int>(nInputArcs, options.GetKBound());
	    }

	    ;
	    virtual ~SuccessorGenerator()
	    {
	        delete [] arcsArray;
	        delete tokenIndices;
	    }

	    ;
	public:
	    void GenerateDiscreteTransitionsSuccessors(const SymbolicMarking & marking, std::vector<VerifyTAPN::Successor> & succ);
	public:
	    void Print(std::ostream & out) const;
	public:
	    inline void ClearAll()
	    {
	        ClearArcsArray();
	        ClearTokenIndices();
	    }

	    inline void ClearArcsArray()
	    {
	        memset(arcsArray, 0, nInputArcs * sizeof (arcsArray[0]));
	    }

	    inline void ClearTokenIndices()
	    {
	        tokenIndices->clear();
	    }

	private:
	    void CollectArcsAndAppropriateTokens(const TAPN::TimedTransition::Vector & transitions, const SymbolicMarking *marking);
	    void GenerateSuccessors(const TAPN::TimedTransition::Vector & transitions, const SymbolicMarking *marking, std::vector<Successor> & succ);
	    void GenerateSuccessorForCurrentPermutation(const TAPN::TimedTransition & currTransition, const unsigned int *indices, const unsigned int currTransitionIndex, const unsigned int presetSize, const SymbolicMarking *marking, std::vector<Successor> & succ);
	private:
	    bool IsTransitionEnabled(const TAPN::TimedTransition& transition, const SymbolicMarking* marking, unsigned int currTransitionIndex, unsigned int presetSize) const;
	    void UpdateArcInfo(const SymbolicMarking *marking, int currInputPlaceIndex, const TAPN::TimeInterval & ti, unsigned int & currInputArcIdx);
	private:
	    const TAPN::TimedArcPetriNet& tapn;
		const MarkingFactory& factory;
		unsigned int* arcsArray;
		unsigned int nInputArcs;
		const VerificationOptions& options;
		boost::numeric::ublas::matrix<int>* tokenIndices;
	};

	inline std::ostream& operator<<(std::ostream& out, const VerifyTAPN::SuccessorGenerator& succGen)
	{
		succGen.Print( out );
		return out;
	}
}
#endif /* SUCCESSORGENERATOR_HPP_ */
