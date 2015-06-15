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

using namespace pgj;
namespace VerifyTAPN {
    namespace DiscreteVerification {

        template<typename T>
        class MarkingEncoder
        {
        typedef binarywrapper<T> encoding_t;
        private:
            const uint maxNumberOfTokens;
            const uint maxAge;
            const uint numberOfPlaces;
            const uint countBitSize;
            const uint placeAgeBitSize;
            const uint offsetBitSize; 
            uint markingBitSize;
            TAPN::TimedArcPetriNet& tapn;
            encoding_t scratchpad;
            void* raw;
        public:
            MarkingEncoder(TAPN::TimedArcPetriNet& tapn, int knumber,
                                                        int nplaces, int mage);
            
            NonStrictMarkingBase* decode
                                            (const ptriepointer<T>& pointer);
            encoding_t encode(NonStrictMarkingBase* marking);
        };
        
        template<typename T>
        MarkingEncoder<T>::MarkingEncoder(TAPN::TimedArcPetriNet& tapn,
                                int knumber, int nplaces, int mage):
            maxNumberOfTokens(knumber),
            maxAge(mage + 1),
            numberOfPlaces(nplaces),
            countBitSize(ceil(log2((knumber ? knumber : 1)) + 1)),
            placeAgeBitSize(ceil(log2((nplaces * (mage + 1))) + 1)),
            offsetBitSize(placeAgeBitSize + countBitSize),
            markingBitSize(offsetBitSize * (knumber ? knumber : 1)),
            tapn(tapn)
        {
                scratchpad = encoding_t(markingBitSize);
                raw = (void*)scratchpad.raw();
        }
        
        template<typename T>
        NonStrictMarkingBase* MarkingEncoder<T>::decode(const ptriepointer<T>& pointer)
        {
            assert(scratchpad.raw() == raw);
            NonStrictMarkingBase* m = new NonStrictMarkingBase();
            assert(pointer.container->isConsistent());
            const encoding_t remainder = pointer.remainder();
            uint n = pointer.revsereWritePartialEncoding(scratchpad);
            assert(pointer.container->isConsistent());
            uint r_offset = n - (n % 8);    // make sure n matches exactly on a byte
            
            uint cbit = 0;
            // foreach token
            uint data = 0;
            uint count = 0;
            bool bit;
            for (int i = 0; i < maxNumberOfTokens; i++) {
                uint offset = offsetBitSize * i;
                cbit = offset + offsetBitSize - 1;
                
                while (cbit >= offset + countBitSize) {
                    data = data << 1;
                    assert(pointer.container->isConsistent());
                    if(cbit < n)
                    {
                        bit = scratchpad.at((n - 1) - cbit);
                        assert(pointer.container->isConsistent());
                    }
                    else
                    {
                        bit = remainder.at(cbit - r_offset);
                        assert(pointer.container->isConsistent());
                    }
                    
                    if (bit) {
                        data = data | 1;
                        assert(pointer.container->isConsistent());
                    }
                    if (cbit == 0) {
                        break;
                    }
                    cbit--;
                }
                assert(pointer.container->isConsistent());
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
                    Token t = Token(age, count);
                    m->addTokenInPlace(tapn.getPlace(place), t);
                    data = 0;
                    count = 0;
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
        
        template<typename T>
        binarywrapper<T> MarkingEncoder<T>::encode(NonStrictMarkingBase* marking)
        {
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

