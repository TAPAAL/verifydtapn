/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "TimeDartLivenessPWList.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        std::pair<LivenessDart*, bool> TimeDartLivenessPWHashMap::Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* marking, int youngest, WaitingDart* parent, int upper) {
            discoveredMarkings++;
            TimeDartList& m = markings_storage[marking->HashKey()];
            for (TimeDartList::const_iterator iter = m.begin();
                    iter != m.end();
                    iter++) {
                if ((*iter)->getBase()->equals(*marking)) {
                    std::pair < LivenessDart*, bool> result(*iter, false);
                    (*iter)->setWaiting(min((*iter)->getWaiting(), youngest));

                    if ((*iter)->getWaiting() < (*iter)->getPassed()) {
                        if(options.GetTrace()){
                            waiting_list->Add((*iter)->getBase(), new TraceDart((*iter), parent, youngest, upper, marking->generatedBy));

                        } else {
                            waiting_list->Add((*iter)->getBase(), new WaitingDart((*iter), parent, youngest, upper));
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
            if(options.GetTrace()){

                waiting_list->Add(dart->getBase(), new TraceDart(dart, parent, youngest, upper, marking->generatedBy));

            } else {
                waiting_list->Add(dart->getBase(), new WaitingDart(dart, parent, youngest, upper));                
            }
            std::pair < LivenessDart*, bool> result(dart, true);
            return result;
        }

        WaitingDart* TimeDartLivenessPWHashMap::GetNextUnexplored() {
            return waiting_list->Peek();
        }

        void TimeDartLivenessPWHashMap::PopWaiting() {
            delete waiting_list->Pop();
        }

        void TimeDartLivenessPWHashMap::flushBuffer() {
            // Flush buffer if w has changed
            waiting_list->flushBuffer();
        }

        std::pair<LivenessDart*, bool> TimeDartLivenessPWPData::Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* marking, int youngest, WaitingDart* parent, int upper) {

            
            discoveredMarkings++;
            PData<LivenessDart>::Result res = passed.Add(marking);

                if (!res.isNew) {
                    LivenessDart* td = res.encoding.GetMetaData();
                    td->setBase(marking);
                    std::pair < LivenessDart*, bool> result(td, false);
                    td->setWaiting(min(td->getWaiting(), youngest));
                           
                    if (td->getWaiting() < td->getPassed()) {
                        EncodingStructure<WaitingDart*> es(res.encoding.GetRaw(), res.encoding.Size());

                        EncodingPointer<WaitingDart>* ewp = new EncodingPointer<WaitingDart > (es, res.pos);
                        WaitingDart *wd;
                        if(options.GetTrace()){
                            wd =  new TraceDart(td, parent, youngest, upper, marking->generatedBy);

                        } else {
                            wd = new WaitingDart(td, parent, youngest, upper);
                        }
                        ewp->encoding.SetMetaData(wd);
                        
                        waiting_list->Add(marking, ewp);
                        result.second = true;
                    }
                    return result;
                }
            
            stored++;
            LivenessDart* dart = new LivenessDart(marking, youngest, INT_MAX);
            res.encoding.SetMetaData(dart);
            
            EncodingStructure<WaitingDart*> es(res.encoding.GetRaw(), res.encoding.Size());
            EncodingPointer<WaitingDart>* ewp = new EncodingPointer<WaitingDart > (es, res.pos);
            WaitingDart *wd;
            if(options.GetTrace()){
                wd =  new TraceDart(dart, parent, youngest, upper, marking->generatedBy);

            } else {
                wd = new WaitingDart(dart, parent, youngest, upper);
            }
            ewp->encoding.SetMetaData(wd);
            
            waiting_list->Add(marking, ewp);
            std::pair < LivenessDart*, bool> result(dart, true);
            return result;
        }

        WaitingDart* TimeDartLivenessPWPData::GetNextUnexplored() {
            EncodingPointer<WaitingDart>* ewp =  waiting_list->Peek();
            WaitingDart* wd = ewp->encoding.GetMetaData();
            NonStrictMarkingBase* base = passed.EnumerateDecode(*((EncodingPointer<LivenessDart>*)ewp));
            wd->dart->setBase(base);
            return wd;
        }

        void TimeDartLivenessPWPData::PopWaiting() {
            EncodingPointer<WaitingDart>* ewp =  waiting_list->Pop();
            WaitingDart* wd = ewp->encoding.GetMetaData();
            delete wd->dart->getBase();
            delete wd;
            ewp->encoding.Release();
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
