

#include "WorkflowPWList.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        WorkflowPWList::WorkflowPWList(WaitingList<NonStrictMarking*>* w_l) : PWList(w_l, false) {
        };

        bool WorkflowPWList::add(NonStrictMarking* marking) {
            discoveredMarkings++;
            NonStrictMarkingList& m = markings_storage[marking->getHashKey()];
            for (NonStrictMarkingList::const_iterator iter = m.begin();
                    iter != m.end();
                    iter++) {
                if ((*iter)->equals(*marking)) {
                    return false;
                }
            }
            stored++;
            m.push_back(marking);
            waiting_list->add(marking, marking);
            return true;
        }

        NonStrictMarking* WorkflowPWList::getCoveredMarking(NonStrictMarking* marking, bool useLinearSweep) {
            if (useLinearSweep) {
                for (HashMap::const_iterator iter = markings_storage.begin(); iter != markings_storage.end(); ++iter) {
                    for (NonStrictMarkingList::const_iterator m_iter = iter->second.begin(); m_iter != iter->second.end(); m_iter++) {
                        if ((*m_iter)->size() >= marking->size()) {
                            continue;
                        }

                        // Test if m_iter is covered by marking
                        PlaceList::const_iterator marking_place_iter = marking->getPlaceList().begin();

                        bool tokensCovered = true;
                        for (PlaceList::const_iterator m_place_iter = (*m_iter)->getPlaceList().begin(); m_place_iter != (*m_iter)->getPlaceList().end(); ++m_place_iter) {
                            while (marking_place_iter != marking->getPlaceList().end() && marking_place_iter->place != m_place_iter->place) {
                                ++marking_place_iter;
                            }

                            if (marking_place_iter == marking->getPlaceList().end()) {
                                tokensCovered = false;
                                break; // Place not covered in marking
                            }

                            TokenList::const_iterator marking_token_iter = marking_place_iter->tokens.begin();
                            for (TokenList::const_iterator m_token_iter = m_place_iter->tokens.begin(); m_token_iter != m_place_iter->tokens.end(); ++m_token_iter) {
                                while (marking_token_iter != marking_place_iter->tokens.end() && marking_token_iter->getAge() != m_token_iter->getAge()) {
                                    ++marking_token_iter;
                                }

                                if (marking_token_iter == marking_place_iter->tokens.end() || marking_token_iter->getCount() < m_token_iter->getCount()) {
                                    tokensCovered = false;
                                    break;
                                }
                            }

                            if (!tokensCovered) break;
                        }

                        if (tokensCovered) {
                            return *m_iter;
                        }
                    }
                }
            } else {
                vector<NonStrictMarking*> coveredMarkings;
                coveredMarkings.push_back(new NonStrictMarking(*marking));
                for (PlaceList::const_iterator p_iter = marking->getPlaceList().begin(); p_iter != marking->getPlaceList().end(); ++p_iter) {
                    for (TokenList::const_iterator t_iter = p_iter->tokens.begin(); t_iter != p_iter->tokens.end(); ++t_iter) {
                        for (int i = 1; i <= t_iter->getCount(); ++i) {
                            vector<NonStrictMarking*> toAdd;
                            for (vector<NonStrictMarking*>::iterator iter = coveredMarkings.begin(); iter != coveredMarkings.end(); ++iter) {
                                NonStrictMarking* new_marking = new NonStrictMarking(**iter);
                                for (int ii = i; ii > 0; --ii) {
                                    new_marking->removeToken(p_iter->place->getIndex(), t_iter->getAge());
                                }
                                toAdd.push_back(new_marking);
                            }
                            for (vector<NonStrictMarking*>::iterator iter = toAdd.begin(); iter != toAdd.end(); ++iter) {
                                coveredMarkings.push_back(*iter);
                            }
                        }
                    }
                }

                bool isFirst = true;
                for (vector<NonStrictMarking*>::iterator iter = coveredMarkings.begin(); 
                        iter != coveredMarkings.end(); ++iter) {
                    if (isFirst) {
                        isFirst = false;
                        continue;
                    }
                    NonStrictMarking* covered = lookup(*iter);
                    if (covered != NULL) {
                        // cleanup
                        for (;iter != coveredMarkings.end(); ++iter)
                        {
                            delete *iter;
                        }
                        return covered;
                    }
                    delete *iter;
                }
                
            }
            return NULL;
        }

        NonStrictMarking* WorkflowPWList::getUnpassed() {
            for (HashMap::iterator hmiter = markings_storage.begin(); hmiter != markings_storage.end(); hmiter++) {
                for (NonStrictMarkingList::const_iterator iter = hmiter->second.begin();
                        iter != hmiter->second.end();
                        iter++) {
                    if (!(*iter)->meta->passed) {
                        return *iter;
                    }
                }
            }
            return NULL;
        }

        NonStrictMarking* WorkflowPWList::addToPassed(
                                        NonStrictMarking* marking, bool strong) 
        {  
            discoveredMarkings++;
            NonStrictMarking* existing = lookup(marking);
            if (existing != NULL) {
                last = existing;                
                return existing;
            } else {
                last = marking;
                NonStrictMarkingList& m = markings_storage[marking->getHashKey()];
                stored++;
                m.push_back(marking);

                if(strong) marking->meta = new MetaData();
                else marking->meta = new WorkflowSoundnessMetaData();

                return NULL;
            }
        }

        NonStrictMarking* WorkflowPWList::lookup(NonStrictMarking* marking) {
            NonStrictMarkingList& m = markings_storage[marking->getHashKey()];
            for (NonStrictMarkingList::const_iterator iter = m.begin();
                    iter != m.end();
                    iter++) {
                if ((*iter)->equals(*marking)) {
                    return *iter;
                }
            }
            return NULL;
        }


        WorkflowPWListHybrid::WorkflowPWListHybrid(
                            TAPN::TimedArcPetriNet& tapn,  
                            WaitingList<ptriepointer_t<MetaData*> >* w_l, 
                            int knumber, 
                            int nplaces, 
                            int mage) 
            : PWListHybrid(tapn, w_l, knumber, nplaces, mage, false, true),
                visitor(encoder)
        {
            
        }
        
        WorkflowPWListHybrid::~WorkflowPWListHybrid()
        {
            ptriepointer_t<MetaData*> begin = passed.begin();
            while(begin != passed.end())
            {
                delete begin.get_meta();
                ++ begin;
            }
        }
        
    	NonStrictMarking* WorkflowPWListHybrid::getCoveredMarking
                                (NonStrictMarking* marking, bool useLinearSweep)
        {
            visitor.set_target(marking, last_pointer);
            passed.visit(visitor);
            if(visitor.found())
            {
                NonStrictMarking* m = visitor.decode();
                return m;
            }
            else
            {
                return NULL;
            }
        }
        
        NonStrictMarking* WorkflowPWListHybrid::getUnpassed()
        {
            ptriepointer_t<MetaData*> it = passed.last();
            for(; it != passed.rend(); --it)
            {
                if(!it.get_meta()->passed)
                {
                    NonStrictMarking* m = encoder.decode(it);
                    m->meta = it.get_meta();
                    return m;
                }
            }
            return NULL;
        }
        
    	bool WorkflowPWListHybrid::add(NonStrictMarking* marking)
        {
            discoveredMarkings++;
            std::pair<bool, ptriepointer_t<MetaData*> > res = 
                                        passed.insert(encoder.encode(marking));
            
            if (!res.first) {
                return false;
            }
            else
            {
                MetaDataWithTraceAndEncoding* meta = 
                                        new MetaDataWithTraceAndEncoding();
                meta->generatedBy = marking->getGeneratedBy();
                meta->ep = res.second;
                meta->parent = parent;
                meta->totalDelay = marking->calculateTotalDelay();
                
                res.second.set_meta(meta);
                
                stored++;
                
                // using min first waiting-list, weight is allready in pointer
                waiting_list->add(NULL, res.second);
                return true;           
            }
        }
        
        NonStrictMarking* WorkflowPWListHybrid::addToPassed
                                    (NonStrictMarking* marking, bool strong)
        {
            discoveredMarkings++;
            std::pair<bool, ptriepointer_t<MetaData*> > res = 
                                        passed.insert(encoder.encode(marking));


            
            if (!res.first) {
                NonStrictMarking* old = encoder.decode(res.second);
                
                MetaDataWithTraceAndEncoding* meta = 
                        (MetaDataWithTraceAndEncoding*)res.second.get_meta();
                old->setGeneratedBy(meta->generatedBy);
                old->meta = meta;
                last_pointer = res.second;
                return old;
            } else {
                stored++;

                if(strong) marking->meta = new MetaDataWithTraceAndEncoding();
                else marking->meta = new WorkflowSoundnessMetaDataWithEncoding();
                
                MetaDataWithTraceAndEncoding* meta = 
                                (MetaDataWithTraceAndEncoding*) marking->meta;
                meta->ep = res.second;
                meta->parent = this->parent;
                meta->generatedBy = marking->getGeneratedBy();
                meta->totalDelay = marking->meta->totalDelay;
                
                res.second.set_meta(meta);
                last_pointer = res.second;
                return NULL;
            }
        }
        
        void WorkflowPWListHybrid::addLastToWaiting()
        {
            // using min first waiting-list, weight is allready in pointer
            waiting_list->add(NULL, last_pointer);
        }
        
        
        

    }
}
