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

        void readByte(char* arr, int size)
        {
            strncpy(arr, m_strData.c_str() + m_nIdx, size);
            m_nIdx += size;
            // for(int i=m_nIdx; i < m_nIdx + size && i < m_strData.length() ; i++)
            // {
            //     arr[i - m_nIdx] = m_strData[i];
            // }
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