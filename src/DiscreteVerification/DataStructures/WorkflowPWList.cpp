

#include "DiscreteVerification/DataStructures/WorkflowPWList.hpp"

namespace VerifyTAPN { namespace DiscreteVerification {

    WorkflowPWList::WorkflowPWList(WaitingList<NonStrictMarking *> *w_l) : PWList(w_l, false) {
    }

    bool WorkflowPWList::add(NonStrictMarking *marking) {
        discoveredMarkings++;
        NonStrictMarkingList &m = markings_storage[marking->getHashKey()];
        for (auto* iter : m) {
            if (iter->equals(*marking)) {
                return false;
            }
        }
        stored++;
        m.push_back(marking);
        waiting_list->add(marking, marking);
        return true;
    }

    NonStrictMarking *WorkflowPWList::getCoveredMarking(NonStrictMarking *marking, bool useLinearSweep) {
        if (useLinearSweep) {
            for (auto& el : markings_storage) {
                for (auto* m_iter : el.second) {
                    if (m_iter->size() >= marking->size()) {
                        continue;
                    }

                    // Test if m_iter is covered by marking
                    auto marking_place_iter = marking->getPlaceList().begin();

                    bool tokensCovered = true;
                    for (auto& m_place_iter : m_iter->getPlaceList()) {
                        while (marking_place_iter != marking->getPlaceList().end() &&
                               marking_place_iter->place != m_place_iter.place) {
                            ++marking_place_iter;
                        }

                        if (marking_place_iter == marking->getPlaceList().end()) {
                            tokensCovered = false;
                            break; // Place not covered in marking
                        }

                        auto marking_token_iter = marking_place_iter->tokens.begin();
                        for (auto& m_token_iter : m_place_iter.tokens) {
                            while (marking_token_iter != marking_place_iter->tokens.end() &&
                                   marking_token_iter->getAge() != m_token_iter.getAge()) {
                                ++marking_token_iter;
                            }

                            if (marking_token_iter == marking_place_iter->tokens.end() ||
                                marking_token_iter->getCount() < m_token_iter.getCount()) {
                                tokensCovered = false;
                                break;
                            }
                        }

                        if (!tokensCovered) break;
                    }

                    if (tokensCovered) {
                        return m_iter;
                    }
                }
            }
        } else {
            std::vector<NonStrictMarking *> coveredMarkings;
            coveredMarkings.push_back(new NonStrictMarking(*marking));
            for (const auto &p_iter : marking->getPlaceList()) {
                for (auto& t_iter : p_iter.tokens) {
                    for (int i = 1; i <= t_iter.getCount(); ++i) {
                        std::vector<NonStrictMarking *> toAdd;
                        for (auto &coveredMarking : coveredMarkings) {
                            auto *new_marking = new NonStrictMarking(*coveredMarking);
                            for (int ii = i; ii > 0; --ii) {
                                new_marking->removeToken(p_iter.place->getIndex(), t_iter.getAge());
                            }
                            toAdd.push_back(new_marking);
                        }
                        for (auto &iter : toAdd) {
                            coveredMarkings.push_back(iter);
                        }
                    }
                }
            }

            bool isFirst = true;
            for (auto iter = coveredMarkings.begin();
                 iter != coveredMarkings.end(); ++iter) {
                if (isFirst) {
                    isFirst = false;
                    continue;
                }
                NonStrictMarking *covered = lookup(*iter);
                if (covered != nullptr) {
                    // cleanup
                    for (; iter != coveredMarkings.end(); ++iter) {
                        delete *iter;
                    }
                    return covered;
                }
                delete *iter;
            }

        }
        return nullptr;
    }

    NonStrictMarking *WorkflowPWList::getUnpassed() {
        for (auto &hmiter : markings_storage) {
            for (auto iter = hmiter.second.begin();
                 iter != hmiter.second.end();
                 iter++) {
                if (!(*iter)->meta->passed) {
                    return *iter;
                }
            }
        }
        return nullptr;
    }

    NonStrictMarking *WorkflowPWList::addToPassed(
            NonStrictMarking *marking, bool strong) {
        discoveredMarkings++;
        NonStrictMarking *existing = lookup(marking);
        if (existing != nullptr) {
            last = existing;
            return existing;
        } else {
            last = marking;
            NonStrictMarkingList &m = markings_storage[marking->getHashKey()];
            stored++;
            m.push_back(marking);

            if (strong) marking->meta = new MetaData();
            else marking->meta = new WorkflowSoundnessMetaData();

            return nullptr;
        }
    }

    NonStrictMarking *WorkflowPWList::lookup(NonStrictMarking *marking) {
        NonStrictMarkingList &m = markings_storage[marking->getHashKey()];
        for (auto iter : m) {
            if (iter->equals(*marking)) {
                return iter;
            }
        }
        return nullptr;
    }


    WorkflowPWListHybrid::WorkflowPWListHybrid(
            TAPN::TimedArcPetriNet &tapn,
            WaitingList<ptriepointer_t<MetaData *> > *w_l,
            int knumber,
            int nplaces,
            int mage)
            : PWListHybrid(tapn, w_l, knumber, nplaces, mage, false, true),
              visitor(encoder) {

    }

    WorkflowPWListHybrid::~WorkflowPWListHybrid() {
        ptriepointer_t<MetaData *> begin = passed.begin();
        while (begin != passed.end()) {
            delete begin.get_meta();
            ++begin;
        }
    }

    NonStrictMarking *WorkflowPWListHybrid::getCoveredMarking
            (NonStrictMarking *marking, bool useLinearSweep) {
        visitor.set_target(marking, last_pointer);
        passed.visit(visitor);
        if (visitor.found()) {
            NonStrictMarking *m = visitor.decode();
            return m;
        } else {
            return nullptr;
        }
    }

    NonStrictMarking *WorkflowPWListHybrid::getUnpassed() {
        ptriepointer_t<MetaData *> it = passed.last();
        for (; it != passed.rend(); --it) {
            if (!it.get_meta()->passed) {
                NonStrictMarking *m = encoder.decode(it);
                m->meta = it.get_meta();
                return m;
            }
        }
        return nullptr;
    }

    bool WorkflowPWListHybrid::add(NonStrictMarking *marking) {
        discoveredMarkings++;
        std::pair<bool, ptriepointer_t<MetaData *> > res =
                passed.insert(encoder.encode(marking));

        if (!res.first) {
            return false;
        } else {
            auto *meta =
                    new MetaDataWithTraceAndEncoding();
            meta->generatedBy = marking->getGeneratedBy();
            meta->ep = res.second;
            meta->parent = parent;
            meta->totalDelay = marking->calculateTotalDelay();

            res.second.set_meta(meta);

            stored++;

            // using min first waiting-list, weight is allready in pointer
            waiting_list->add(nullptr, res.second);
            return true;
        }
    }

    NonStrictMarking *WorkflowPWListHybrid::addToPassed
            (NonStrictMarking *marking, bool strong) {
        discoveredMarkings++;
        std::pair<bool, ptriepointer_t<MetaData *> > res =
                passed.insert(encoder.encode(marking));


        if (!res.first) {
            NonStrictMarking *old = encoder.decode(res.second);

            auto *meta =
                    (MetaDataWithTraceAndEncoding *) res.second.get_meta();
            old->setGeneratedBy(meta->generatedBy);
            old->meta = meta;
            last_pointer = res.second;
            return old;
        } else {
            stored++;

            if (strong) marking->meta = new MetaDataWithTraceAndEncoding();
            else marking->meta = new WorkflowSoundnessMetaDataWithEncoding();

            auto *meta =
                    (MetaDataWithTraceAndEncoding *) marking->meta;
            meta->ep = res.second;
            meta->parent = this->parent;
            meta->generatedBy = marking->getGeneratedBy();
            meta->totalDelay = marking->meta->totalDelay;

            res.second.set_meta(meta);
            last_pointer = res.second;
            return nullptr;
        }
    }

    void WorkflowPWListHybrid::addLastToWaiting() {
        // using min first waiting-list, weight is allready in pointer
        waiting_list->add(nullptr, last_pointer);
    }
} }