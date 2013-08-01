/*
 * PWList.hpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#ifndef PWLIST_HPP_
#define PWLIST_HPP_

#include "WaitingList.hpp"
#include <iostream>
#include "google/sparse_hash_map"
#include  "NonStrictMarking.hpp"
#include "NonStrictMarking.hpp"
#include "WaitingList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {
    
    class PWListBase {
    public:
        PWListBase() : stored(0), discoveredMarkings(0), maxNumTokensInAnyMarking(-1), isLiveness(false) {};
        PWListBase(bool isLiveness) : stored(0), discoveredMarkings(0), maxNumTokensInAnyMarking(-1), isLiveness(isLiveness){};
        int stored;
    	int discoveredMarkings;
	int maxNumTokensInAnyMarking;
        bool isLiveness;
        
        virtual bool hasWaitingStates() = 0;
        virtual long long size() const = 0;
        virtual bool add(NonStrictMarking* marking) = 0;
	virtual NonStrictMarking* getNextUnexplored() = 0;
        virtual long long explored()= 0;
        virtual ~PWListBase(){};
	inline void setMaxNumTokensIfGreater(int i){ if(i>maxNumTokensInAnyMarking) maxNumTokensInAnyMarking = i; };
    };
    
class PWList : public PWListBase {
public:
	typedef std::vector<NonStrictMarking*> NonStrictMarkingList;
	typedef google::sparse_hash_map<size_t, NonStrictMarkingList> HashMap;
public:
	PWList() : PWListBase(false), markings_storage(256000), waiting_list(){};
	PWList(WaitingList<NonStrictMarking>* w_l, bool isLiveness) :PWListBase(isLiveness), markings_storage(256000), waiting_list(w_l) {};
	virtual ~PWList();
	friend std::ostream& operator<<(std::ostream& out, PWList& x);

public: // inspectors
	virtual bool hasWaitingStates() {
		return (waiting_list->size() > 0);
	};

	virtual long long size() const {
		return stored;
	};

        virtual long long explored() {return waiting_list->size();};
        
public: // modifiers
	virtual bool add(NonStrictMarking* marking);
	virtual NonStrictMarking* getNextUnexplored();

public:
	HashMap markings_storage;
	WaitingList<NonStrictMarking>* waiting_list;
};

class PWListHybrid : public PWListBase {
        public:
            typedef unsigned int uint;
            PTrie<MetaData>* passed;

        public:

            PWListHybrid(TAPN::TimedArcPetriNet& tapn, WaitingList<EncodingPointer<MetaData> >* w_l, int knumber, int nplaces, int mage, bool isLiveness, bool makeTrace) :
            PWListBase(isLiveness),
            waiting_list(w_l),
            makeTrace(makeTrace) {
                discoveredMarkings = 0;
                passed = new PTrie<MetaData>(tapn, knumber,nplaces,mage);
                parent = NULL;
            };
            virtual ~PWListHybrid() {
                // We don't care, it is deallocated on program execution done
                delete passed;
            }
            friend std::ostream& operator<<(std::ostream& out, PWListHybrid& x);

        public: // inspectors
            NonStrictMarking* decode(EncodingPointer<MetaData>* ep){
                NonStrictMarkingBase* base = this->passed->enumerateDecode(*ep);
                NonStrictMarking* m = new NonStrictMarking(*base);
                delete base;
                return m;
            };
            virtual bool hasWaitingStates() {
                return (waiting_list->size() > 0);
            };

            virtual long long size() const {
                return passed->stored;
            };
            virtual long long explored() {return waiting_list->size();};
            void printMemStats() {
                passed->printMemStats();
            }

        public: // modifiers
            virtual bool add(NonStrictMarking* marking);
            virtual NonStrictMarking* getNextUnexplored();

        public:

             WaitingList<EncodingPointer<MetaData> >* waiting_list;
             bool makeTrace;
             MetaDataWithTraceAndEncoding* parent;
};

std::ostream& operator<<(std::ostream& out, PWList& x);

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
