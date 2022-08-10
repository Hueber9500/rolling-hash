#pragma once
#include <vector>
#include "BaseRollingHash.h"

class RollingHash : public BaseRollingHash{
    private:
        long long base;
        unsigned char* arr;
        long long power;
        int index;

        RollingHash& operator=(RollingHash&& obj) = delete;
        RollingHash& operator=(const RollingHash& obj) = delete;

    public:
        RollingHash(const RollingHash& obj) : BaseRollingHash(obj.width)
        {
            if( this != &obj )
            {
                delete[] arr;

                arr = new unsigned char[obj.width];

                base = obj.base;
                width = obj.width;
                power = obj.power;
                hash = obj.hash;
                index = obj.index;

                std::copy(obj.arr, obj.arr + width, arr);
            }
        }
    
    public:
        RollingHash(long base, int width) : BaseRollingHash(width), base(base), index(0)
        {
            power = base;
            arr = new unsigned char[width];
            memset(arr, 0, width * sizeof(unsigned char));

            for(int i = 0; i < width; i++) 
            {
                power *= base;
            }
        }

        RollingHash(RollingHash&& obj) : BaseRollingHash(std::forward<RollingHash>(obj))
        {
            base = obj.base;
            power = obj.power;
            hash = obj.hash;
            index = obj.index;

            arr = obj.arr;
            obj.arr = nullptr;
        }

        void AddByte(unsigned char byte)
        {
            long long removed_part = power * arr[index];
            hash -= removed_part;
            hash *= base;
            hash += byte;

            arr[index++] = byte;
            index %= width;
        }

        void Init()
        {
            hash = 0;
            index = 0;
            memset(arr, 0, width * sizeof(unsigned char));
        }

        virtual ~RollingHash()
        {
            delete[] arr;
        }
};