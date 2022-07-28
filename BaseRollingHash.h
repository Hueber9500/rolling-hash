#pragma once

class BaseRollingHash
{
    protected:
        int width;
        long long hash;

    public:
        BaseRollingHash(int window): width(window), hash(0)
        {
            
        }

        BaseRollingHash(BaseRollingHash&& obj)
        {
            width = obj.width;
        }

    public:
        virtual void AddByte(unsigned char byte) = 0;

        long long GetHash() { return hash; }

        virtual void Init() = 0;

    public:
        virtual ~BaseRollingHash()
        {
            
        }
};