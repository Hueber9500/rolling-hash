#include "RollingHash.h"
#include <fstream>
#include "ByteStream.h"
#include "ChunkInfo.h"
#include <vector>
#include <openssl/sha.h>

const char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

class Chunker
{
    private:
        ByteStream* m_pStream;
        BaseRollingHash* m_pRollingHash;

    public:
        Chunker(ByteStream* pStream, BaseRollingHash* pHash) :
            m_pStream(pStream), m_pRollingHash(pHash)
        {
        }

    public:
        std::vector<ChunkInfo> chunkFile()
        {
            /// 16 byte chunks
            int mask = (1 << 4) - 1;
            int window_size = 4;

            std::vector<unsigned char> bytes_buffer;
            bytes_buffer.reserve(1024);
            std::vector<ChunkInfo> m_info;

            unsigned char output_hash [20] = {0};
            int index = 0;
            int last = 0;
            int length = m_pStream->length();

            while (!m_pStream->isEndOfStream())
            {
                char current_byte;
            
                m_pStream->readByte(&current_byte, 1);

                m_pRollingHash->AddByte(current_byte);
                bytes_buffer.push_back(current_byte);

                if( ((m_pRollingHash->GetHash() | mask) == m_pRollingHash->GetHash()) || index + 1 == length )
                {
                    /// calcualte sha 1 hash
                    /// convert hash to hex
                    SHA1(&bytes_buffer[0], bytes_buffer.size(), output_hash);

                    std::vector<unsigned char> hash_hex = this->convertByteArrayToHex(output_hash, sizeof(output_hash));

                    /// add rolling hash to map
                    ChunkInfo ci(
                        (int)bytes_buffer.size(), 
                        last,
                        m_pRollingHash->GetHash(),
                        std::string((char*)&bytes_buffer[0], bytes_buffer.size()),
                        (char*)&hash_hex[0]
                    );

                    m_info.push_back(ci);
                    last = index;
                    
                    memset(output_hash, 0, sizeof(output_hash));
                    bytes_buffer.clear();
                    m_pRollingHash->Init();
                }

                index++;
            }

            return m_info;
        }


    private:
        std::vector<unsigned char> convertByteArrayToHex(unsigned char* arr, size_t size)
        {
            std::vector<unsigned char> result;
            result.reserve(size * 2);

            for(size_t i = 0; i < size; i++)
            {
                result.push_back(hexmap[(arr[i] & 0xF0) >> 4]);
                result.push_back(hexmap[arr[i] & 0x0F]);
            }

            return result;
        }
};