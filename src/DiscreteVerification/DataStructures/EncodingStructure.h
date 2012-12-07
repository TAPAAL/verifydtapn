/* 
 * File:   Encoding.h
 * Author: Peter Gjøl Jense
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
            virtual ~EncodingStructure();

            EncodingStructure Clone() {
                EncodingStructure s;
                s.rsize = rsize;
                s.shadow = new char[rsize + sizeof (T)];
                memcpy(s.shadow, shadow, rsize + sizeof (T));
                return s;
            }

            void Copy(const EncodingStructure &other, unsigned int offset) {
                memcpy(&(shadow[offset / 8]), other.shadow, other.rsize);
            }

            bool At(const uint place) const;
            void Set(const uint place, const bool value) const;

            void Zero() const {
                memset(shadow, 0x0, rsize);
            }

            unsigned short int Size() const {
                return rsize;
            }

            void Release() const {
                delete[] shadow;
            }

            char* GetRaw() const {
                return shadow;
            }

            void PrintEncoding() const {
                for (unsigned short int i = 0; i < rsize * 8; i++)
                    cout << this->At(i);
                cout << endl;
            }

            inline static uint Overhead(uint size) {
                size = size % 8;
                if (size == 0)
                    return 0;
                else
                    return 8 - size;
            }

            inline void SetMetaData(T data) const {
                memcpy(&(shadow[rsize]), &data, sizeof (T));
            }

            inline T GetMetaData() const {
                T res;
                memcpy(&res, &(shadow[rsize]), sizeof (T));
                return res;
            }

            const char operator[](int i) {

                if (i >= rsize) {
                    return 0x0;
                }
                return shadow[i];
            }
            
            friend bool operator==(const EncodingStructure &enc1, const EncodingStructure &enc2) {
                if(enc1.rsize != enc2.rsize)
                    return false;
                for(int i = 0; i < enc1.rsize; i++)
                    if(enc1.shadow[i] != enc2.shadow[i])
                        return false;
                return true;
            }
            
            
            friend bool operator<(const EncodingStructure &enc1, const EncodingStructure &enc2) {
                int count = enc1.rsize > enc2.rsize ? enc1.rsize : enc2.rsize;

                for (int i = 0; i < count; i++) {
                    if (enc1.rsize > i && enc2.rsize > i && enc1.shadow[i] != enc2.shadow[i]) {
                        return ((unsigned short int) enc1.shadow[i]) < ((unsigned short int) enc2.shadow[i]);
                    }

                }
                if (enc1.rsize > enc2.rsize) {
                    return false;

                } else if (enc1.rsize < enc2.rsize) {
                    return true;
                }

                return false;
            }

        private:
            char* shadow;
            unsigned short rsize;
            const static char masks[8];
        };

        template<class T>
        const char EncodingStructure<T>::masks[8] = {
            0x01,
            0x02,
            0x04,
            0x08,
            0x10,
            0x20,
            0x40,
            0x80
        };

        template<class T>
        EncodingStructure<T>::EncodingStructure(uint size) {
            rsize = (size + Overhead(size)) / 8;
            shadow = new char[rsize + sizeof (T)];
            memset(shadow, 0x0, rsize + sizeof (T));
        }

        template<class T>
        EncodingStructure<T>::EncodingStructure(const EncodingStructure &other, uint offset) {
            offset = offset / 8;

            rsize = other.rsize;
            if (rsize > offset)
                rsize -= offset;
            else {
                rsize = 0;
            }

            shadow = new char[rsize + sizeof (T)];
            memcpy(shadow, &(other.shadow[offset]), rsize);
            SetMetaData(other.GetMetaData());
        }

        template<class T>
        EncodingStructure<T>::EncodingStructure(const EncodingStructure &other, uint size, uint offset, uint encsize) {

            uint so = size + offset;
            offset = ((so - 1) / 8) - ((size - 1) / 8);

            rsize = ((encsize + this->Overhead(encsize)) / 8);
            if (rsize > offset)
                rsize -= offset;
            else {
                rsize = 0;
            }

            shadow = new char[rsize + sizeof (T)];
            memcpy(shadow, &(other.shadow[offset]), rsize);
            SetMetaData(other.GetMetaData());
        }

        template<class T>
        EncodingStructure<T>::EncodingStructure(char* raw, uint size, uint offset, uint encsize) {

            uint so = size + offset;
            offset = ((so - 1) / 8) - ((size - 1) / 8);

            rsize = ((encsize + this->Overhead(encsize)) / 8);
            if (rsize > offset)
                rsize -= offset;
            else {
                rsize = 0;
            }

            shadow = &(raw[offset]);
        }

        template<class T>
        EncodingStructure<T>::~EncodingStructure() {

        }

        template<class T>
        bool EncodingStructure<T>::At(const uint place) const {
            //    return data[place];
            uint offset = place % 8;
            bool res2;
            if (place / 8 < rsize)
                res2 = (shadow[place / 8] & masks[offset]) != 0;
            else
                res2 = false;

            return res2;
        }

        template<class T>
        void EncodingStructure<T>::Set(const uint place, const bool value) const {
            uint offset = place % 8;
            uint theplace = place / 8;
            if (value) {
                shadow[theplace] |= masks[offset];
            } else {
                shadow[theplace] &= ~masks[offset];
            }

        }

    }
}
#endif	/* ENCODING_H */

