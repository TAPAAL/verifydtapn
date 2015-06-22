/* 
 * File:   ptrie.h
 * Author: Peter G. Jensen
 *
 * Created on 10 June 2015, 18:44
 */

#ifndef PTRIE_H
#define	PTRIE_H

#include "binarywrapper.h"
#include "visitor.h"
#include <assert.h>
#include <limits>
#include <vector>
#include <stdint.h>
#include <stack>

namespace ptrie
{
    typedef uint32_t uint;
    typedef unsigned char uchar;
    
    template<typename T>
    class ptrie_t;
    
    template<typename T>
    class ptriepointer_t
    {
        typedef binarywrapper_t<T> encoding_t;
        public:
            ptrie_t<T>* container;
            uint index;
        public:
            ptriepointer_t(ptrie_t<T>* container, uint i); 
            T get_meta() const;
            void set_meta(T);
            uint write_partial_encoding(encoding_t&) const;
            encoding_t& remainder() const;
            
            ptriepointer_t() : container(NULL), index(0) {};
            
            ptriepointer_t<T>& operator=(const ptriepointer_t<T>&);
            ptriepointer_t<T>& operator++();
            ptriepointer_t<T>& operator--();
            bool operator==(const ptriepointer_t<T>& other);
            bool operator!=(const ptriepointer_t<T>& other);
            
    };
    
    template<typename T>
    ptriepointer_t<T>::ptriepointer_t(ptrie_t<T>* container, uint i) : 
    container(container), index(i)
    {        
    }
    
    template<typename T>

    ptriepointer_t<T>& ptriepointer_t<T>::operator=
                                                (const ptriepointer_t<T>& other)
    {
        container = other.container;
        index = other.index;
        return *this;
    }
    
    template<typename T>
    ptriepointer_t<T>& ptriepointer_t<T>::operator++()
    {
        ++index;
        assert(index <= container->_next_free_entry);
        return *this;
    }
    
    template<typename T>
    ptriepointer_t<T>& ptriepointer_t<T>::operator--()
    {
        --index;
        return *this;
    }
    
    template<typename T>
    bool ptriepointer_t<T>::operator==(const ptriepointer_t<T>& other)
    {
        return other.container == container && other.index == index;
    }
    
    template<typename T>
    bool ptriepointer_t<T>::operator!=(const ptriepointer_t<T>& other)
    {
        return !(*this == other);
    }
    
    /*
    template<typename T>
    void swap(ptriepointer<T>& lhs, ptriepointer<T>& rhs)
    {
        ptrie<T>* container = lhs.container;
        uint index = lhs.index;
        lhs.container = rhs.container;
        lhs.index = rhs.index;
        rhs.index = index;
        rhs.container = container;
    }*/
    
    template<typename T>
    T ptriepointer_t<T>::get_meta() const
    {
        return container->get_entry(index)->_data.get_meta();
    }
    
    template<typename T>
    void ptriepointer_t<T>::set_meta(T val)
    {
        container->get_entry(index)->_data.set_meta(val);
    }
    
    template<typename T>
    uint ptriepointer_t<T>::write_partial_encoding(encoding_t& encoding) const
    {
        return container->writePathToRoot(index, encoding);
    }
    
    template<typename T>
    binarywrapper_t<T>& ptriepointer_t<T>::remainder() const
    {
        return container->get_entry(index)->_data;
    }
    
    template<typename T>
    class ptrie_t {
        typedef binarywrapper_t<T> encoding_t;  
        friend class ptriepointer_t<T>;
        private:
            
            // nodes in the tree
            struct node_t
            {
                uint _highpos;           // branches
                uint _lowpos;
                short int _highcount;    // bucket-counts
                short int _lowcount;
                uint _parent;            // for back-traversal
                uint* _entries;          // back-pointers to data-array up to date
            };
            
            struct entry_t
            {
                encoding_t _data;       // remainder-data, not in path of tree 
                uint _nodeindex;        // index of node
            };
            
            const uint _blocksize;   // number to allocate at a time
            uint _next_free_node;      // location of next vacant node-location
            uint _next_free_entry;     // location of next vacant entry-node
            
            std::vector<node_t*> _nodevector;    // Vector of arrays of nodes in tree
            std::vector<entry_t*> _entryvector;  // Vector of remainders
            
            short int _threshold;
            
        protected:
            node_t* get_node(uint index) const;
            uint new_node();
            
            entry_t* get_entry(uint index) const;
            uint new_entry();
            
            bool best_match(const encoding_t& encoding, uint& tree_pos , uint& e_index,
                                uint& enc_pos, uint& b_index, uint& bucketsize);
            
            uint split_node(node_t* node, uint tree_pos, uint enc_pos, 
                    uint bucketsize, bool branch);

            uint writePathToRoot(uint n_index, encoding_t& encoding) const;
            
        public:
            ptrie_t();
            ~ptrie_t();

            std::pair<bool, ptriepointer_t<T> > insert(const encoding_t& encoding);
            std::pair<bool, ptriepointer_t<T> > find(const encoding_t& encoding);
            bool consistent() const;
            uint size() const { return _next_free_entry; }
            ptriepointer_t<T> begin();
            ptriepointer_t<T> end();
            ptriepointer_t<T> rbegin();
            ptriepointer_t<T> rend();
            
            void visit(visitor_t<T>&);
    };
    
    template<typename T>
    ptrie_t<T>::~ptrie_t() {
        size_t deleted = 0;
        for(size_t i = 0; i < _nodevector.size(); ++i)
        {

            for(size_t b = 0; b < _blocksize; ++b)
            {
                if(deleted >= _next_free_node) break;
                ++deleted;
                delete[] _nodevector[i][b]._entries;
            }
            delete[] _nodevector[i];
        }
        deleted = 0;
        _nodevector.clear();
        for(size_t i = 0; i < _entryvector.size(); ++i)
        {
            for(size_t b = 0; b < _blocksize; ++b)
            {
                if(deleted >= _next_free_entry) break;
                ++deleted;
                _entryvector[i][b]._data.release();
            }
            delete[] _entryvector[i];
        }
        _entryvector.clear();
    }
    
    template<typename T>
    ptrie_t<T>::ptrie_t() :
            _blocksize(1024), 
            _next_free_node(1), 
            _next_free_entry(0), 
            _nodevector(), 
            _entryvector(), 
            _threshold(sizeof(node_t) * 8)
    {
        _nodevector.push_back(new node_t[_blocksize]);
        _nodevector[0][0]._entries = NULL;
        _nodevector[0][0]._highcount = 0;
        _nodevector[0][0]._lowcount = 0;
        _nodevector[0][0]._highpos = 0;
        _nodevector[0][0]._lowpos = 0;
        _nodevector[0][0]._parent = 0;
    }
    
    template<typename T>
    ptriepointer_t<T> ptrie_t<T>::begin()
    {
        return ptriepointer_t<T>(this, 0);
    }
    
    template<typename T>
    ptriepointer_t<T> ptrie_t<T>::end()
    {
        return ptriepointer_t<T>(this, _next_free_entry);
    }
    
    template<typename T>
    ptriepointer_t<T> ptrie_t<T>::rbegin()
    {
        return ptriepointer_t<T>(this, _next_free_entry-1);
    }
    
    template<typename T>
    ptriepointer_t<T> ptrie_t<T>::rend()
    {
        return ptriepointer_t<T>(this, std::numeric_limits<uint>::max());
    }
    
    template<typename T>
    typename ptrie_t<T>::node_t*
    ptrie_t<T>::get_node(uint index) const
    {
        assert(index < _next_free_node);
        return &_nodevector[index / _blocksize][index % _blocksize];
    }
    
    template<typename T>
    uint ptrie_t<T>::new_node()
    {
        uint next = _next_free_node;
        if(next % _blocksize == 0)
        {
            _nodevector.push_back(new node_t[_blocksize]);
        }
        
        ++_next_free_node;
        return next;
    }
    
    template<typename T>
    typename ptrie_t<T>::entry_t* 
    ptrie_t<T>::get_entry(uint index) const
    {
        assert(index < _next_free_entry);
        return &_entryvector[index / _blocksize][index % _blocksize];
    }
    
    template<typename T>
    uint ptrie_t<T>::new_entry()
    {
        uint next = _next_free_entry;
        if(next % _blocksize == 0)
        {
            _entryvector.push_back(new entry_t[_blocksize]);
        }
        
        ++_next_free_entry;
        return next;
    }
    
    template<typename T>
    bool ptrie_t<T>::consistent() const
    {
        return true;
        assert(_next_free_node >= _nodevector.size());
        assert(_next_free_entry >= _entryvector.size());
        for(size_t i = 0; i < _next_free_node; ++i)
        {
            node_t* node = get_node(i);
            assert(node->_highpos < _next_free_node);
            assert(node->_lowpos < _next_free_node);
            assert(node->_parent < _next_free_node);
            assert(node->_parent == 0 && i == 0 || node->_parent < i);
            assert((node->_highpos > i && node->_highcount == -1) 
                    || (node->_highpos == 0 && node->_highcount >= 0));
            assert((node->_lowpos > i && node->_lowcount == -1) 
                    || (node->_lowpos == 0 && node->_lowcount >= 0));
            
            size_t bucket = 0;
            
            if(node->_highcount > 0)
                bucket += node->_highcount;
            if(node->_lowcount > 0)
                bucket += node->_lowcount;

            assert(node->_entries != NULL || bucket == 0);
            for(size_t e = 0; e < bucket; ++e)
            {
                assert(e < _next_free_entry);
                assert(get_entry(node->_entries[e])->_nodeindex == i);
            }
        }
        return true;
    }
    
    template<typename T>
    uint ptrie_t<T>::writePathToRoot(uint e_index, encoding_t& encoding) const
    {
        entry_t* ent = get_entry(e_index);
        size_t count = 0;
        uint c_index = ent->_nodeindex;
        node_t* node = get_node(c_index);
        assert(consistent());
        while(c_index != 0)
        {
            uint p_index = node->_parent;
            node = get_node(p_index);
            bool branch = c_index == node->_highpos;
            encoding.set(count, branch);
            ++count;
            c_index = p_index;
        }
        assert(consistent());
        return count;
    }
    
    template<typename T>
    void ptrie_t<T>::visit(visitor_t<T>& visitor)
    {
        std::stack<std::pair<uint, uint> > waiting;
        waiting.push(std::pair<uint, uint>(0, 0));

        bool stop = false;
        do{
            uint distance = waiting.top().first;
            uint n_index = waiting.top().second;
            waiting.pop();
            
            visitor.back(distance);
            
            node_t* node = get_node(n_index);
            bool skip = false;
            do
            {
                if(node->_highpos != 0)
                {
                    waiting.push(std::pair<uint, uint>(distance, node->_highpos));
                }                 
                
                if(node->_lowpos == 0) break;
                else
                {
                    node_t* node = get_node(node->_lowpos);

                    skip = visitor.set(distance, false);                   

                    ++distance;
                }
                
            } while(!skip);
            
            uint bucketsize = 0;
            if(!skip)
            {
                if(node->_highcount > 0) bucketsize += node->_highcount;
                if(node->_lowcount > 0) bucketsize += node->_lowcount;
                
                for(uint i = 0; i < bucketsize && !stop; ++i)
                {
                    stop = visitor.set_remainder(distance,
                                    ptriepointer_t<T>(this, node->_entries[i]));
                }
            }            
        } while(!waiting.empty() && !stop);
    }
    
    template<typename T>
    bool ptrie_t<T>::best_match(const encoding_t& encoding, uint& tree_pos, 
                uint& e_index, uint& enc_pos, uint& b_index, uint& bucketsize)
    {
        assert(consistent());
        uint t_pos = 0;
        tree_pos = 0;
        enc_pos = 0;
        bucketsize = 0;

        size_t encsize = encoding.size() * 8;
        
        // run through tree as long as there are branches covering some of 
        // the encoding
        do {
            tree_pos = t_pos;
            if (encoding.at(enc_pos)) {
                t_pos = get_node(t_pos)->_highpos;
            } else {
                t_pos = get_node(t_pos)->_lowpos;
            }

            assert(t_pos == 0 || get_node(t_pos)->_parent == tree_pos);
            
            enc_pos++;

        } while (t_pos != 0);
        
        enc_pos--;  // previous always overshoots by 1
        
        assert(tree_pos != 0 || enc_pos == 0);
        
        node_t* node = get_node(tree_pos);
        
        // find out the size of the bucket
        if (node->_highcount > 0) {
            bucketsize += node->_highcount;
            
        }
        
        if (node->_lowcount > 0) {
            bucketsize += node->_lowcount;
        }
        
        // run through the stored data in the bucket, looking for matches
        // start by creating an encoding that "points" to the "unmatched"
        // part of the encoding. Notice, this is a shallow copy, no actual
        // heap-allocation happens!
        encoding_t s_enc = encoding_t(  encoding.raw(), 
                                        (encsize - enc_pos), 
                                        enc_pos, 
                                        encsize);
        
        bool found = false;

        e_index = std::numeric_limits<uint>::max();
        if(bucketsize > 0)
        {
            int low = 0;
            int high = bucketsize - 1;
            do
            {
                b_index = (high + low) / 2 ;
                entry_t* ent = get_entry(node->_entries[b_index]);
                
                int cmp = s_enc.cmp(ent->_data);
                
                if(cmp == 0)
                {
                    found = true;
                    e_index = node->_entries[b_index];
                    break;
                }
                else if(cmp == 1)
                {
                    low = b_index + 1;
                }
                else //if cmp == -1
                {
                    high = b_index - 1;
                }
                
                if(low > high)
                {
                    if(cmp == 1)
                        b_index += 1;
                    break;
                }
                assert(b_index < bucketsize);
            } while(true);
        }
        else
        {
            b_index = 0;
        }
         
        /*int tmp;// refference debug code!
        for(tmp = 0; tmp < bucketsize; ++tmp)
        {
            entry_t* ent = getEntry(node->entries[tmp]);
            if(ent->data < s_enc)
            {
                continue;
            }
            else
            if(ent->data == s_enc)
            {
                assert(found);
                break;
            }
            else
            {
                break;
            }
        }
        assert(tmp == b_index);*/
        
        assert(consistent());        
        return found;
    }
    
    template<typename T>
    uint ptrie_t<T>::split_node(node_t* node, uint tree_pos, uint enc_pos, 
                                                uint bucketsize, bool branch)
    {
        assert(consistent());
        uint n_node_index = new_node();
        node_t* n_node = get_node(n_node_index);
        uint* o_entries = NULL;

        // if we are overflowing in, split bucket
        uint n_node_count = 0;
        uint node_count = 0;

        if (branch) {
            n_node->_entries = new uint[n_node_count = node->_highcount];

            node->_highpos = n_node_index;
            node->_highcount = -1;

            if (node->_lowcount > 0)
            {
                o_entries = new uint[node_count = node->_lowcount];
            }

        } else {
            n_node->_entries = new uint[n_node_count = node->_lowcount];
            
            node->_lowpos = n_node_index;
            node->_lowcount = -1;

            if (node->_highcount > 0)
            {
                o_entries = new uint[node_count = node->_highcount];
            }
        }

        assert(o_entries != NULL || node_count == 0);
        assert(n_node_count > 0);
        assert(tree_pos != n_node_index);
        n_node->_parent = tree_pos;

        n_node->_lowcount = n_node->_highcount = 0;
        n_node->_lowpos = n_node->_highpos = 0;

        // because we are only really shifting around bits when enc_pos % 8 = 0
        // then we need to find out which bit of the first 8 we are
        // splitting on in the "remainder"
        uint r_pos = enc_pos % 8;


        size_t clistcount = 0;
        size_t nlistcount = 0;

        // Copy over the data to the new buckets
        for (size_t i = 0; i < bucketsize; i++) {
            entry_t* entry = get_entry(node->_entries[i]);
            if (entry->_data.at(r_pos) == branch) {
                // Adjust counters
                if (entry->_data.at(r_pos + 1)) {
                    n_node->_highcount++;
                } else {
                    n_node->_lowcount++;
                }

                // This goes to the new bucket, we can maybe remove a byte
                if ((r_pos + 1) == 8) { 
                    // Tree tree is representing the first byte, remove it 
                    entry->_data.pop_front(1);
                }

                assert(clistcount < n_node_count);
                n_node->_entries[clistcount] = node->_entries[i];
                entry->_nodeindex = n_node_index;
                clistcount++;
            } else {
                assert(nlistcount < node_count);
                o_entries[nlistcount] = node->_entries[i];
                nlistcount++;
            }
        }
        
        assert(clistcount == n_node_count);
        assert(nlistcount == node_count);

        delete[] node->_entries;
        node->_entries = o_entries; 

        if (node->_highcount == -1 && node->_lowcount == -1) {
            assert(node->_entries == NULL);
        }

        assert(consistent());
        return n_node_index;
    }
    
    template<typename T>
    std::pair<bool, ptriepointer_t<T> > ptrie_t<T>::find(const encoding_t& encoding)
    {
        uint tree_pos;
        uint enc_pos;
        uint bucketsize;
        uint e_index;
        uint b_index;
        if(best_match(encoding, tree_pos, e_index, enc_pos, b_index,  bucketsize))
        {
            return std::pair<bool, ptriepointer_t<T> >(true, 
                                                ptriepointer_t<T>(this, e_index));
        }
        else
        {
            return std::pair<bool, ptriepointer_t<T> >(false, 
                                                ptriepointer_t<T>(this, e_index));
        }
    }
    
    template<typename T>
    std::pair<bool, ptriepointer_t<T> > ptrie_t<T>::insert(const encoding_t& encoding)
    {
        assert(consistent());
        size_t encsize = encoding.size() * 8;
        uint tree_pos;
        uint enc_pos;
        uint bucketsize;
        uint e_index;
        uint b_index;
                
        if(best_match(encoding, tree_pos, e_index, enc_pos, b_index, bucketsize))
        {   // We are not inserting duplicates, semantics of PTrie is a set.
            assert(consistent());
            return std::pair<bool, ptriepointer_t<T> >(false, 
                                                ptriepointer_t<T>(this, e_index));
        }

        // closest matched node
        node_t* node = get_node(tree_pos);
        
        // Else we need to insert it, start by doing a deep-copy, and putting
        // it into a new entry
        uint ne_index = new_entry();
        entry_t* n_entry = get_entry(ne_index);
        uint remainder_size = (encsize - enc_pos);
        n_entry->_data = encoding_t(  encoding, 
                                    remainder_size, 
                                    enc_pos, 
                                    encsize);
        assert(n_entry->_data.raw() != encoding.raw());
                
        n_entry->_nodeindex = tree_pos;
        
        // make a new bucket, add new entry to end, copy over old data
        bucketsize += 1;
        uint* nbucket = new uint[bucketsize];
        for(size_t i = 0; i < bucketsize; ++i)
        {
            if(i < b_index)
            {
                nbucket[i] = node->_entries[i];
            } 
            else if (i == b_index)
            {
                nbucket[i] = ne_index;
            }
            else
            {
                nbucket[i] = node->_entries[i - 1];
            }
        }
        // We added one to the bucket, now it is larger

        
        // remove old bucket and replace
        delete[] node->_entries;
        node->_entries = nbucket;
        
        // increment correct counter
        short int count;
        bool branch = encoding.at(enc_pos);
        if (branch) {
            count = (++node->_highcount);
            assert(node->_highpos == 0);
        } else {
            count = (++node->_lowcount);
            assert(node->_lowpos == 0);
        }
        
        // if needed, split the node
        if(count > _threshold)
        {
            split_node(node, tree_pos, enc_pos, bucketsize, branch);
        }
        assert(consistent());
        return std::pair<bool, ptriepointer_t<T> >(true, 
                                            ptriepointer_t<T>(this, ne_index));
    }
}



#endif	/* PTRIE_H */

