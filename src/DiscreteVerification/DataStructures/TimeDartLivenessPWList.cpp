/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "TimeDartLivenessPWList.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        std::pair<TimeDart*, bool> TimeDartLivenessPWHashMap::Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* marking, int youngest, TimeDart* parent, int upper) {
            discoveredMarkings++;
            TimeDartList& m = markings_storage[marking->HashKey()];
            for (TimeDartList::const_iterator iter = m.begin();
                    iter != m.end();
                    iter++) {
                if ((*iter)->getBase()->equals(*marking)) {
                    std::pair < TimeDart*, bool> result(*iter, false);
                    (*iter)->setWaiting(min((*iter)->getWaiting(), youngest));

                    if ((*iter)->getWaiting() < (*iter)->getPassed()) {
                        waiting_list->Add((*iter)->getBase(), new WaitingDart((*iter), parent, youngest, upper));
                        result.second = true;
                    }

                    delete marking;

                    return result;
                }
            }
            stored++;
            TimeDart* dart = new TimeDart(marking, youngest, INT_MAX);
            m.push_back(dart);
            waiting_list->Add(dart->getBase(), new WaitingDart(dart, parent, youngest, upper));
            std::pair < TimeDart*, bool> result(dart, true);
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

        std::pair<TimeDart*, bool> TimeDartLivenessPWPData::Add(TAPN::TimedArcPetriNet* tapn, NonStrictMarkingBase* marking, int youngest, TimeDart* parent, int upper) {
            discoveredMarkings++;
            PData<TimeDart>::Result res = passed.Add(marking);
            std::pair < TimeDart*, bool> result;

            if (!res.isNew) {
                TimeDart* t = res.encoding.GetMetaData();
                t->setWaiting(min(t->getWaiting(), youngest));
                delete marking;
                if (t->getWaiting() < t->getPassed()) {
                    EncodingStructure<WaitingDart*> es;
                    es.Copy(res.encoding.GetRaw(), res.encoding.Size());
                    es.SetMetaData(new WaitingDart(t, parent, youngest, upper));
                    EncodingPointer<WaitingDart>* ewp = new EncodingPointer<WaitingDart > (es, res.pos);
                    waiting_list->Add(t->getBase(), ewp);
                    if (es.GetMetaData() == NULL) {
                        cout << "error" << endl;
                    }
                    return std::pair < TimeDart*, bool> (t, true);
                } else {
                    return std::pair < TimeDart*, bool> (t, false);
                }
            }

            stored++;
            TimeDart* dart = new TimeDart(marking, youngest, INT_MAX);
            WaitingDart* wd = new WaitingDart(dart, parent, youngest, upper);
            EncodingStructure<WaitingDart*> es;
            es.Copy(res.encoding.GetRaw(), res.encoding.Size());
            es.SetMetaData(wd);
            EncodingPointer<WaitingDart>* ewp = new EncodingPointer<WaitingDart > (es, res.pos);
            res.encoding.SetMetaData(dart);
            waiting_list->Add(marking, ewp);
            result.first = dart;
            result.second = true;
            if (es.GetMetaData() == NULL) {
                cout << "error" << endl;
            }
            return result;
        }

        WaitingDart* TimeDartLivenessPWPData::GetNextUnexplored() {
            return waiting_list->Peek()->encoding.GetMetaData();
        }

        void TimeDartLivenessPWPData::PopWaiting() {
            delete waiting_list->Pop();
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
