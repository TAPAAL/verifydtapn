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
        void set_meta(T data);
        T get_meta() const;
        uchar operator[](int i);
        void pop_front(unsigned short);
        
        inline int cmp(const binarywrapper &other)
        {
            if(_nbytes != other._nbytes)
            {
                if(_nbytes < other._nbytes) return -1;
                else return 1;
            }
                
            for(int i = _nbytes - 1; i >= 0; i--)
            {
                if(_blob[i] < other._blob[i])
                    return -1;
                else if (_blob[i] > other._blob[i])
                    return 1;
            }
            
            return 0;
        }
        
        // Operators
        inline friend bool operator==(  const binarywrapper &enc1, 
                                        const binarywrapper &enc2) {
            if(enc1._nbytes != enc2._nbytes)
                return false;
            
            for(size_t i = 0; i < enc1._nbytes; i++)
                if(enc1._blob[i] != enc2._blob[i])
                    return false;
            
            return true;
        }
        
       /* inline friend bool operator <=( const binarywrapper &enc1,
                                        const binarywrapper &enc2)
        {
            if(enc1.numberOfBytes != enc2.numberOfBytes)
                return enc1.numberOfBytes < enc2.numberOfBytes;
            
            for(size_t i = 0; i < enc1.numberOfBytes; i++)
                if(enc1.blob[i] < enc2.blob[i])
                    return true;
                else if (enc1.blob[i] > enc2.blob[i])
                    return false;
            
            return true;
        }*/
        
        inline friend bool operator<(   const binarywrapper &enc1, 
                                        const binarywrapper &enc2) {
            if(enc1._nbytes != enc2._nbytes)
                return enc1._nbytes < enc2._nbytes;
            
            for(size_t i = 0; i < enc1._nbytes; i++)
                if(enc1._blob[i] < enc2._blob[i])
                    return true;
                else if (enc1._blob[i] > enc2._blob[i])
                    return false;
            
            return false;
        }
        
    private:
        uchar* _blob;
        unsigned short _nbytes;
        T _meta;
        const static uchar _masks[8];
    };
    
    template<class T>
    const uchar binarywrapper<T>::_masks[8] = {
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
        _nbytes = (size + overhead(size)) / 8;
        _blob = new uchar[_nbytes];
        memset(_blob, 0x0, _nbytes);
    }
    
    template<class T>
    binarywrapper<T>::binarywrapper(const binarywrapper& other, uint offset)
    {
         offset = offset / 8;

        _nbytes = other._nbytes;
        if (_nbytes > offset)
            _nbytes -= offset;
        else {
            _nbytes = 0;
        }

        _blob = new uchar[_nbytes];
        memcpy(_blob, &(other._blob[offset]), _nbytes);
        set_meta(other.get_meta());
    }
    
    template<class T>
    binarywrapper<T>::binarywrapper(
        const binarywrapper& other, uint size, uint offset, uint encodingsize)
    {
        uint so = size + offset;
        offset = ((so - 1) / 8) - ((size - 1) / 8);

        _nbytes = ((encodingsize + this->overhead(encodingsize)) / 8);
        if (_nbytes > offset)
            _nbytes -= offset;
        else {
            _nbytes = 0;
        }

        _blob = new uchar[_nbytes];
        memcpy(_blob, &(other._blob[offset]), _nbytes);
        set_meta(other.get_meta());
    }
    
    template<class T>
    binarywrapper<T>::binarywrapper
        (uchar* raw, uint size, uint offset, uint encodingsize)
    {
        
        uint so = size + offset;
        offset = ((so - 1) / 8) - ((size - 1) / 8);

        _nbytes = ((encodingsize + this->overhead(encodingsize)) / 8);
        if (_nbytes > offset)
            _nbytes -= offset;
        else {
            _nbytes = 0;
        }

        _blob = &(raw[offset]);
    }
    
    template<class T>
    binarywrapper<T>::binarywrapper(uchar* raw, uint size)
    {
        _blob = raw;
        _nbytes = size / 8 + (size % 8 ? 1 : 0);     
    }
    
    // Copy and clones
    template<class T>
    binarywrapper<T> binarywrapper<T>::clone() const
    {
        binarywrapper<T> s;
        s._nbytes = _nbytes;
        s._blob = new uchar[_nbytes];
        memcpy(s._blob, _blob, _nbytes);
        s._meta = _meta;
        return s; 
    }
    
    template<class T>
    void binarywrapper<T>::copy(const binarywrapper& other, uint offset)
    {
        memcpy(&(_blob[offset / 8]), other._blob, other._nbytes);
        _meta = other._meta;
    }
    
    template<class T>
    void binarywrapper<T>::copy(const uchar* raw, uint size)
    {
        if(size > 0)
        {
            _blob = new char[size];
            memcpy(_blob, raw, size);
        }
    }
        
    // accessors
    template<class T>
    bool binarywrapper<T>::at(const uint place) const
    {
        uint offset = place % 8;
        bool res2;
        if (place / 8 < _nbytes)
            res2 = (_blob[place / 8] & _masks[offset]) != 0;
        else
            res2 = false;

        return res2;  
    }
    
    template<class T>
    uint binarywrapper<T>::size() const
    {
        return _nbytes;
    }
    
    template<class T>
    uchar* binarywrapper<T>::raw() const
    {
        return _blob; 
    }
    
    template<class T>
    void binarywrapper<T>::print() const
    {
        for (size_t i = 0; i < _nbytes * 8; i++)
                std::cout << this->at(i);
            std::cout << std::endl;
    }
    
    
    template<class T>
    void binarywrapper<T>::pop_front(unsigned short int topop)
    {
        if(_nbytes == 0) return;  // Special case, nothing to do!
        unsigned short int nbytes;
        if(topop >= _nbytes)
        {
            topop = _nbytes;
            nbytes = 0;
        }
        else
        {
            nbytes = _nbytes - topop;            
        }
        
        if(nbytes > 0)
        {
            uchar* tmpblob = new uchar[nbytes];
            memcpy(tmpblob, &(_blob[topop]), (nbytes));
            delete[] _blob;
            _blob = tmpblob;
        }
        else
        {
            delete[] _blob;
            _blob = NULL;
        }
        _nbytes = nbytes;
    }
    
    template<class T>
    void binarywrapper<T>::set(const uint place, const bool value) const
    {
        assert(place < _nbytes*8);
        uint offset = place % 8;
        uint theplace = place / 8;
        if (value) {
            _blob[theplace] |= _masks[offset];
        } else {
            _blob[theplace] &= ~_masks[offset];
        }    
    }
    
    template<class T>
    void binarywrapper<T>::zero() const
    {
        if(_nbytes > 0 && _blob != NULL)
        {
            memset(_blob, 0x0, _nbytes); 
        }
    }
    
    template<class T>
    void binarywrapper<T>::release() const
    {
        delete[] _blob;
    }
    
    template<class T>
    void binarywrapper<T>::set_meta(T data)
    {
        _meta = data;
    }
    
    template<class T>
    T binarywrapper<T>::get_meta() const
    {
        return _meta;
    }
    
    template<class T>
    uchar binarywrapper<T>::operator[](int i)
    {
       if (i >= _nbytes) {
            return 0x0;
        }
        return _blob[i]; 
    }
}


#endif	/* BINARYWRAPPER_H */

