/* 
 * File:   PData.h
 * Author: Peter Gjøl Jensen
 *
 * Created on 5. november 2012, 10:22
 */


#include "NonStrictMarking.hpp"
#include "EncodingStructure.h"
#include "TimeDart.hpp"

#ifndef PDATA_H
#define	PDATA_H
namespace VerifyTAPN {
    namespace DiscreteVerification {

        // pointer containing enough data to reconstruct the stored data at any time!
        template<typename T>
        struct EncodingPointer {
            // The part of the encoding not being represented by the path in the PTrie
            EncodingStructure<T*> encoding;
            // The coresponding node in the PTrie
            unsigned int node;

            // empty constructor
            EncodingPointer() {
            };
            
            // Construct a pointer with enough (persistent) data to recreate the marking. 
            // The encoding is cloned as it is not persistant in the PTrie
            EncodingPointer(EncodingStructure<T*> &en, unsigned int n) : encoding(en.clone()), node(n) {
            }
        };
        
        template<typename T>
        class PTrie {
        public:
            typedef unsigned int uint;
            typedef EncodingStructure<T*> MarkingEncoding;

            struct Result {
                bool isNew;
                MarkingEncoding encoding;
                uint pos;

                Result(bool ex, MarkingEncoding en, uint node) : isNew(ex), encoding(en), pos(node) {
                };
            };

            struct PNode {
                MarkingEncoding* data;
                uint highpos;
                uint lowpos;
                short int highCount;
                short int lowCount;
                uint parent;
            };


            PTrie(TAPN::TimedArcPetriNet& tapn, int knumber, int nplaces, int mage) :
            maxNumberOfTokens(knumber),
            maxAge(mage + 1),
            numberOfPlaces(nplaces),
            countBitSize(ceil(log2((knumber ? knumber : 1)) + 1)),
            offsetBitSize(ceil(log2((nplaces * (mage + 1))) + 1) + countBitSize),
            markingBitSize(offsetBitSize * (knumber ? knumber : 1)),
            blockSize(128),
            tapn(tapn) {
                overhead = MarkingEncoding::overhead(this->markingBitSize);
                this->markingBitSize += overhead;
                stored = 0;
                totalSize = blockSize;
                this->pnodeArray.push_back(new PNode[this->totalSize]);
                memset(this->pnodeArray[0], 0xffffffff, this->totalSize * sizeof (PNode));
                pnodeArray[0][0].data = NULL;
                pnodeArray[0][0].highCount = pnodeArray[0][0].lowCount = 0;
                pnodeArray[0][0].lowpos = pnodeArray[0][0].highpos = 0;
                pnodeArray[0][0].parent = 0;
                nextFree = 1;
                encoding = MarkingEncoding(this->markingBitSize);
                blockCount = 0;
                splitThreshold = sizeof (PNode) * 8;
                //+ sizeof (std::list<PNode, allocator<PNode> >) * 4;

            };
            virtual ~PTrie();

            uint splitThreshold;

            bool search(MarkingEncoding* arr, MarkingEncoding en, int size) {
                for (int i = 0; i < size; i++) {
                    if (arr[i] == en)
                        return true;
                }
                return false;
            }

            Result add(NonStrictMarkingBase* marking);

            unsigned int size() {
                return stored;
            }
            void printMemStats();
            void printEncoding(bool* encoding, int length);

            inline PNode* fetchNode(uint i) {
                return &pnodeArray[i / blockSize][i % blockSize];
            }
            NonStrictMarkingBase* enumerateDecode(const EncodingPointer<T>& pointer);

            int enumeratedEncoding(NonStrictMarkingBase* marking);
            const uint maxNumberOfTokens;
            const uint maxAge;
            const uint numberOfPlaces;
            const uint countBitSize;
            const uint offsetBitSize;
            uint markingBitSize;
            const uint blockSize;

            TAPN::TimedArcPetriNet& tapn;

            MarkingEncoding encoding;
            vector<PNode*> pnodeArray;
            uint totalSize;
            uint stored;
            uint nextFree;
            uint overhead;
            uint blockCount;
        };

        template<typename T>
        PTrie<T>::~PTrie() {
            // should only be used when looking for leaks.
            for(uint i = 0; i < (nextFree-1); ++i ){
                PNode* m = fetchNode(i);
                for(uint ii = 0; ii < (m->highCount + m->lowCount); ++ii){
                    delete m->data[ii].getMetaData();
                    delete[] m->data;
                }
            }
            int i = (nextFree-1) / blockSize;
            if( ((nextFree-1) % blockSize) > 0){
                ++i;
            }
            for(; i >= 0; --i){
                delete[] pnodeArray[i];
            }
        }


        template<typename T>
        int PTrie<T>::enumeratedEncoding(NonStrictMarkingBase* marking) {
            encoding.zero();

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
                        if (number & 1) {
                            this->encoding.set(overhead + offset + bitcount, true);
                        }
                        bitcount++;
                        number = number >> 1;
                    }
                    uint pos = pc + this->numberOfPlaces * ti->getAge(); // the enumerated configuration of the token
                    bitcount = countBitSize;
                    /* binary */
                    while (pos) { // set the vars while there are bits left
                        if (pos & 1) {
                            this->encoding.set(overhead + offset + bitcount, true);
                        }
                        bitcount++;
                        pos = pos >> 1;
                    }
                    tc++;
                }
            }
            if (tc == 0)
                return 0;
            else
                return ((tc - 1) * this->offsetBitSize) + bitcount;
        }

        template<typename T>
        NonStrictMarkingBase* PTrie<T>::enumerateDecode(const EncodingPointer<T> &pointer) {
            NonStrictMarkingBase* m = new NonStrictMarkingBase();
            this->encoding.zero();

            uint var = 0;
            uint n = pointer.node;
            while (n) {
                n = fetchNode(n)->parent;
                var++;
            }
            var += this->overhead;

            this->encoding.copy(pointer.encoding, var);
            uint nbits = (var - (var % 8)) + pointer.encoding.Size()*8;
            uint self = pointer.node;

            while (self) {
                var--;
                n = fetchNode(self)->parent;
                bool branch = fetchNode(n)->highpos == self;
                this->encoding.set(var, branch);
                self = n;

            }

            var = this->markingBitSize - 1;
            // foreach token
            uint data = 0;
            uint count = 0;
            for (int i = this->maxNumberOfTokens - 1; i >= 0; i--) {
                uint offset = (this->offsetBitSize * i) + this->overhead + this->countBitSize;
                while (nbits >= offset) {
                    data = data << 1;

                    if (encoding.at(nbits)) {
                        data = data | 1;
                    }
                    if (nbits == 0) {
                        break;
                    }
                    nbits--;
                }
                offset -= this->countBitSize;
                while (nbits >= offset) {
                    count = count << 1;

                    if (encoding.at(nbits)) {
                        count = count | 1;
                    }
                    if (nbits == 0) {
                        break;
                    }
                    nbits--;
                }

                if (count) {
                    int age = floor(data / this->numberOfPlaces);
                    uint place = (data % this->numberOfPlaces);
                    Token t = Token(age, count);
                    m->addTokenInPlace(tapn.getPlace(place), t);
                    data = 0;
                    count = 0;
                }
            }
            return m;
        }

        template<typename T>
        typename PTrie<T>::Result PTrie<T>::add(NonStrictMarkingBase* marking) {

            int encsize = this->enumeratedEncoding(marking) + overhead;
            // go through the PTrie as far as possible with the encoding of the marking
            uint c_count = 0;
            uint prev_count = 0;
            int var = overhead;
            do {

                prev_count = c_count;
                if (encoding.at(var)) {
                    c_count = pnodeArray[c_count / blockSize][c_count % blockSize].highpos;

                } else {
                    c_count = pnodeArray[c_count / blockSize][c_count % blockSize].lowpos;
                }

                var++;

            } while (c_count != 0);
            var--;

            PNode* prev_node = fetchNode(prev_count);

            int listsize = 0;
            if (prev_node->highCount >= 0) {
                listsize += prev_node->highCount;
            }
            if (prev_node->lowCount >= 0) {
                listsize += prev_node->lowCount;
            }

            int size = this->markingBitSize - var;
            MarkingEncoding en = MarkingEncoding(encoding.getRaw(), size, var, encsize);

            int ins = 0;

            for (; ins < listsize; ins++) {
                if (prev_node->data[ins] == en) {
                    break;
                }
            }
            if (ins != listsize) {
                return Result(false, prev_node->data[ins], prev_count);
            }

            en = MarkingEncoding(encoding, size, var, encsize);
            short unsigned int count;

            MarkingEncoding* nlist = new MarkingEncoding[listsize + 1];
            nlist[listsize] = en;

            for (int i = 0; i < listsize; i++) {
                nlist[i] = prev_node->data[i];
            }
            delete[] prev_node->data;
            prev_node->data = nlist;


            bool branch = encoding.at(var);
            if (branch) {
                count = (++prev_node->highCount);
            } else {
                count = (++prev_node->lowCount);
            }

            if (count > splitThreshold) {

                size--;
                int testclist = 0;
                int testnlist = 0;
                PNode* c_node = &this->pnodeArray[floor(this->nextFree / this->blockSize)][this->nextFree % this->blockSize];
                if (branch) {
                    prev_node->highpos = this->nextFree;
                    c_node->data = new MarkingEncoding[testclist = prev_node->highCount];
                    prev_node->highCount = -1;
                    if (prev_node->lowCount > 0)
                        nlist = new MarkingEncoding[testnlist = prev_node->lowCount];
                    else
                        nlist = NULL;
                } else {
                    prev_node->lowpos = this->nextFree;
                    c_node->data = new MarkingEncoding[testclist = prev_node->lowCount];
                    prev_node->lowCount = -1;
                    if (prev_node->highCount > 0)
                        nlist = new MarkingEncoding[testnlist = prev_node->highCount];
                    else
                        nlist = NULL;
                }
                c_node->parent = prev_count;
                prev_count = this->nextFree;
                blockCount++;
                c_node->lowCount = c_node->highCount = 0;
                c_node->lowpos = c_node->highpos = 0;
                uint npos = ((this->markingBitSize - (size + 1)) % 8);

                MarkingEncoding nee;
                int clistcount = 0;
                int nlistcount = 0;
                for (int i = 0; i < listsize + 1; i++) {
                    if (prev_node->data[i].at(npos) == branch) {
                        if (!(size % 8)) {
                            nee = MarkingEncoding(prev_node->data[i], 8);
                            if (i == ins) {
                                en = nee;
                            }
                            prev_node->data[i].release();
                        } else {
                            nee = prev_node->data[i];
                        }
                        if (nee.at((npos + 1) % 8)) {
                            c_node->highCount++;
                        } else {
                            c_node->lowCount++;
                        }
                        c_node->data[clistcount] = nee;
                        clistcount++;
                    } else {
                        nlist[nlistcount] = prev_node->data[i];
                        nlistcount++;
                    }

                }
                delete[] prev_node->data;
                prev_node->data = nlist;

                if (prev_node->highCount == -1 && prev_node->lowCount == -1) {
                    blockCount--;
                    delete[] prev_node->data;
                }

                this->nextFree++;
                if (this->nextFree == this->totalSize) {
                    this->totalSize += this->blockSize;
                    this->pnodeArray.push_back(new PNode[this->blockSize]);
                    memset(this->pnodeArray[floor(this->totalSize / this->blockSize) - 1], 0xffffffff, (this->blockSize) * sizeof (PNode));
                }
            }

            stored++;
            return Result(true, en, prev_count);

        }

        template<typename T>
        void PTrie<T>::printMemStats() {
            cout << endl << "Encoding size;" << endl <<
                    "\t\t\t" << this->markingBitSize << endl;
            cout << "Lists:" << endl <<
                    "\t count \t\t" << this->blockCount << endl;
            cout << "Nodes:" << endl <<
                    "\t count \t\t" << this->nextFree - 1 << endl;
        }

        template<typename T>
        void PTrie<T>::printEncoding(bool* encoding, int length) {
            for (int i = 0; i < length; i++)
                cout << encoding[i];
            cout << endl;
        }
    }
}
#endif	/* PDATA_H */

