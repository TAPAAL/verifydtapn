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
        
        virtual bool HasWaitingStates() = 0;
        virtual long long Size() const = 0;
        virtual bool Add(NonStrictMarking* marking) = 0;
	virtual NonStrictMarking* GetNextUnexplored() = 0;
	inline void SetMaxNumTokensIfGreater(int i){ if(i>maxNumTokensInAnyMarking) maxNumTokensInAnyMarking = i; };
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
	virtual bool HasWaitingStates() {
		return (waiting_list->Size() > 0);
	};

	virtual long long Size() const {
		return stored;
	};

public: // modifiers
	virtual bool Add(NonStrictMarking* marking);
	virtual NonStrictMarking* GetNextUnexplored();

public:
	HashMap markings_storage;
	WaitingList<NonStrictMarking>* waiting_list;
};

class PWListHybrid : public PWListBase {
        public:
            typedef unsigned int uint;
            //            typedef google::sparse_hash_map<size_t, EncodingList> HashMap;
            PData<MetaData>* passed;

        public:

            PWListHybrid(boost::shared_ptr<TAPN::TimedArcPetriNet>& tapn, WaitingList<EncodingPointer<MetaData> >* w_l, int knumber, int nplaces, int mage) :
            waiting_list(w_l) {
                discoveredMarkings = 0;
                passed = new PData<MetaData>(tapn, knumber,nplaces,mage);
            };
            virtual ~PWListHybrid();
            friend std::ostream& operator<<(std::ostream& out, PWListHybrid& x);

        public: // inspectors

            virtual bool HasWaitingStates() {
                return (waiting_list->Size() > 0);
            };

            virtual long long Size() const {
                return passed->stored;
            };

            void PrintMemStats() {
                passed->PrintMemStats();
            }

        public: // modifiers
            virtual bool Add(NonStrictMarking* marking);
            virtual NonStrictMarking* GetNextUnexplored();

            inline void SetMaxNumTokensIfGreater(int i) {
                if (i > maxNumTokensInAnyMarking) maxNumTokensInAnyMarking = i;
            };

        public:

             WaitingList<EncodingPointer<MetaData> >* waiting_list;
            int discoveredMarkings;
            int maxNumTokensInAnyMarking;
        };

std::ostream& operator<<(std::ostream& out, PWList& x);

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PWLIST_HPP_ */
