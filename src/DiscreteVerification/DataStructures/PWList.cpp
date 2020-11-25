/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "DiscreteVerification/DataStructures/PWList.hpp"
#include "DiscreteVerification/DataStructures/ptrie.h"

using namespace ptrie;
namespace VerifyTAPN { namespace DiscreteVerification {

    bool PWList::add(NonStrictMarking *marking) {

        discoveredMarkings++;
        NonStrictMarkingList &m = markings_storage[marking->getHashKey()];
        for (auto iter : m) {
            if (iter->equals(*marking)) {
                if (isLiveness) {
                    marking->meta = iter->meta;
                    if (!marking->meta->passed) {
                        iter->setGeneratedBy(marking->getGeneratedBy());
                        waiting_list->add(iter, iter);
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

    NonStrictMarking *PWList::getNextUnexplored() {
        NonStrictMarking *m = waiting_list->pop();
        return m;
    }

    PWList::~PWList() {
        // We don't care, it is deallocated on program execution done
    }

    std::ostream &operator<<(std::ostream &out, PWList &x) {
        out << "Passed and waiting:" << std::endl;
        for (auto &iter : x.markings_storage) {
            for (auto& m_iter : iter.second) {
                out << "- " << m_iter << std::endl;
            }
        }
        out << "Waiting:" << std::endl << x.waiting_list;
        return out;
    }

    bool PWListHybrid::add(NonStrictMarking *marking) {
        discoveredMarkings++;
        // reset the encoding array

        std::pair<bool, ptriepointer_t<MetaData *> > res = passed.insert(encoder.encode(marking));

        if (res.first) {
            res.second.set_meta(nullptr);
            if (isLiveness) {
                MetaData *meta;
                if (makeTrace) {
                    meta = new MetaDataWithTrace();
                    ((MetaDataWithTrace *) meta)->generatedBy = marking->getGeneratedBy();
                } else {
                    meta = new MetaData();
                }
                res.second.set_meta(meta);
                marking->meta = meta;
            } else if (makeTrace) {
                auto *meta = new MetaDataWithTraceAndEncoding();
                meta->generatedBy = marking->getGeneratedBy();
                res.second.set_meta(meta);
                meta->ep = res.second;
                meta->parent = parent;

                meta->totalDelay = marking->calculateTotalDelay();
            }
            waiting_list->add(marking, res.second);
        } else {
            if (isLiveness) {
                marking->meta = res.second.get_meta();
                if (this->makeTrace) {
                    ((MetaDataWithTrace *) marking->meta)->generatedBy = marking->getGeneratedBy();
                }
            }
            if (isLiveness && !marking->meta->passed) {
                waiting_list->add(marking, res.second);
                return true;
            }
        }
        return res.first;
    }

    NonStrictMarking *PWListHybrid::getNextUnexplored() {
        ptriepointer_t<MetaData *> p = waiting_list->pop();
        NonStrictMarking *m = encoder.decode(p);

        delete m->meta;
        m->meta = p.get_meta();


        if (makeTrace) {
            if (isLiveness) {
                m->setGeneratedBy(((MetaDataWithTrace *) (m->meta))->generatedBy);
            } else {
                parent = (MetaDataWithTraceAndEncoding *) (m->meta);
            }
        }
        return m;
    }

    PWListHybrid::~PWListHybrid() {
        // We don't care, it is deallocated on program execution done
    }


} } /* namespace VerifyTAPN */
