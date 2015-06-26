/* 
 * File:   CoveredMarkingVisitor.cpp
 * Author: Peter G. Jensen
 *
 * Created on 18 June 2015, 14:23
 */

#include "CoveredMarkingVisitor.h"


using namespace ptrie;
namespace VerifyTAPN {
namespace DiscreteVerification {

    CoveredMarkingVisitor::CoveredMarkingVisitor(
                            MarkingEncoder<MetaData*, NonStrictMarking>& enc)
    : encoder(enc)
    {
        scratchpad = enc.scratchpad.clone();
    }
    
    
    CoveredMarkingVisitor::~CoveredMarkingVisitor()
    {
        scratchpad.release();
    }
    
    bool CoveredMarkingVisitor::set(uint32_t index, bool value)
    {
        if(_found) return true;  // end
        scratchpad.set(index, value);
        // If we have enough bits to constitute a single token (with placement)
        if(index % encoder.offsetBitSize == 0 && index > 0)
        {
            size_t begin = index - encoder.offsetBitSize;
            uint data = 0;
            uint count = 0;
            uint cbit = index - 1;
            
            while (cbit >= begin + encoder.countBitSize) {
                data = data << 1;
                if(scratchpad.at(cbit))
                {
                    data |= 1;
                }
                --cbit;
            }
            
            while(cbit >= begin)
            {
                data = data << 1;
                if(scratchpad.at(cbit))
                {
                    data |= 1;
                }
                if(cbit == 0) break;
                cbit--;
            }
            
            if (count) {
                return !target_contains_token(data, count);
            }
            // should not really happen
            assert(false);
            return true;    // skip if happens!
        }
        else
        {
            return false;   // not enough info
        }
    }
    
    bool CoveredMarkingVisitor::set_remainder(uint32_t index, 
                                            ptriepointer_t<MetaData*> pointer)
    {
        if(_found) return true;  // end
        uint size = scratchpad.size();
        size -= (index / 8);
        encoding_t enc = pointer.remainder();
        if(enc.size() > size)
        {
            return false;   // not a match, dont stop
        }
        else
        {
            encoding_t remainder = pointer.remainder();
            uint offset = index - (index % 8);  // offset matches on a byte
            uint begin = index - (index % encoder.offsetBitSize);
            // check inclusion 

            bool bit;
            uint cbit = begin;
            while(true)
            {
                uint data = 0;
                uint count = 0;
                cbit += encoder.markingBitSize;
                // unpack marking
                while (cbit >= begin + encoder.countBitSize) {
                    data = data << 1;

                    if(cbit < offset) bit = scratchpad.at(cbit);
                    else bit = remainder.at(cbit - offset);

                    if(bit)
                    {
                        data |= 1;
                    }
                    --cbit;
                }

                while(cbit >= begin)
                {
                    count = count << 1;

                    if(cbit < offset) bit = scratchpad.at(cbit);
                    else bit = remainder.at(cbit - offset);

                    if(bit)
                    {
                        count |= 1;
                    }
                    if(cbit == 0) break;
                    cbit--;                
                }
                begin += encoder.markingBitSize;
                if (count) 
                {
                    if(!target_contains_token(data, count))
                    {
                        return false;
                    }
                }
                else
                {
                    break;
                }
            }
        }
        match = pointer;
        _found = true;
        return true;
    }
    
    bool CoveredMarkingVisitor::back(uint32_t index)
    {
        return false;
    }
    
    bool CoveredMarkingVisitor::target_contains_token(uint placeage, uint count)
    {
        if(count == 0) return true;
        
        size_t age = floor(placeage / encoder.numberOfPlaces);
        uint place = (placeage % encoder.numberOfPlaces);
        const TokenList& tokens = target->getTokenList(place);

        for(    TokenList::const_iterator it = tokens.begin();
                it != tokens.end(); ++it)
        {
            if(it->getAge() == age)
            {
                if(it->getCount() <= count) return true; // continue
                else return false; // skip branch
            }
            else if(it->getAge() > age) return false;  // skip branch
        }

        return false; // skip branch
    }
    
    NonStrictMarking* CoveredMarkingVisitor::decode()
    {
        NonStrictMarking* m = encoder.decode(match);
        m->meta = match.get_meta();
        return m;
    }
    
}
}