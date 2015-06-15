/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "PWList.hpp"
#include "ptrie.h"
#include "MarkingEncoder.h"
using namespace pgj;
namespace VerifyTAPN {
namespace DiscreteVerification {

bool PWList::add(NonStrictMarking* marking){
        std::cout << "a:" << *marking << std::endl;

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
        
        marking->meta->totalDelay = marking->calculateTotalDelay();
                
	waiting_list->add(marking, marking);
	return true;
}

NonStrictMarking* PWList::getNextUnexplored(){
    NonStrictMarking* m = waiting_list->pop();
    
    std::cout << "p:" << *m << std::endl;
    std::cout << "m:it->" <<    m->meta->inTrace << ":passed->"  << m->meta->passed << std::endl;
    return m;
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
            std::cout << "a:" << *marking << std::endl;
            discoveredMarkings++;
            // reset the encoding array

            assert(passed.isConsistent());

            std::pair<bool, ptriepointer<MetaData*> > res = passed.insert(encoder.encode(marking));

            if(res.first){
                res.second.setMeta(NULL);
                if(isLiveness){
                    MetaData* meta;
                    if(makeTrace){
                        meta = new MetaDataWithTrace();   
                        ((MetaDataWithTrace*)meta)->generatedBy = marking->getGeneratedBy();
                    } else {
                        meta = new MetaData();
                    }
                    res.second.setMeta(meta);
                    marking->meta = meta;
                    std::cout << "m:it->" << marking->meta->inTrace << ":passed->"  << marking->meta->passed << std::endl;
                } else if(makeTrace){
                    MetaDataWithTraceAndEncoding* meta = new MetaDataWithTraceAndEncoding();
                    meta->generatedBy = marking->getGeneratedBy();
                    res.second.setMeta(meta);
                    meta->ep = res.second;
                    meta->parent = parent;
                    
                    meta->totalDelay = marking->calculateTotalDelay();
                }
                waiting_list->add(marking, res.second);
            } else{
                if(isLiveness){
                        marking->meta = res.second.getMeta();
                        if(this->makeTrace){
                            ((MetaDataWithTrace*)marking->meta)->generatedBy = marking->getGeneratedBy();
                        }
                }
                if(isLiveness && !marking->meta->passed){
                    waiting_list->add(marking, res.second);
                    return true;
                }
            }
            return res.first;
        }

        NonStrictMarking* PWListHybrid::getNextUnexplored() {
            ptriepointer<MetaData*> p = waiting_list->pop();
            assert(passed.isConsistent());
            NonStrictMarkingBase* base = encoder.decode(p);
            assert(passed.isConsistent());
            NonStrictMarking* m = new NonStrictMarking(*base);
            delete base;
            
            m->meta = p.getMeta();

            
            if(makeTrace){
                if(isLiveness){
                        m->setGeneratedBy(((MetaDataWithTrace*)(m->meta))->generatedBy);
                } else {
                    parent = (MetaDataWithTraceAndEncoding*)(m->meta);
                }
            }
            std::cout << "p:" << *m << std::endl;
            std::cout << "m:it->" <<    m->meta->inTrace << ":passed->"  << m->meta->passed << std::endl;
            return m;
        }

        PWListHybrid::~PWListHybrid() {
            // We don't care, it is deallocated on program execution done
        }


} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
