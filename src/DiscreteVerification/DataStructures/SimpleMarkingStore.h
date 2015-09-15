/* 
 * File:   SimpleMarkingStore.h
 * Author: Peter G. Jensen
 *
 * Created on 14 September 2015, 11:34
 */

#ifndef SIMPLEMARKINGSTORE_H
#define	SIMPLEMARKINGSTORE_H
#include <vector>
#include "google/sparse_hash_set"
#include "MetaData.h"
#include "NonStrictMarkingBase.hpp"
#include "MarkingStore.h"


/**
 * Base class for storing markings
 */
namespace VerifyTAPN {
namespace DiscreteVerification {
template<typename T = MetaData*>
class SimpleMarkingStore : public MarkingStore<T>
{
    public:
        /**
         * Generic wrapper for a pointer to a marking
         */
        class Pointer : public MarkingStore<T>::Pointer {
            friend class SimpleMarkingStore<T>;
            protected:
                T meta;
                NonStrictMarkingBase* marking;
            public:
            Pointer(NonStrictMarkingBase* marking) 
                : meta(), marking(marking){};
                
            Pointer(NonStrictMarkingBase* marking, T& meta) 
                : meta(meta), marking(marking){};
                
            void set_meta_data(T& data)
            {
                meta = data;
            };
            
            virtual
            T& get_meta_data()
            {
                return meta;
            };
        };        
    protected:
        // We are just saving a bunch of pointers
        typedef std::vector<Pointer*> pointer_list;
        
        // set is unordered version of map. Fine for this, we need no ordering.
	typedef google::sparse_hash_set<size_t, pointer_list> map_t;  
        
        map_t store;
    public:
        
        /**
         * Default constructor
         */
        SimpleMarkingStore() : MarkingStore<T>(), store(256000) {};
        virtual ~SimpleMarkingStore()
        {            
            for(typename map_t::iterator it = store.begin();
                    it != store.end(); ++it )
            {
                for(typename pointer_list::iterator pit = it->begin(); 
                        pit != it->end(); ++pit)
                {
                    delete pit->marking;
                    delete *pit;
                }
            }
        }
        
        /**
         * 
         * @param m A marking to insert
         * @return PAIR (bool, Pointer). bool = true if inserted, false if not.
         * Pointer points to the newly inserted, or existing marking.
         */
        virtual
        typename
        MarkingStore<T>::result_t insert(NonStrictMarkingBase* m)
        {
            size_t key = m->getHashKey();
            typename
            map_t::iterator location = store.find(key);
            typename
            pointer_list::iterator pit = location->begin();
            for(;pit != location->end(); ++pit)
            {
                int cmp = (*pit)->marking->cmp(*m);
                if(cmp == 0)
                {
                    return result_t(false, *pit);
                }
                if(cmp > 0) break;
            }
            Pointer* p = new Pointer(m);
            location->insert(pit, p);
            return result_t(true, p);
        }
        
        /**
         * Method for reconstructing a marking from a given pointer. The 
         * template parameter decides (if needed) how the pointer is reconstructed 
         * @param p
         * @return an expanded marking of type U.
         */
        virtual
        NonStrictMarkingBase* expand(typename MarkingStore<T>::Pointer* p)
        {
            
            return static_cast<Pointer*>(p)->marking;
        }
        
        /**
         * Method for freeing/deleting markings returned from expand.
         * @param m a marking to free
         */
        virtual
        void free(NonStrictMarkingBase* m){
            // do nothing
        };
};
}
}


#endif	/* SIMPLEMARKINGSTORE_H */

