/* 
 * File:   CoveredMarkingVisitor.h
 * Author: Peter G. Jensen
 *
 * Created on 17 June 2015, 00:04
 */

#ifndef COVEREDMARKINGVISITOR_H
#define    COVEREDMARKINGVISITOR_H

#include "visitor.h"
#include "MarkingEncoder.h"
#include "NonStrictMarkingBase.hpp"
#include "NonStrictMarking.hpp"

#include <limits>
#include <vector>


using namespace ptrie;
namespace VerifyTAPN::DiscreteVerification {

    class CoveredMarkingVisitor
            : public visitor_t<MetaData *> {
        typedef binarywrapper_t<MetaData *> encoding_t;
    private:
        MarkingEncoder<MetaData *, NonStrictMarking> &encoder;
        NonStrictMarking *target{};
        encoding_t scratchpad;
        ptriepointer_t<MetaData *> match;
        bool _found{};
        ptriepointer_t<MetaData *> _targetencoding;

    private:
        bool target_contains_token(unsigned long long placeage, uint count);

    public:
        explicit CoveredMarkingVisitor(
                MarkingEncoder<MetaData *, NonStrictMarking> &enc);

        ~CoveredMarkingVisitor();

        bool back(int index) override;

        bool set(int index, bool value) override;

        bool set_remainder(int index,
                           ptriepointer_t<MetaData *> pointer) override;

        void set_target(NonStrictMarking *m, ptriepointer_t<MetaData *> me);

        NonStrictMarking *decode();

        bool found() { return _found; }
    };

}

#endif    /* BINARYMARKINGVISITOR_H */

