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
#include "boost/functional/hash.hpp"
#include "NonStrictMarkingBase.hpp"
#include <iostream>
#include "../../Core/TAPN/TAPN.hpp"

using namespace std;

namespace VerifyTAPN {
namespace DiscreteVerification {
 
   struct MetaData {
    public:
        MetaData() : passed(false), inTrace(false) {};
        bool passed;
        bool inTrace;
    };
    
    struct MetaDataWithTrace : public MetaData {
        const TAPN::TimedTransition* generatedBy;
    };
    
    // ugly forward declaration
    template<class MetaData>
    class EncodingPointer;
    
    struct MetaDataWithTraceAndEncoding : public MetaDataWithTrace {
        EncodingPointer<MetaData>* ep;
        MetaDataWithTraceAndEncoding* parent;
    };
    
    class NonStrictMarking : public NonStrictMarkingBase{
    public:
        NonStrictMarking():NonStrictMarkingBase(), meta(new MetaData()){}
	NonStrictMarking(const TAPN::TimedArcPetriNet& tapn, const std::vector<int>& v):NonStrictMarkingBase(tapn, v){}
	NonStrictMarking(const NonStrictMarkingBase& nsm):NonStrictMarkingBase(nsm){

        }
        NonStrictMarking(const NonStrictMarking& nsm):NonStrictMarkingBase(nsm){

        }
    public:
        MetaData* meta;
    };
    

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */

#endif /* NONSTRICTMARKING_HPP_ */
