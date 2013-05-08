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
        class PData {
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


            PData(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, int knumber, int nplaces, int mage) :
            k(knumber),
            maxAge(mage + 1),
            numberOfPlaces(nplaces),
            countSize(ceil(log2((knumber ? knumber : 1)) + 1)),
            enumeratedOffset(ceil(log2((nplaces * (mage + 1))) + 1) + countSize),
            numberOfVariables(enumeratedOffset * (knumber ? knumber : 1)),
            cachesize(128),
            tapn(tapn) {
                overhead = MarkingEncoding::overhead(this->numberOfVariables);
                this->numberOfVariables += overhead;
                stored = 0;
                bddsize = cachesize;
                this->BDDArr.push_back(new PNode[this->bddsize]);
                memset(this->BDDArr[0], 0xffffffff, this->bddsize * sizeof (PNode));
                BDDArr[0][0].shadow = NULL;
                BDDArr[0][0].highCount = BDDArr[0][0].lowCount = 0;
                BDDArr[0][0].lowpos = BDDArr[0][0].highpos = 0;
                BDDArr[0][0].parent = 0;
                bddnext = 1;
                encoding = MarkingEncoding(this->numberOfVariables);
                listcount = 0;
                maxCount = sizeof (PNode) * 4 + sizeof (std::list<PNode>) * 4;

            };
            virtual ~PData();

            uint maxCount;

            struct PNode {
                MarkingEncoding* shadow;
                uint highpos;

                uint lowpos;

                short int highCount;
                short int lowCount;
                uint parent;
            };

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
                return &BDDArr[i / cachesize][i % cachesize];
            }
            NonStrictMarkingBase* enumerateDecode(const EncodingPointer<T>& pointer);

            int enumeratedEncoding(NonStrictMarkingBase* marking);
            const uint k;
            const uint maxAge;
            const uint numberOfPlaces;
            const uint countSize;
            const uint enumeratedOffset;
            uint numberOfVariables;
            const uint cachesize;

            boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn;

            MarkingEncoding encoding;
            vector<PNode*> BDDArr;
            uint bddsize;
            uint stored;
            uint bddnext;
            uint overhead;
            uint listcount;
        };

        template<typename T>
        PData<T>::~PData() {
        }


        template<typename T>
        int PData<T>::enumeratedEncoding(NonStrictMarkingBase* marking) {
            encoding.zero();

            int tc = 0;
            uint bitcount = 0;

            for (vector<Place>::const_iterator pi = marking->getPlaceList().begin();
                    pi != marking->getPlaceList().end();
                    pi++) { // for each place

                int pc = pi->place->GetIndex();

                for (TokenList::const_iterator ti = pi->tokens.begin(); // for each token-element
                        ti != pi->tokens.end();
                        ti++) {


                    int offset = tc * this->enumeratedOffset; // the offset of the variables for this token
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
                    bitcount = countSize;
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
                return ((tc - 1) * this->enumeratedOffset) + bitcount;
        }

        template<typename T>
        NonStrictMarkingBase* PData<T>::enumerateDecode(const EncodingPointer<T> &pointer) {
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

            var = this->numberOfVariables - 1;
            // foreach token
            uint data = 0;
            uint count = 0;
            for (int i = this->k - 1; i >= 0; i--) {
                uint offset = (this->enumeratedOffset * i) + this->overhead + this->countSize;
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
                offset -= this->countSize;
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
                    m->addTokenInPlace(tapn->getPlace(place), t);
                    data = 0;
                    count = 0;
                }
            }
            return m;
        }

        template<typename T>
        typename PData<T>::Result PData<T>::add(NonStrictMarkingBase* marking) {

            int encsize = this->enumeratedEncoding(marking) + overhead;
            // go through the PTrie as far as possible with the encoding of the marking
            uint c_count = 0;
            uint prev_count = 0;
            int var = overhead;
            do {

                prev_count = c_count;
                if (encoding.at(var)) {
                    c_count = BDDArr[c_count / cachesize][c_count % cachesize].highpos;

                } else {
                    c_count = BDDArr[c_count / cachesize][c_count % cachesize].lowpos;
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

            int size = this->numberOfVariables - var;
            MarkingEncoding en = MarkingEncoding(encoding.getRaw(), size, var, encsize);

            int ins = 0;

            for (; ins < listsize; ins++) {
                if (prev_node->shadow[ins] == en) {
                    break;
                }
            }
            if (ins != listsize) {
                return Result(false, prev_node->shadow[ins], prev_count);
            }

            en = MarkingEncoding(encoding, size, var, encsize);
            short unsigned int count;

            MarkingEncoding* nlist = new MarkingEncoding[listsize + 1];
            nlist[listsize] = en;

            for (int i = 0; i < listsize; i++) {
                nlist[i] = prev_node->shadow[i];
            }
            delete[] prev_node->shadow;
            prev_node->shadow = nlist;


            bool branch = encoding.at(var);
            if (branch) {
                count = (++prev_node->highCount);
            } else {
                count = (++prev_node->lowCount);
            }

            if (count > maxCount) {

                size--;
                int testclist = 0;
                int testnlist = 0;
                PNode* c_node = &this->BDDArr[floor(this->bddnext / this->cachesize)][this->bddnext % this->cachesize];
                if (branch) {
                    prev_node->highpos = this->bddnext;
                    c_node->shadow = new MarkingEncoding[testclist = prev_node->highCount];
                    prev_node->highCount = -1;
                    if (prev_node->lowCount > 0)
                        nlist = new MarkingEncoding[testnlist = prev_node->lowCount];
                    else
                        nlist = NULL;
                } else {
                    prev_node->lowpos = this->bddnext;
                    c_node->shadow = new MarkingEncoding[testclist = prev_node->lowCount];
                    prev_node->lowCount = -1;
                    if (prev_node->highCount > 0)
                        nlist = new MarkingEncoding[testnlist = prev_node->highCount];
                    else
                        nlist = NULL;
                }
                c_node->parent = prev_count;
                prev_count = this->bddnext;
                listcount++;
                c_node->lowCount = c_node->highCount = 0;
                c_node->lowpos = c_node->highpos = 0;
                uint npos = ((this->numberOfVariables - (size + 1)) % 8);

                MarkingEncoding nee;
                int clistcount = 0;
                int nlistcount = 0;
                for (int i = 0; i < listsize + 1; i++) {
                    if (prev_node->shadow[i].at(npos) == branch) {
                        if (!(size % 8)) {
                            nee = MarkingEncoding(prev_node->shadow[i], 8);
                            if (i == ins) {
                                en = nee;
                            }
                            prev_node->shadow[i].release();
                        } else {
                            nee = prev_node->shadow[i];
                        }
                        if (nee.at((npos + 1) % 8)) {
                            c_node->highCount++;
                        } else {
                            c_node->lowCount++;
                        }
                        c_node->shadow[clistcount] = nee;
                        clistcount++;
                    } else {
                        nlist[nlistcount] = prev_node->shadow[i];
                        nlistcount++;
                    }

                }
                delete[] prev_node->shadow;
                prev_node->shadow = nlist;

                if (prev_node->highCount == -1 && prev_node->lowCount == -1) {
                    listcount--;
                    delete[] prev_node->shadow;
                }

                this->bddnext++;
                if (this->bddnext == this->bddsize) {
                    this->bddsize += this->cachesize;
                    this->BDDArr.push_back(new PNode[this->cachesize]);
                    memset(this->BDDArr[floor(this->bddsize / this->cachesize) - 1], 0xffffffff, (this->cachesize) * sizeof (PNode));
                }
            }

            stored++;
            return Result(true, en, prev_count);

        }

        template<typename T>
        void PData<T>::printMemStats() {
            cout << endl << "Encoding size;" << endl <<
                    "\t\t\t" << this->numberOfVariables << endl;
            cout << "Lists:" << endl <<
                    "\t count \t\t" << this->listcount << endl;
            cout << "Nodes:" << endl <<
                    "\t count \t\t" << this->bddnext - 1 << endl;
        }

        template<typename T>
        void PData<T>::printEncoding(bool* encoding, int length) {
            for (int i = 0; i < length; i++)
                cout << encoding[i];
            cout << endl;
        }
    }
}
#endif	/* PDATA_H */

