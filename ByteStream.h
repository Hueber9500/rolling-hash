#pragma once

class ByteStream
{
    public:
        virtual void readByte(char* arr, int size) = 0;

        virtual bool isEndOfStream() = 0;

        virtual int length() = 0;

        virtual ~ByteStream()
        {
            
        }
};