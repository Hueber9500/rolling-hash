#pragma once
#include "ByteStream.h"
#include <fstream>

class FileStream : public ByteStream
{
    private:
        std::ifstream file;

    public:
        FileStream(const char* file_path) 
            : file(file_path, std::ios::in | std::ios::binary)
        {
        }

        FileStream(const FileStream& other) = delete;

        FileStream(FileStream&& other) = delete;

        FileStream& operator=(FileStream&& rhs) = delete;

        FileStream& operator=(const FileStream& rhs) = delete;

        void readBytes(char* arr, int size)
        {
            if (file) file.read(arr, size);
        }

        bool isEndOfStream()
        {
            return file.eof();
        }

        int length()
        {
            int current_position = file.tellg();
            file.seekg(0, file.end);
            int length = file.tellg();
            file.seekg(current_position, file.beg);

            return length;
        }

        virtual ~FileStream()
        {
            if (file.is_open())
            {
                file.close();
            }
        }
};