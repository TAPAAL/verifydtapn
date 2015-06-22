/*
 * NonStrictMarking.hpp
 *
 *  Created on: 29/02/2012
 *      Author: MathiasGS
 */

#ifndef NONSTRICTMARKING_HPP_
#define NONSTRICTMARKING_HPP_

#include <vector>
#include "NonStrictMarkingBase.hpp"
#include <iostream>
#include "../../Core/TAPN/TAPN.hpp"
#include "ptrie.h"

using namespace ptrie;
using namespace std;

namespace VerifyTAPN {
namespace DiscreteVerification {

	class NonStrictMarking;
 
   struct MetaData {
    public:
        MetaData() : passed(false), inTrace(false), totalDelay(0) {};
        virtual ~MetaData(){};
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
        virtual ~WorkflowSoundnessMetaData(){parents.clear();}
		   vector<MetaData*> parents;
	   };
           
    
    struct MetaDataWithTraceAndEncoding : public MetaDataWithTrace {
        ptriepointer_t<MetaData*> ep;
            MetaDataWithTraceAndEncoding* parent;
    };
    
    struct WorkflowSoundnessMetaDataWithEncoding : public MetaDataWithTraceAndEncoding {
    public:
        WorkflowSoundnessMetaDataWithEncoding() : MetaDataWithTraceAndEncoding(), parents() 
            { totalDelay = INT_MAX;};
            virtual ~WorkflowSoundnessMetaDataWithEncoding()
            {
                parents.clear();
            }
            
            vector<MetaData*> parents;
    };
    
    class NonStrictMarking : public NonStrictMarkingBase{
    public:
        NonStrictMarking():NonStrictMarkingBase(), meta(NULL){}
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
