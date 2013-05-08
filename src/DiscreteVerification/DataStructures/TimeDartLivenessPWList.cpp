/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "TimeDartLivenessPWList.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        std::pair<LivenessDart*, bool> TimeDartLivenessPWHashMap::add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* marking, int youngest, WaitingDart* parent, int upper, int start) {
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
                        delete marking;
                    }

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
            return waiting_list->peek();
        }

        void TimeDartLivenessPWHashMap::popWaiting() {
            delete waiting_list->pop();
        }

        void TimeDartLivenessPWHashMap::flushBuffer() {
            // Flush buffer if w has changed
            waiting_list->flushBuffer();
        }

        std::pair<LivenessDart*, bool> TimeDartLivenessPWPData::add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* marking, int youngest, WaitingDart* parent, int upper, int start) {

            
            discoveredMarkings++;
            PData<LivenessDart>::Result res = passed.add(marking);

                if (!res.isNew) {
                    LivenessDart* td = res.encoding.getMetaData();
                    td->setBase(marking);
                    std::pair < LivenessDart*, bool> result(td, false);
                    td->setWaiting(min(td->getWaiting(), youngest));
                           
                    if (td->getWaiting() < td->getPassed()) {
                        EncodingStructure<WaitingDart*> es(res.encoding.getRaw(), res.encoding.Size());

                        EncodingPointer<WaitingDart>* ewp = new EncodingPointer<WaitingDart > (es, res.pos);
                        WaitingDart *wd;
                        if(options.getTrace()){
                            wd =  new TraceDart(td, parent, youngest, start, upper, marking->getGeneratedBy());

                        } else {
                            wd = new WaitingDart(td, parent, youngest, upper);
                        }
                        ewp->encoding.setMetaData(wd);
                        
                        waiting_list->add(marking, ewp);
                        result.second = true;
                    } else {
                        if(options.getTrace() == VerificationOptions::SOME){
                            EncodingStructure<WaitingDart*> es(res.encoding.getRaw(), res.encoding.Size());
                           ((EncodedLivenessDart*)td)->encoding = new EncodingPointer<WaitingDart > (es, res.pos);
                           result.first = td;
                        }
                    }
                    return result;
                }
            
            stored++;
            LivenessDart* dart;
            if(options.getTrace()){
                dart= new EncodedLivenessDart(marking, youngest, INT_MAX);
            } else {
                dart = new LivenessDart(marking, youngest, INT_MAX);
            }
            res.encoding.setMetaData(dart);
            
            EncodingStructure<WaitingDart*> es(res.encoding.getRaw(), res.encoding.Size());
            EncodingPointer<WaitingDart>* ewp = new EncodingPointer<WaitingDart > (es, res.pos);
            
            WaitingDart *wd;
            if(options.getTrace()){
                wd =  new TraceDart(dart, parent, youngest, start, upper, marking->getGeneratedBy());
                ((EncodedLivenessDart*)dart)->encoding = ewp;
            } else {
                wd = new WaitingDart(dart, parent, youngest, upper);
            }
            ewp->encoding.setMetaData(wd);
            
            waiting_list->add(marking, ewp);
            std::pair < LivenessDart*, bool> result(dart, true);
            return result;
        }

        WaitingDart* TimeDartLivenessPWPData::getNextUnexplored() {
            EncodingPointer<WaitingDart>* ewp =  waiting_list->peek();
            WaitingDart* wd = ewp->encoding.getMetaData();
            NonStrictMarkingBase* base = passed.enumerateDecode(*((EncodingPointer<LivenessDart>*)ewp));
            wd->dart->setBase(base);
            if(options.getTrace() == VerificationOptions::SOME){
                ((EncodedLivenessDart*)wd->dart)->encoding = ewp;
            }
            return wd;
        }

        void TimeDartLivenessPWPData::popWaiting() {
            EncodingPointer<WaitingDart>* ewp =  waiting_list->pop();
            WaitingDart* wd = ewp->encoding.getMetaData();
            delete wd->dart->getBase();
            delete wd;
            ewp->encoding.release();
            delete ewp;
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
