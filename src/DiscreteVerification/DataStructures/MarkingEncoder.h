/* 
 * File:   MarkingEncoder.h
 * Author: Peter G. Jensen
 *
 * Created on 10 June 2015, 18:45
 */

#ifndef MARKINGENCODER_H
#define	MARKINGENCODER_H

#include "binarywrapper.h"
#include <assert.h>
#include <limits>
#include <vector>
#include "ptrie.h"
#include "NonStrictMarkingBase.hpp"


using namespace ptrie;
namespace VerifyTAPN {
    namespace DiscreteVerification {

        class CoveredMarkingVisitor;
        
        template<typename T, typename M = NonStrictMarkingBase>
        class MarkingEncoder
        {
        friend class CoveredMarkingVisitor;
        typedef binarywrapper_t<T> encoding_t;

        private:
            const uint maxNumberOfTokens;
            const uint maxAge;
            const uint numberOfPlaces;
            const uint countBitSize;
            const uint placeAgeBitSize;
            const uint offsetBitSize; 
            const uint markingBitSize;
            TAPN::TimedArcPetriNet& tapn;
            encoding_t scratchpad;
        public:
            MarkingEncoder(TAPN::TimedArcPetriNet& tapn, int knumber);
            ~MarkingEncoder();
            

            M* decode(const ptriepointer_t<T>& pointer);
            encoding_t encode(M* marking);
        };
        
        template<typename T, typename M>
        MarkingEncoder<T, M>::MarkingEncoder(TAPN::TimedArcPetriNet& tapn,
                                int knumber):
            maxNumberOfTokens(knumber),
            maxAge(tapn.getMaxConstant() + 1),
            numberOfPlaces(tapn.getNumberOfPlaces()),
            countBitSize(ceil(log2((knumber ? knumber : 1)) + 1)),
            placeAgeBitSize(ceil(log2((numberOfPlaces * (tapn.getMaxConstant() + 1))) + 1)),
            offsetBitSize(placeAgeBitSize + countBitSize),
            markingBitSize(offsetBitSize * (knumber ? knumber : 1)),
            tapn(tapn)
        {
                scratchpad = encoding_t(0);
        }
        
        template<typename T, typename M>
        MarkingEncoder<T, M>::~MarkingEncoder()
        {
            scratchpad.release();
        }
        
        template<typename T, typename M>
        M* MarkingEncoder<T, M>::decode(const ptriepointer_t<T>& pointer)
        {
            // we allready know here that the scratchpad is large enough,
            // it is monotonically increased when we encode, ie; no marking
            // taking up more bits are currently in the ptrie.
            
            M* m = new M();
            assert(pointer.container->consistent());
            const encoding_t remainder = pointer.remainder();
            uint n = pointer.write_partial_encoding(scratchpad);
            assert(pointer.container->consistent());
            uint r_offset = n - (n % 8);    // make sure n matches exactly on a byte
            
            uint cbit = 0;
            // foreach token
      
            bool bit;
            PlaceList& places = m->getPlaceList();
            for (uint i = 0; i < maxNumberOfTokens; i++) {
                uint offset = offsetBitSize * i;
                cbit = offset + offsetBitSize - 1;
                unsigned long long data = 0;
                uint count = 0;
                
                while (cbit >= offset + countBitSize) {
                    data = data << 1;
                    assert(pointer.container->consistent());
                    if(cbit < n)
                    {
                        bit = scratchpad.at((n - 1) - cbit);
                        assert(pointer.container->consistent());
                    }
                    else
                    {
                        bit = remainder.at(cbit - r_offset);
                        assert(pointer.container->consistent());
                    }
                    
                    if (bit) {
                        data = data | 1;
                        assert(pointer.container->consistent());
                    }
                    if (cbit == 0) {
                        break;
                    }
                    cbit--;
                }
                assert(pointer.container->consistent());
                while (cbit >= offset) {
                    count = count << 1;

                    if(cbit < n)
                    {
                        bit = scratchpad.at((n - 1) - cbit);
                    }
                    else
                    {
                        bit = remainder.at(cbit - r_offset);
                    }
                    
                    if (bit) {
                        count = count | 1;
                    }
                    
                    if(cbit == 0) break;
                    cbit--;
                }

                if (count) {
                    int age = floor(data / this->numberOfPlaces);
                    uint place = (data % this->numberOfPlaces);
                    auto tplace = &tapn.getPlace(place);
                    size_t last = 0;
                    if(places.size() == 0)
                    {
                        last = 0;
                        places.push_back(Place(tplace));
                    }
                    else
                    {
                        if(places[last].place != tplace)
                        {
                            places.push_back(Place(tplace));
                        }
                        last = places.size() - 1;
                    }
                    places[last].tokens.push_back(Token(age, count));
                }
                else
                {
                    // no more data, just getting zeroes
                    break;
                }
                
                if (cbit == r_offset + remainder.size() * 8) {
                    // rest is zeroes, ignore
                    break;
                }
            }
            return m;
        }
        
        template<typename T, typename M>
        binarywrapper_t<T> MarkingEncoder<T, M>::encode(M* marking)
        {
            // make sure we have space to encode marking
            size_t count = 0;
            for (vector<Place>::const_iterator pi = marking->getPlaceList().begin();
                    pi != marking->getPlaceList().end();
                    pi++) {
                count += pi->tokens.size();
            }
            count *= offsetBitSize;
            count /= 8;
            count += 1;
            
            if(scratchpad.size() < count)
            {
                scratchpad.release();
                scratchpad = encoding_t(count * 8);
            }
            


            scratchpad.zero();
            int tc = 0;
            uint bitcount = 0;
            
            for (vector<Place>::const_iterator pi = marking->getPlaceList().begin();
                    pi != marking->getPlaceList().end();
                    pi++) { // for each place

                int pc = pi->place->getIndex();

                for (TokenList::const_iterator ti = pi->tokens.begin(); // for each token-element
                        ti != pi->tokens.end();
                        ti++) {


                    int offset = tc * this->offsetBitSize; // the offset of the variables for this token
                    uint number = ti->getCount();
                    bitcount = 0;
                    while (number) { // set the vars while there are bits left

                        this->scratchpad.set(offset + bitcount, number & 1);
                        bitcount++;
                        number = number >> 1;
                    }
                    uint pos = pc + this->numberOfPlaces * ti->getAge(); // the enumerated configuration of the token
                    bitcount = countBitSize;
                    /* binary */
                    while (pos) { // set the vars while there are bits left
                        this->scratchpad.set(offset + bitcount, pos & 1);
                        bitcount++;
                        pos = pos >> 1;
                    }
                    tc++;
                }
            }
            if (tc == 0)
                return encoding_t(scratchpad.raw(), 0);
            else
                return encoding_t(scratchpad.raw(), ((tc - 1) * offsetBitSize) + bitcount); 
        }
    }   
}


#endif	/* MARKINGENCODER_H */

