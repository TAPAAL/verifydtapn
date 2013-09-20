/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "PWList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

bool PWList::add(NonStrictMarking* marking){
	discoveredMarkings++;
	NonStrictMarkingList& m = markings_storage[marking->getHashKey()];
	for(NonStrictMarkingList::const_iterator iter = m.begin();
			iter != m.end();
			iter++){
		if((*iter)->equals(*marking)){
                    if(isLiveness){
                        marking->meta = (*iter)->meta;
                        if(!marking->meta->passed){
                                (*iter)->setGeneratedBy(marking->getGeneratedBy());
                                waiting_list->add(*iter, *iter);
                                return true;
                        }
                    }
                    return false;
		}
	}
        stored++;
	m.push_back(marking);
    marking->meta = new MetaData();
	waiting_list->add(marking, marking);
	return true;
}

NonStrictMarking* PWList::addToPassed(NonStrictMarking* marking){
		NonStrictMarkingList& m = markings_storage[marking->getHashKey()];
		for(NonStrictMarkingList::const_iterator iter = m.begin();
					iter != m.end();
					iter++){
				if((*iter)->equals(*marking)){
					return *iter;
				}
		}
		stored++;
		m.push_back(marking);
		marking->meta = new MetaData();
		return marking;
	}

NonStrictMarking* PWList::getNextUnexplored(){
	return waiting_list->pop();
}

PWList::~PWList() {
    // We don't care, it is deallocated on program execution done
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

        bool PWListHybrid::add(NonStrictMarking* marking) {

            discoveredMarkings++;
            // reset the encoding array

            PTrie<MetaData>::Result res = passed->add(marking);
            if(res.isNew){
                res.encoding.setMetaData(NULL);
                if(isLiveness){
                    MetaData* meta;
                    if(this->makeTrace){
                        meta = new MetaDataWithTrace();   
                        ((MetaDataWithTrace*)meta)->generatedBy = marking->getGeneratedBy();
                    } else {
                        meta = new MetaData();
                    }
                    res.encoding.setMetaData(meta);
                    marking->meta = meta;
                } else if(this->makeTrace){
                    MetaDataWithTraceAndEncoding* meta = new MetaDataWithTraceAndEncoding();
                    meta->generatedBy = marking->getGeneratedBy();
                    res.encoding.setMetaData(meta);
                    meta->ep = new EncodingPointer<MetaData > (res.encoding, res.pos);
                    meta->parent = parent;
                }
                this->waiting_list->add(marking, new EncodingPointer<MetaData > (res.encoding, res.pos));
            } else{
                if(isLiveness){
                        marking->meta = res.encoding.getMetaData();
                        if(this->makeTrace){
                            ((MetaDataWithTrace*)marking->meta)->generatedBy = marking->getGeneratedBy();
                        }
                }
                if(isLiveness && !marking->meta->passed){
                    this->waiting_list->add(marking, new EncodingPointer<MetaData > (res.encoding, res.pos));
                    return true;
                }
            }
            return res.isNew;
        }

        NonStrictMarking* PWListHybrid::getNextUnexplored() {
            EncodingPointer<MetaData>* p = waiting_list->pop();
            NonStrictMarkingBase* base = passed->enumerateDecode(*p);
            NonStrictMarking* m = new NonStrictMarking(*base);
            delete base;
            
            m->meta = p->encoding.getMetaData();
            
            if(this->makeTrace){
                if(isLiveness){
                        m->setGeneratedBy(((MetaDataWithTrace*)(m->meta))->generatedBy);
                } else {
                    this->parent = (MetaDataWithTraceAndEncoding*)(m->meta);
                }
            }
            if(isLiveness || !this->makeTrace){
                p->encoding.release();
                delete p;
            }
            return m;
        }

        PWListHybrid::~PWListHybrid() {
            // We don't care, it is deallocated on program execution done
        }


} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
