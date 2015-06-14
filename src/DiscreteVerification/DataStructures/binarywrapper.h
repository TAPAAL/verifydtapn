/* 
 * File:   binarywrapper.h
 * Author: Peter G. Jensen
 *
 * Created on 10 June 2015, 19:20
 */

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

#ifndef BINARYWRAPPER_H
#define	BINARYWRAPPER_H
namespace pgj
{
    typedef unsigned int uint;
    typedef unsigned char uchar;
    
    template<class T>
    class binarywrapper
    {
    public:
        // Constructors
        binarywrapper();        
        binarywrapper(uint size);
        binarywrapper(const binarywrapper& other, uint offset);
        binarywrapper(const binarywrapper& other, uint size, uint offset, 
                                                            uint encodingsize);
        binarywrapper(uchar* raw, uint size, uint offset, uint encsize);
        binarywrapper(uchar* raw, uint size);
        
        // Destructor
        ~binarywrapper();
        
        // Copy and clones
        binarywrapper clone() const;
        void copy(const binarywrapper& other, uint offset);
        void copy(const uchar* raw, uint size);
        
        // accessors
        bool at(const uint place) const;
        uint size() const;
        uchar* raw() const;
        void print() const;
        static size_t overhead(uint size);
        
        // modifiers
        void set(const uint place, const bool value) const;
        void zero() const;
        void release() const;
        void setMeta(T data);
        T getMeta() const;
        uchar operator[](int i);
        void pop_front(unsigned short);
        
        // Operators
        inline friend bool operator==(  const binarywrapper &enc1, 
                                        const binarywrapper &enc2) {
            if(enc1.numberOfBytes != enc2.numberOfBytes)
                return false;
            
            for(size_t i = 0; i < enc1.numberOfBytes; i++)
                if(enc1.blob[i] != enc2.blob[i])
                    return false;
            
            std::cout << "match : " << std::endl;
            enc1.print(); 
            enc2.print();
            std::cout << "done" << std::endl;
            return true;
        }
        
        inline friend bool operator<(   const binarywrapper &enc1, 
                                        const binarywrapper &enc2) {
            int count = enc1.numberOfBytes > enc2.numberOfBytes ?
                                        enc1.numberOfBytes : enc2.numberOfBytes;

            for (size_t i = 0; i < count; i++) {
                if (enc1.numberOfBytes > i && enc2.numberOfBytes > i &&
                        enc1.blob[i] != enc2.blob[i]) {
                    return 
                            ((unsigned short int) enc1.blob[i]) < 
                            ((unsigned short int) enc2.blob[i]);
                }
            }
            
            if (enc1.numberOfBytes > enc2.numberOfBytes) {
                return false;
            } else if (enc1.numberOfBytes < enc2.numberOfBytes) {
                return true;
            }

            return false;
        }
        
    private:
        uchar* blob;
        unsigned short numberOfBytes;
        T meta;
        const static uchar masks[8];
    };
    
    template<class T>
    const uchar binarywrapper<T>::masks[8] = {
        static_cast <uchar>(0x01),
        static_cast <uchar>(0x02),
        static_cast <uchar>(0x04),
        static_cast <uchar>(0x08),
        static_cast <uchar>(0x10),
        static_cast <uchar>(0x20),
        static_cast <uchar>(0x40),
        static_cast <uchar>(0x80)
    };
            
    template<class T>
    size_t binarywrapper<T>::overhead(uint size)
    {
        size = size % 8;
        if (size == 0)
            return 0;
        else
            return 8 - size; 
    }
    
    template<class T>
    binarywrapper<T>::~binarywrapper()
    {
        
    }
    
    template<class T>
    binarywrapper<T>::binarywrapper()
    {}
    
    template<class T>
    binarywrapper<T>::binarywrapper(uint size)
    {
        numberOfBytes = (size + overhead(size)) / 8;
        blob = new uchar[numberOfBytes];
        memset(blob, 0x0, numberOfBytes);
    }
    
    template<class T>
    binarywrapper<T>::binarywrapper(const binarywrapper& other, uint offset)
    {
         offset = offset / 8;

        numberOfBytes = other.numberOfBytes;
        if (numberOfBytes > offset)
            numberOfBytes -= offset;
        else {
            numberOfBytes = 0;
        }

        blob = new uchar[numberOfBytes];
        memcpy(blob, &(other.blob[offset]), numberOfBytes);
        setMeta(other.getMeta());
    }
    
    template<class T>
    binarywrapper<T>::binarywrapper(
        const binarywrapper& other, uint size, uint offset, uint encodingsize)
    {
        uint so = size + offset;
        offset = ((so - 1) / 8) - ((size - 1) / 8);

        numberOfBytes = ((encodingsize + this->overhead(encodingsize)) / 8);
        if (numberOfBytes > offset)
            numberOfBytes -= offset;
        else {
            numberOfBytes = 0;
        }

        blob = new uchar[numberOfBytes];
        memcpy(blob, &(other.blob[offset]), numberOfBytes);
        setMeta(other.getMeta());
    }
    
    template<class T>
    binarywrapper<T>::binarywrapper
        (uchar* raw, uint size, uint offset, uint encodingsize)
    {
        
        uint so = size + offset;
        offset = ((so - 1) / 8) - ((size - 1) / 8);

        numberOfBytes = ((encodingsize + this->overhead(encodingsize)) / 8);
        if (numberOfBytes > offset)
            numberOfBytes -= offset;
        else {
            numberOfBytes = 0;
        }

        blob = &(raw[offset]);
    }
    
    template<class T>
    binarywrapper<T>::binarywrapper(uchar* raw, uint size)
    {
        blob = raw;
        numberOfBytes = size / 8 + (size % 8 ? 1 : 0);     
    }
    
    // Copy and clones
    template<class T>
    binarywrapper<T> binarywrapper<T>::clone() const
    {
        binarywrapper<T> s;
        s.numberOfBytes = numberOfBytes;
        s.blob = new uchar[numberOfBytes];
        memcpy(s.blob, blob, numberOfBytes);
        s.meta = meta;
        return s; 
    }
    
    template<class T>
    void binarywrapper<T>::copy(const binarywrapper& other, uint offset)
    {
        memcpy(&(blob[offset / 8]), other.blob, other.numberOfBytes);
    }
    
    template<class T>
    void binarywrapper<T>::copy(const uchar* raw, uint size)
    {
        if(size > 0)
        {
            blob = new char[size];
            memcpy(blob, raw, size);
        }
    }
        
    // accessors
    template<class T>
    bool binarywrapper<T>::at(const uint place) const
    {
        uint offset = place % 8;
        bool res2;
        if (place / 8 < numberOfBytes)
            res2 = (blob[place / 8] & masks[offset]) != 0;
        else
            res2 = false;

        return res2;  
    }
    
    template<class T>
    uint binarywrapper<T>::size() const
    {
        return numberOfBytes;
    }
    
    template<class T>
    uchar* binarywrapper<T>::raw() const
    {
        return blob; 
    }
    
    template<class T>
    void binarywrapper<T>::print() const
    {
        for (size_t i = 0; i < numberOfBytes * 8; i++)
                std::cout << this->at(i);
            std::cout << std::endl;
    }
    
    
    template<class T>
    void binarywrapper<T>::pop_front(unsigned short int topop)
    {
        if(numberOfBytes == 0) return;  // Special case, nothing to do!
        unsigned short int nbytes;
        if(topop >= numberOfBytes)
        {
            topop = numberOfBytes;
            nbytes = 0;
        }
        else
        {
            nbytes = numberOfBytes - topop;            
        }
        
        if(nbytes > 0)
        {
            uchar* tmpblob = new uchar[nbytes];
            memcpy(tmpblob, &(blob[topop]), (nbytes));
            delete[] blob;
            blob = tmpblob;
        }
        else
        {
            delete[] blob;
            blob = NULL;
        }
        numberOfBytes = nbytes;
    }
    
    template<class T>
    void binarywrapper<T>::set(const uint place, const bool value) const
    {
        assert(place < numberOfBytes*8);
        uint offset = place % 8;
        uint theplace = place / 8;
        if (value) {
            blob[theplace] |= masks[offset];
        } else {
            blob[theplace] &= ~masks[offset];
        }    
    }
    
    template<class T>
    void binarywrapper<T>::zero() const
    {
        if(numberOfBytes > 0 && blob != NULL)
        {
            memset(blob, 0x0, numberOfBytes); 
        }
    }
    
    template<class T>
    void binarywrapper<T>::release() const
    {
        delete[] blob;
    }
    
    template<class T>
    void binarywrapper<T>::setMeta(T data)
    {
        meta = data;
    }
    
    template<class T>
    T binarywrapper<T>::getMeta() const
    {
        return meta;
    }
    
    template<class T>
    uchar binarywrapper<T>::operator[](int i)
    {
       if (i >= numberOfBytes) {
            return 0x0;
        }
        return blob[i]; 
    }
}


#endif	/* BINARYWRAPPER_H */

