/* 
 * File:   PTrie.h
 * Author: Peter G. Jensen
 *
 * Created on 10 June 2015, 18:44
 */

#ifndef PTRIE_H
#define	PTRIE_H

#include "binarywrapper.h"
#include <assert.h>
#include <limits>
#include <vector>
namespace pgj
{
    typedef unsigned int uint;
    typedef unsigned char uchar;
    
    template<typename T>
    class ptrie;
    
    template<typename T>
    class ptriepointer
    {
        typedef binarywrapper<T> encoding_t;
        public:
            ptrie<T>* container;
            uint index;
        public:
            ptriepointer(ptrie<T>* container, uint i); 
            T getMeta() const;
            void setMeta(T);
            uint revsereWritePartialEncoding(encoding_t&) const;
            encoding_t& remainder() const;
            ptriepointer() : container(NULL), index(0) {};
    };
    
    template<typename T>
    ptriepointer<T>::ptriepointer(ptrie<T>* container, uint i) : 
    container(container), index(i)
    {        
    }
    
    template<typename T>
    T ptriepointer<T>::getMeta() const
    {
        return container->getEntry(index)->data.getMeta();
    }
    
    template<typename T>
    void ptriepointer<T>::setMeta(T val)
    {
        container->getEntry(index)->data.setMeta(val);
    }
    
    template<typename T>
    uint ptriepointer<T>::revsereWritePartialEncoding(encoding_t& encoding) const
    {
        return container->writePathToRoot(index, encoding);
    }
    
    template<typename T>
    binarywrapper<T>& ptriepointer<T>::remainder() const
    {
        return container->getEntry(index)->data;
    }
    
    template<typename T>
    class ptrie {
        typedef binarywrapper<T> encoding_t;  
        friend class ptriepointer<T>;
        private:
            
            // nodes in the tree
            struct node_t
            {
                uint highpos;           // branches
                uint lowpos;
                short int highCount;    // bucket-counts
                short int lowCount;
                uint parent;            // for back-traversal
                uint* entries;          // back-pointers to data-array up to date
            };
            
            struct entry_t
            {
                encoding_t data;       // remainder-data, not in path of tree 
                uint nodeindex;        // index of node
            };
            
            const uint blockSize;   // number to allocate at a time
            uint nextFreeNode;      // location of next vacant node-location
            uint nextFreeEntry;     // location of next vacant entry-node
            
            std::vector<node_t*> nodeVector;    // Vector of arrays of nodes in tree
            std::vector<entry_t*> entryVector;  // Vector of remainders
            
            short int splitThreshold;
            
            node_t* getNode(uint index) const;
            uint newNode();
            
            entry_t* getEntry(uint index) const;
            uint newEntry();
            
            bool bestMatch(const encoding_t& encoding, uint& tree_pos , uint& e_index,
                                            uint& enc_pos, uint& bucketsize);
            
            uint splitNode(node_t* node, uint tree_pos, uint enc_pos, 
                    uint bucketsize, bool branch);
            protected:
                uint writePathToRoot(uint n_index, encoding_t& encoding) const;
            
        public:
            ptrie();
            ~ptrie();

            std::pair<bool, ptriepointer<T> > insert(const encoding_t& encoding);
            std::pair<bool, ptriepointer<T> > find(const encoding_t& encoding);
            bool isConsistent() const;
            uint size() const { return nextFreeEntry; }
    };
    
    template<typename T>
    ptrie<T>::~ptrie() {
        for(node_t* nodes : nodeVector)
        {
            delete[] nodes;
        }
        nodeVector.clear();
        for(entry_t* ents : entryVector)
        {
            for(size_t i = 0; i < blockSize; ++i)
            {
                ents[i].data.release();
            }
            delete[] ents;
        }
        entryVector.clear();
    }
    
    template<typename T>
    ptrie<T>::ptrie() :
            blockSize(1024), 
            nextFreeNode(1), 
            nextFreeEntry(0), 
            nodeVector(), 
            entryVector(), 
            splitThreshold(sizeof(node_t) * 8)
    {
        nodeVector.push_back(new node_t[blockSize]);
        nodeVector[0][0].entries = NULL;
        nodeVector[0][0].highCount = 0;
        nodeVector[0][0].lowCount = 0;
        nodeVector[0][0].highpos = 0;
        nodeVector[0][0].lowpos = 0;
        nodeVector[0][0].parent = 0;
    }
    
    template<typename T>
    typename ptrie<T>::node_t*
    ptrie<T>::getNode(uint index) const
    {
        assert(index < nextFreeNode);
        return &nodeVector[index / blockSize][index % blockSize];
    }
    
    template<typename T>
    uint ptrie<T>::newNode()
    {
        uint next = nextFreeNode;
        if(next % blockSize == 0)
        {
            nodeVector.push_back(new node_t[blockSize]);
        }
        
        ++nextFreeNode;
        return next;
    }
    
    template<typename T>
    typename ptrie<T>::entry_t* 
    ptrie<T>::getEntry(uint index) const
    {
        assert(index < nextFreeEntry);
        return &entryVector[index / blockSize][index % blockSize];
    }
    
    template<typename T>
    uint ptrie<T>::newEntry()
    {
        uint next = nextFreeEntry;
        if(next % blockSize == 0)
        {
            entryVector.push_back(new entry_t[blockSize]);
        }
        
        ++nextFreeEntry;
        return next;
    }
    
    template<typename T>
    bool ptrie<T>::isConsistent() const
    {
        return true;
        for(size_t i = 0; i < nextFreeNode; ++i)
        {
            node_t* node = getNode(i);
            assert(node->highpos < nextFreeNode);
            assert(node->lowpos < nextFreeNode);
            assert(node->parent < nextFreeNode);
            assert(node->parent == 0 && i == 0 || node->parent < i);
            assert((node->highpos > i && node->highCount == -1) 
                    || (node->highpos == 0 && node->highCount >= 0));
            assert((node->lowpos > i && node->lowCount == -1) 
                    || (node->lowpos == 0 && node->lowCount >= 0));
            
            size_t bucket = 0;
            
            if(node->highCount > 0)
                bucket += node->highCount;
            if(node->lowCount > 0)
                bucket += node->lowCount;

            assert(node->entries != NULL || bucket == 0);
            for(size_t e = 0; e < bucket; ++e)
            {
                assert(e < nextFreeEntry);
                assert(getEntry(node->entries[e])->nodeindex == i);
            }
        }
        return true;
    }
    
    template<typename T>
    uint ptrie<T>::writePathToRoot(uint e_index, encoding_t& encoding) const
    {
        entry_t* ent = getEntry(e_index);
        size_t count = 0;
        uint c_index = ent->nodeindex;
        node_t* node = getNode(c_index);
        assert(isConsistent());
        while(c_index != 0)
        {
            uint p_index = node->parent;
            node = getNode(p_index);
            bool branch = c_index == node->highpos;
            encoding.set(count, branch);
            ++count;
            c_index = p_index;
        }
        assert(isConsistent());
        return count;
    }
    
    template<typename T>
    bool ptrie<T>::bestMatch(const encoding_t& encoding, uint& tree_pos, 
                                uint& e_index, uint& enc_pos, uint& bucketsize)
    {
        assert(isConsistent());
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
                t_pos = getNode(t_pos)->highpos;
            } else {
                t_pos = getNode(t_pos)->lowpos;
            }

            assert(t_pos == 0 || getNode(t_pos)->parent == tree_pos);
            
            enc_pos++;

        } while (t_pos != 0);
        
        enc_pos--;  // previous always overshoots by 1
        
        assert(tree_pos != 0 || enc_pos == 0);
        
        node_t* node = getNode(tree_pos);
        
        // find out the size of the bucket
        if (node->highCount > 0) {
            bucketsize += node->highCount;
            
        }
        
        if (node->lowCount > 0) {
            bucketsize += node->lowCount;
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
        
        for(uint i = 0; i < bucketsize; ++i)
        {
            if(getEntry(node->entries[i])->data == s_enc)
            {
                found = true;
                e_index = node->entries[i];
                break;
            }
        }
        
        assert(isConsistent());        
        return found;
    }
    
    template<typename T>
    uint ptrie<T>::splitNode(node_t* node, uint tree_pos, uint enc_pos, 
                                                uint bucketsize, bool branch)
    {
        assert(isConsistent());
        uint n_node_index = newNode();
        node_t* n_node = getNode(n_node_index);
        uint* o_entries = NULL;

        // if we are overflowing in, split bucket
        uint n_node_count = 0;
        uint node_count = 0;

        if (branch) {
            n_node->entries = new uint[n_node_count = node->highCount];

            node->highpos = n_node_index;
            node->highCount = -1;

            if (node->lowCount > 0)
            {
                o_entries = new uint[node_count = node->lowCount];
            }

        } else {
            n_node->entries = new uint[n_node_count = node->lowCount];
            
            node->lowpos = n_node_index;
            node->lowCount = -1;

            if (node->highCount > 0)
            {
                o_entries = new uint[node_count = node->highCount];
            }
        }

        assert(o_entries != NULL || node_count == 0);
        assert(n_node_count > 0);
        assert(tree_pos != n_node_index);
        n_node->parent = tree_pos;

        n_node->lowCount = n_node->highCount = 0;
        n_node->lowpos = n_node->highpos = 0;

        // because we are only really shifting around bits when enc_pos % 8 = 0
        // then we need to find out which bit of the first 8 we are
        // splitting on in the "remainder"
        uint r_pos = enc_pos % 8;


        size_t clistcount = 0;
        size_t nlistcount = 0;

        // Copy over the data to the new buckets
        for (size_t i = 0; i < bucketsize; i++) {
            entry_t* entry = getEntry(node->entries[i]);
            if (entry->data.at(r_pos) == branch) {
                // Adjust counters
                if (entry->data.at(r_pos + 1)) {
                    n_node->highCount++;
                } else {
                    n_node->lowCount++;
                }

                // This goes to the new bucket, we can maybe remove a byte
                if ((r_pos + 1) == 8) { 
                    // Tree tree is representing the first byte, remove it 
                    entry->data.pop_front(1);
                }

                assert(clistcount < n_node_count);
                n_node->entries[clistcount] = node->entries[i];
                entry->nodeindex = n_node_index;
                clistcount++;
            } else {
                assert(nlistcount < node_count);
                o_entries[nlistcount] = node->entries[i];
                nlistcount++;
            }
        }
        
        assert(clistcount == n_node_count);
        assert(nlistcount == node_count);

        delete[] node->entries;
        node->entries = o_entries; 

        if (node->highCount == -1 && node->lowCount == -1) {
            assert(node->entries == NULL);
        }

        assert(isConsistent());
        return n_node_index;
    }
    
    template<typename T>
    std::pair<bool, ptriepointer<T> > ptrie<T>::find(const encoding_t& encoding)
    {
        uint tree_pos;
        uint enc_pos;
        uint bucketsize;
        uint e_index;
        
        if(bestMatch(encoding, tree_pos, e_index, enc_pos, bucketsize))
        {
            return std::pair<bool, ptriepointer<T> >(true, 
                                                ptriepointer<T>(this, e_index));
        }
        else
        {
            return std::pair<bool, ptriepointer<T> >(false, 
                                                ptriepointer<T>(this, e_index));
        }
    }
    
    template<typename T>
    std::pair<bool, ptriepointer<T> > ptrie<T>::insert(const encoding_t& encoding)
    {
        assert(isConsistent());
        size_t encsize = encoding.size() * 8;
        uint tree_pos;
        uint enc_pos;
        uint bucketsize;
        uint e_index;
                
        if(bestMatch(encoding, tree_pos, e_index, enc_pos, bucketsize))
        {   // We are not inserting duplicates, semantics of PTrie is a set.
            assert(isConsistent());
            return std::pair<bool, ptriepointer<T> >(false, 
                                                ptriepointer<T>(this, e_index));
        }

        // closest matched node
        node_t* node = getNode(tree_pos);
        
        // Else we need to insert it, start by doing a deep-copy, and putting
        // it into a new entry
        uint ne_index = newEntry();
        entry_t* n_entry = getEntry(ne_index);
        uint remainder_size = (encsize - enc_pos);
        n_entry->data = encoding_t(  encoding, 
                                    remainder_size, 
                                    enc_pos, 
                                    encsize);
        assert(n_entry->data.raw() != encoding.raw());
                
        n_entry->nodeindex = tree_pos;
        
        // make a new bucket, add new entry to end, copy over old data
        uint* nbucket = new uint[bucketsize + 1];
        nbucket[bucketsize] = ne_index;
        for(size_t i = 0; i < bucketsize; ++i)
        {
            nbucket[i] = node->entries[i];
        }
        // We added one to the bucket, now it is larger
        bucketsize += 1;
        
        // remove old bucket and replace
        delete[] node->entries;
        node->entries = nbucket;
        
        // increment correct counter
        short int count;
        bool branch = encoding.at(enc_pos);
        if (branch) {
            count = (++node->highCount);
            assert(node->highpos == 0);
        } else {
            count = (++node->lowCount);
            assert(node->lowpos == 0);
        }
        
        // if needed, split the node
        if(count > splitThreshold)
        {
            splitNode(node, tree_pos, enc_pos, bucketsize, branch);
        }
        assert(isConsistent());
        return std::pair<bool, ptriepointer<T> >(true, 
                                            ptriepointer<T>(this, ne_index));
    }
}



#endif	/* PTRIE_H */

