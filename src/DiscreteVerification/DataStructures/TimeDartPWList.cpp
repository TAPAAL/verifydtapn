/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "DiscreteVerification/DataStructures/TimeDartPWList.hpp"

namespace VerifyTAPN { namespace DiscreteVerification {
    TimeDartPWHashMap::~TimeDartPWHashMap() {
        // We don't care, it is deallocated on program execution done
    }

    bool
    TimeDartPWHashMap::add(NonStrictMarkingBase *marking, int youngest, WaitingDart *parent, int upper, int start) {
        discoveredMarkings++;
        TimeDartList &m = markings_storage[marking->getHashKey()];
        for (auto* iter : m) {
            if (iter->getBase()->equals(*marking)) {
                bool inWaiting = iter->getWaiting() < iter->getPassed();

                iter->setWaiting(std::min(iter->getWaiting(), youngest));

                if (iter->getWaiting() < iter->getPassed() && !inWaiting) {
                    waiting_list->add(iter->getBase(), iter);
                    if (this->trace) {
                        ((ReachabilityTraceableDart *) iter)->trace = new TraceDart(iter, parent, youngest,
                                                                                    start, upper,
                                                                                    marking->getGeneratedBy());
                        this->last = ((ReachabilityTraceableDart *) iter)->trace;
                    }
                }

                delete marking;

                return false;
            }
        }
        TimeDartBase *dart;
        if (this->trace) {
            dart = new ReachabilityTraceableDart(marking, youngest, std::numeric_limits<int32_t>::max());
            ((ReachabilityTraceableDart *) dart)->trace = new TraceDart(dart, parent, youngest, start, upper,
                                                                        marking->getGeneratedBy());
            this->last = ((ReachabilityTraceableDart *) (dart))->trace;
        } else {
            dart = new TimeDartBase(marking, youngest, std::numeric_limits<int32_t>::max());
        }
        stored++;
        m.push_back(dart);

        waiting_list->add(dart->getBase(), dart);
        return true;
    }

    TimeDartBase *TimeDartPWHashMap::getNextUnexplored() {
        return waiting_list->pop();
    }

    bool
    TimeDartPWPData::add(NonStrictMarkingBase *marking, int youngest, WaitingDart *parent, int upper, int start) {
        discoveredMarkings++;
        std::pair<bool, ptriepointer_t<TimeDartBase *> > res = passed.insert(encoder.encode(marking));


        if (!res.first) {
            TimeDartBase *t = res.second.get_meta();
            bool inWaiting = t->getWaiting() < t->getPassed();
            t->setWaiting(std::min(t->getWaiting(), youngest));

            if (t->getWaiting() < t->getPassed() && !inWaiting) {
                if (this->trace) {
                    ((EncodedReachabilityTraceableDart *) t)->trace = new TraceDart(t, parent, youngest, start,
                                                                                    upper,
                                                                                    marking->getGeneratedBy());
                    this->last = ((EncodedReachabilityTraceableDart *) t)->trace;
                }
                waiting_list->add(marking, res.second);
            }
            return false;
        }


        TimeDartBase *dart;
        if (this->trace) {
            dart = new EncodedReachabilityTraceableDart(marking, youngest, std::numeric_limits<int32_t>::max());
            ((EncodedReachabilityTraceableDart *) dart)->trace = new TraceDart(dart, parent, youngest, start, upper,
                                                                               marking->getGeneratedBy());
            this->last = ((ReachabilityTraceableDart *) (dart))->trace;
        } else {
            dart = new TimeDartBase(marking, youngest, std::numeric_limits<int32_t>::max());
        }
        stored++;
        res.second.set_meta(dart);
        waiting_list->add(marking, res.second);
        if (this->trace) {
            // if trace, create new (persistent) encodingpointer as regular one gets deleted every time we pop from waiting.
            ((EncodedReachabilityTraceableDart *) dart)->encoding = res.second;
        }
        return true;
    }

    TimeDartBase *TimeDartPWPData::getNextUnexplored() {

        ptriepointer_t<TimeDartBase *> p = waiting_list->pop();
        NonStrictMarkingBase *m = encoder.decode(p);
        TimeDartBase *dart = p.get_meta();
        dart->setBase(m);
        return dart;
    }

    std::ostream &operator<<(std::ostream &out, TimeDartPWHashMap &x) {
        out << "Passed and waiting:" << std::endl;
        for (auto &iter : x.markings_storage) {
            for (auto& m_iter : iter.second) {
                out << "- " << m_iter << std::endl;
            }
        }
        out << "Waiting:" << std::endl << x.waiting_list;
        return out;
    }

} } /* namespace VerifyTAPN */
