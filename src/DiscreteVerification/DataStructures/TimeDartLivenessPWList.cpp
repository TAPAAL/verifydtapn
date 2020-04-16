/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "DiscreteVerification/DataStructures/TimeDartLivenessPWList.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        std::pair<LivenessDart*, bool> TimeDartLivenessPWHashMap::add(NonStrictMarkingBase* marking, int youngest, WaitingDart* parent, int upper, int start) {
            discoveredMarkings++;
            TimeDartList& m = markings_storage[marking->getHashKey()];
            for (TimeDartList::const_iterator iter = m.begin();
                    iter != m.end();
                    iter++) {
                if ((*iter)->getBase()->equals(*marking)) {
                    std::pair < LivenessDart*, bool> result(*iter, false);
                    (*iter)->setWaiting(min((*iter)->getWaiting(), youngest));

                    if ((*iter)->getWaiting() < (*iter)->getPassed()) {
                        if(options.getTrace()){
                            waiting_list->add((*iter)->getBase(), new TraceDart((*iter), parent, youngest, start, upper, marking->getGeneratedBy()));

                        } else {
                            waiting_list->add((*iter)->getBase(), new WaitingDart((*iter), parent, youngest, upper));
                        }
                        result.second = true;
                    }
                    delete marking;
                    return result;
                }
            }
            stored++;
            LivenessDart* dart = new LivenessDart(marking, youngest, INT_MAX);
            m.push_back(dart);
            if(options.getTrace()){

                waiting_list->add(dart->getBase(), new TraceDart(dart, parent, youngest, start, upper, marking->getGeneratedBy()));

            } else {
                waiting_list->add(dart->getBase(), new WaitingDart(dart, parent, youngest, upper));                
            }
            std::pair < LivenessDart*, bool> result(dart, true);
            return result;
        }

        WaitingDart* TimeDartLivenessPWHashMap::getNextUnexplored() {
            WaitingDart* wd = waiting_list->peek();
            return wd;
        }

        void TimeDartLivenessPWHashMap::popWaiting() {
            delete waiting_list->pop();
        }

        void TimeDartLivenessPWHashMap::flushBuffer() {
            // Flush buffer if w has changed
            waiting_list->flushBuffer();
        }

        std::pair<LivenessDart*, bool> TimeDartLivenessPWPData::add(NonStrictMarkingBase* marking, int youngest, WaitingDart* parent, int upper, int start) {

            discoveredMarkings++;
            std::pair<bool, ptriepointer_t<LivenessDart*> > res = passed.insert(encoder.encode(marking));
            if (!res.first) {
                LivenessDart* td = res.second.get_meta();
                td->setBase(marking);
                std::pair < LivenessDart*, bool> result(td, false);
                td->setWaiting(min(td->getWaiting(), youngest));

                if (td->getWaiting() < td->getPassed()) {

                    WaitingDart *wd;
                    if(options.getTrace()){
                        wd =  new TraceDart(td, parent, youngest, start, upper, marking->getGeneratedBy());

                    } else {
                        wd = new WaitingDart(td, parent, youngest, upper);
                    }
                    
                    waiting_list->add(marking, waitingpair_t(wd, res.second));
                    result.second = true;
                } else {
                    if(options.getTrace() == VerificationOptions::SOME_TRACE){
                       ((EncodedLivenessDart*)td)->encoding = res.second;
                       result.first = td;
                    }
                }
                return result;
            }
            else
            {
            
                stored++;
                LivenessDart* dart;
                if(options.getTrace()){
                    dart = new EncodedLivenessDart(marking, youngest, INT_MAX);
                } else {
                    dart = new LivenessDart(marking, youngest, INT_MAX);
                }
                res.second.set_meta(dart);

                WaitingDart *wd;
                if(options.getTrace()){
                    wd =  new TraceDart(dart, parent, youngest, start, upper, marking->getGeneratedBy());
                    ((EncodedLivenessDart*)dart)->encoding = res.second;
                } else {
                    wd = new WaitingDart(dart, parent, youngest, upper);
                }

                waiting_list->add(marking, waitingpair_t(wd, res.second));
                std::pair < LivenessDart*, bool> result(dart, true);
                return result;
            }
        }

        WaitingDart* TimeDartLivenessPWPData::getNextUnexplored() {
            waitingpair_t ewp =  waiting_list->peek();
            WaitingDart* wd = ewp.first;
            NonStrictMarkingBase* base = encoder.decode(ewp.second);
            wd->dart->setBase(base);
            return wd;
        }

        void TimeDartLivenessPWPData::popWaiting() {
            waitingpair_t ewp =  waiting_list->pop();
            WaitingDart* wd = ewp.first;
            delete wd->dart->getBase();
            delete wd;
        }

        void TimeDartLivenessPWPData::flushBuffer() {
            // Flush buffer if w has changed
            waiting_list->flushBuffer();
        }

        std::ostream& operator<<(std::ostream& out, TimeDartLivenessPWHashMap& x) {
            out << "Passed and waiting:" << std::endl;
            for (TimeDartLivenessPWHashMap::HashMap::iterator iter = x.markings_storage.begin(); iter != x.markings_storage.end(); iter++) {
                for (TimeDartLivenessPWHashMap::TimeDartList::iterator m_iter = iter->second.begin(); m_iter != iter->second.end(); m_iter++) {
                    out << "- " << *m_iter << std::endl;
                }
            }
            out << "Waiting:" << std::endl << x.waiting_list;
            return out;
        }

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
