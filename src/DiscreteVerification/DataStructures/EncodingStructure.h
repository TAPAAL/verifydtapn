/* 
 * File:   Encoding.h
 * Author: Peter Gjøl Jensen
 *
 * Created on 27. oktober 2012, 12:36
 */

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

#ifndef ENCODING_H
#define	ENCODING_H
using namespace std;
namespace VerifyTAPN {
    namespace DiscreteVerification {

        template<class T>
        class EncodingStructure {
        public:
            typedef unsigned int uint;

            EncodingStructure() {
            };
            EncodingStructure(uint size);
            EncodingStructure(const EncodingStructure &other, uint offset);
            EncodingStructure(const EncodingStructure &other, uint size, uint offset, uint encsize);
            EncodingStructure(char* raw, uint size, uint offset, uint encsize);
            EncodingStructure(char* raw, uint size){
                binaryBlob = raw;
                numberOfBytes = size;
            };
            virtual ~EncodingStructure();

            EncodingStructure clone() {
                EncodingStructure s;
                s.numberOfBytes = numberOfBytes;
                s.binaryBlob = new char[numberOfBytes + sizeof (T)];
                memcpy(s.binaryBlob, binaryBlob, numberOfBytes + sizeof (T));
                return s;
            }

            inline void copy(const EncodingStructure &other, unsigned int offset) {
                memcpy(&(binaryBlob[offset / 8]), other.binaryBlob, other.numberOfBytes);
            }
            
            inline void copy(const char* raw, unsigned int size){
                binaryBlob = new char[size + sizeof(T)];
                memcpy(binaryBlob, raw, size);
            }

            inline bool at(const uint place) const {
                uint offset = place % 8;
                bool res2;
                if (place / 8 < numberOfBytes)
                    res2 = (binaryBlob[place / 8] & masks[offset]) != 0;
                else
                    res2 = false;

                return res2;
            }

            inline void set(const uint place, const bool value) const {
                uint offset = place % 8;
                uint theplace = place / 8;
                if (value) {
                    binaryBlob[theplace] |= masks[offset];
                } else {
                    binaryBlob[theplace] &= ~masks[offset];
                }

            }

            inline void zero() const {
                memset(binaryBlob, 0x0, numberOfBytes);
            }

            inline unsigned short int Size() const {
                return numberOfBytes;
            }

            inline void release() const {
                delete[] binaryBlob;
            }

            inline char* getRaw() const {
                return binaryBlob;
            }

            void printEncoding() const {
                for (unsigned short int i = 0; i < numberOfBytes * 8; i++)
                    cout << this->at(i);
                cout << endl;
            }

            inline static uint overhead(uint size) {
                size = size % 8;
                if (size == 0)
                    return 0;
                else
                    return 8 - size;
            }

            inline void setMetaData(T data) const {
                memcpy(&(binaryBlob[numberOfBytes]), &data, sizeof (T));
            }

            inline T getMetaData() const {
                T res;
                memcpy(&res, &(binaryBlob[numberOfBytes]), sizeof (T));
                return res;
            }

            inline const char operator[](int i) {

                if (i >= numberOfBytes) {
                    return 0x0;
                }
                return binaryBlob[i];
            }
            
            inline friend bool operator==(const EncodingStructure &enc1, const EncodingStructure &enc2) {
                if(enc1.numberOfBytes != enc2.numberOfBytes)
                    return false;
                for(int i = 0; i < enc1.numberOfBytes; i++)
                    if(enc1.binaryBlob[i] != enc2.binaryBlob[i])
                        return false;
                return true;
            }
            
            
            inline friend bool operator<(const EncodingStructure &enc1, const EncodingStructure &enc2) {
                int count = enc1.numberOfBytes > enc2.numberOfBytes ? enc1.numberOfBytes : enc2.numberOfBytes;

                for (int i = 0; i < count; i++) {
                    if (enc1.numberOfBytes > i && enc2.numberOfBytes > i && enc1.binaryBlob[i] != enc2.binaryBlob[i]) {
                        return ((unsigned short int) enc1.binaryBlob[i]) < ((unsigned short int) enc2.binaryBlob[i]);
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
            char* binaryBlob;
            unsigned short numberOfBytes;
            const static char masks[8];
        };

        template<class T>
        const char EncodingStructure<T>::masks[8] = {
            static_cast <char>(0x01),
            static_cast <char>(0x02),
            static_cast <char>(0x04),
            static_cast <char>(0x08),
            static_cast <char>(0x10),
            static_cast <char>(0x20),
            static_cast <char>(0x40),
            static_cast <char>(0x80)
        };

        template<class T>
        EncodingStructure<T>::EncodingStructure(uint size) {
            numberOfBytes = (size + overhead(size)) / 8;
            binaryBlob = new char[numberOfBytes + sizeof (T)];
            memset(binaryBlob, 0x0, numberOfBytes + sizeof (T));
        }

        template<class T>
        EncodingStructure<T>::EncodingStructure(const EncodingStructure &other, uint offset) {
            offset = offset / 8;

            numberOfBytes = other.numberOfBytes;
            if (numberOfBytes > offset)
                numberOfBytes -= offset;
            else {
                numberOfBytes = 0;
            }

            binaryBlob = new char[numberOfBytes + sizeof (T)];
            memcpy(binaryBlob, &(other.binaryBlob[offset]), numberOfBytes);
            setMetaData(other.getMetaData());
        }

        template<class T>
        EncodingStructure<T>::EncodingStructure(const EncodingStructure &other, uint size, uint offset, uint encsize) {

            uint so = size + offset;
            offset = ((so - 1) / 8) - ((size - 1) / 8);

            numberOfBytes = ((encsize + this->overhead(encsize)) / 8);
            if (numberOfBytes > offset)
                numberOfBytes -= offset;
            else {
                numberOfBytes = 0;
            }

            binaryBlob = new char[numberOfBytes + sizeof (T)];
            memcpy(binaryBlob, &(other.binaryBlob[offset]), numberOfBytes);
            setMetaData(other.getMetaData());
        }

        template<class T>
        EncodingStructure<T>::EncodingStructure(char* raw, uint size, uint offset, uint encsize) {

            uint so = size + offset;
            offset = ((so - 1) / 8) - ((size - 1) / 8);

            numberOfBytes = ((encsize + this->overhead(encsize)) / 8);
            if (numberOfBytes > offset)
                numberOfBytes -= offset;
            else {
                numberOfBytes = 0;
            }

            binaryBlob = &(raw[offset]);
        }

        template<class T>
        EncodingStructure<T>::~EncodingStructure() {

        }



    }
}
#endif	/* ENCODING_H */

