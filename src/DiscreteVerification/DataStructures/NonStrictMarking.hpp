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
        MetaData() : passed(false), inTrace(false), parents(new  vector<NonStrictMarking*>) {};
        bool passed;
        bool inTrace;
        vector<NonStrictMarking*>* parents;
    };
    
    struct MetaDataWithTrace : public MetaData {
        const TAPN::TimedTransition* generatedBy;
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
	NonStrictMarking(const TAPN::TimedArcPetriNet& tapn, const std::vector<int>& v): meta(NULL), NonStrictMarkingBase(tapn, v){}
	NonStrictMarking(const NonStrictMarkingBase& nsm):meta(NULL), NonStrictMarkingBase(nsm){

        }
        NonStrictMarking(const NonStrictMarking& nsm):meta(NULL),NonStrictMarkingBase(nsm){

        }
    public:
        MetaData* meta;
    };
    

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */

#endif /* NONSTRICTMARKING_HPP_ */
