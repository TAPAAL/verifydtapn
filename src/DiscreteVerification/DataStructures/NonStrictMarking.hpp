/*
 * NonStrictMarking.hpp
 *
 *  Created on: 29/02/2012
 *      Author: MathiasGS
 */

#ifndef NONSTRICTMARKING_HPP_
#define NONSTRICTMARKING_HPP_

#include <assert.h>
#include <vector>
#include "NonStrictMarkingBase.hpp"
#include <iostream>
#include "../../Core/TAPN/TAPN.hpp"

using namespace std;

namespace VerifyTAPN {
namespace DiscreteVerification {

	class NonStrictMarking;
 
   struct MetaData {
    public:
        MetaData() : passed(false), inTrace(false), totalDelay(0) {};
        bool passed;
        bool inTrace;
        int totalDelay;
    };
    
    struct MetaDataWithTrace : public MetaData {
        const TAPN::TimedTransition* generatedBy;
    };
    
    struct WorkflowSoundnessMetaData : public MetaData {
	   public:
    	WorkflowSoundnessMetaData() : MetaData(), parents() { totalDelay = INT_MAX;};
		   vector<WorkflowSoundnessMetaData*> parents;
	   };

    // ugly forward declaration
    template<class MetaData>
    struct EncodingPointer;
    
    struct MetaDataWithTraceAndEncoding : public MetaDataWithTrace {
        EncodingPointer<MetaData>* ep;
        MetaDataWithTraceAndEncoding* parent;
    };
    
    class NonStrictMarking : public NonStrictMarkingBase{
    public:
        NonStrictMarking():NonStrictMarkingBase(), meta(new MetaData()){}
	NonStrictMarking(const TAPN::TimedArcPetriNet& tapn, const std::vector<int>& v): NonStrictMarkingBase(tapn, v), meta(NULL){}
	NonStrictMarking(const NonStrictMarkingBase& nsm):NonStrictMarkingBase(nsm), meta(NULL){

        }
        NonStrictMarking(const NonStrictMarking& nsm):NonStrictMarkingBase(nsm), meta(NULL){

        }
	inline int calculateTotalDelay(){
		int totalDelay = 0;
		NonStrictMarking* parent = (NonStrictMarking*)this->getParent();
		if (parent && parent->meta) {
			totalDelay = parent->meta->totalDelay;
			if (this->getGeneratedBy() == NULL) ++totalDelay;
		}
		return totalDelay;
	}
    public:
        MetaData* meta;
    };
} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */

#endif /* NONSTRICTMARKING_HPP_ */
