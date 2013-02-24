/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "PWList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

bool PWList::Add(NonStrictMarking* marking){
	discoveredMarkings++;
	NonStrictMarkingList& m = markings_storage[marking->HashKey()];
	for(NonStrictMarkingList::const_iterator iter = m.begin();
			iter != m.end();
			iter++){
		if((*iter)->equals(*marking)){
                    if(isLiveness){
                        marking->meta = (*iter)->meta;
                        if(!marking->meta->passed){
                                waiting_list->Add(*iter, *iter);
                                return true;
                        }
                    }
                    return false;
		}
	}
        stored++;
	m.push_back(marking);
        marking->meta = new MetaData();
	waiting_list->Add(marking, marking);
	return true;
}

NonStrictMarking* PWList::GetNextUnexplored(){
	return waiting_list->Pop();
}

PWList::~PWList() {
	// TODO Auto-generated destructor stub
}

std::ostream& operator<<(std::ostream& out, PWList& x){
	out << "Passed and waiting:" << std::endl;
	for(PWList::HashMap::iterator iter = x.markings_storage.begin(); iter != x.markings_storage.end(); iter++){
		for(PWList::NonStrictMarkingList::iterator m_iter = iter->second.begin(); m_iter != iter->second.end(); m_iter++){
			out << "- "<< *m_iter << std::endl;
		}
	}
	out << "Waiting:" << std::endl << x.waiting_list;
	return out;
}

        bool PWListHybrid::Add(NonStrictMarking* marking) {

            discoveredMarkings++;
            // reset the encoding array

            PData<MetaData>::Result res = passed->Add(marking);
            if(res.isNew){
                if(isLiveness){
                    MetaData* meta = new MetaData();
                    meta->inTrace = false;
                    meta->passed = false;
                    res.encoding.SetMetaData(meta);
                    marking->meta = meta;
                }
                this->waiting_list->Add(marking, new EncodingPointer<MetaData > (res.encoding, res.pos));
            } else{
                if(isLiveness){
                        marking->meta = res.encoding.GetMetaData();
                }
                if(isLiveness && !marking->meta->passed){
                    this->waiting_list->Add(marking, new EncodingPointer<MetaData > (res.encoding, res.pos));
                    return true;
                }
            }
            return res.isNew;
        }

        NonStrictMarking* PWListHybrid::GetNextUnexplored() {
            // TODO: Is this really it?
            EncodingPointer<MetaData>* p = waiting_list->Pop();
            NonStrictMarkingBase* base = passed->EnumerateDecode(*p);
            NonStrictMarking* m = new NonStrictMarking(*base);
            delete base;
            
            m->meta = p->encoding.GetMetaData();
            p->encoding.Release();
            return m;
        }

        PWListHybrid::~PWListHybrid() {
            // TODO Auto-generated destructor stub
        }

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
