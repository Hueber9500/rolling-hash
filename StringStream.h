#pragma once
#include <string>
#include "ByteStream.h"

class StringStream : public ByteStream
{
    private: 
        std::string m_strData;
        int m_nIdx;
    public:
        StringStream(std::string val) : m_strData(val), m_nIdx(0)
        {

        }

        StringStream(const StringStream& other) = delete;

        StringStream(StringStream&& other) = delete;

        StringStream& operator=(StringStream&& rhs) = delete;

        StringStream& operator=(const StringStream& rhs) = delete;

        void readBytes(char* arr, int size)
        {
            if (isEndOfStream()){
                return;
            }
            
            strncpy(arr, m_strData.c_str() + m_nIdx, size);
            m_nIdx += size;
        }

        bool isEndOfStream()
        {
            return m_nIdx >= m_strData.length();
        }

        int length()
        {
            return m_strData.length();
        }

        virtual ~StringStream()
        {

        } 
};