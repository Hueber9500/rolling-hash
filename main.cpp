#include <iostream>
#include <string>
#include <string.h>
#include <cmath>
#include <vector>
#include <bitset>
#include <fstream>
#include <map>
#include <sstream>
#include <openssl/sha.h>
#include "FileStream.h"
#include "StringStream.h"
#include <memory>
#include <algorithm>

#include <iomanip>
#include <utility>
#include "ChunkInfo.h"
#include "Chunker.h"
#include "Comparer.h"

using namespace std;

const int base = 2287;
const int window_size = 4;
const long long mod = 100000012459;

int rollingHashCode(const char* data)
{
    long long ret = 0;
    size_t length = strlen(data);

    for (int i = 0; i < length; i++)
    {
        ret = ret * base + int(data[i]);
        ret %= mod;
    }
    
    return ret < 0 ? ret + mod : ret;
}

std::string convertByteArrayToHex(unsigned char* arr, size_t size)
{
    std::string hex (size * 2, ' ');
    for(size_t i = 0; i < size; i++)
    {
        hex[2 * i]     = hexmap[(arr[i] & 0xF0) >> 4];
        hex[2 * i + 1] = hexmap[arr[i] & 0x0F];
    }

    return hex;
}

std::string text()
{
    return "The fact that template type deduction deduces the “wrong” types for 0 and NULL (i.e., their true types, rather than their fallback meaning as a representation for a null pointer) is the most compelling reason to use nullptr instead of 0 or NULL when you want to refer to a null pointer. With nullptr, templates pose no special challenge. Combined with the fact that nullptr doesn’t suffer from the overload resolution sur prises that 0 and NULL are susceptible to, the case is ironclad.";
}

std::string text2()
{
    return "that template type deduction deduces the “wrong” types for 0 and NULL (i.e., their true types, rather than their fallback meaning as a representation for a null pointer) is the most compelling reason to use nullptr instead of 0 or NULL when you want to refer to a null pointer. With nullptr, templates pose no special challenge. Combined with the fact that nullptr doesn’t suffer from the overload resolution sur prises that 0 and NULL are susceptible to, the case is ironclad.";
}

int rabinKarpAlgo(const string& pattern, const string& data)
{
    long long hash1 = rollingHashCode(pattern.c_str());
    long long hash2 = 0;
    long long power = 1;

    for (int i = 0; i < pattern.size(); i++)
        power = (power * base) % mod;

    for (int i = 0; i < data.size(); i++)
    {
        hash2 = hash2 * base + int(data[i]);
        hash2 %= mod;

    	if (i >= pattern.size())
    	{
            hash2 -= power * int(data[i-pattern.size()]) % mod;
            hash2 += mod;
            hash2 %= mod;
        }

    	if (i >= pattern.size() - 1 && hash1 == hash2){
            cout<<"Found match at position "<< i - (pattern.size() - 1) <<endl;
        }
    }

    return -1;

}

int getNextRollingHash(char data, int hash, char new_data, int window_length)
{
    int first_char_code = (int)data;
    int last_new_char_code = int(new_data);
    int result = (hash - first_char_code * std::pow(base, window_length - 1)) * base + last_new_char_code;

    return (result % mod + mod) % mod;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void test2()
{
    std::string original(text());
    std::string modified(text2());

    StringStream ss1(original);
    StringStream ss2(modified);

    RollingHash hash(base, 4);

    vector<ChunkInfo> original_chunks = Chunker(&ss1, &hash).chunkFile();
    vector<ChunkInfo> new_chunks = Chunker(&ss2, &hash).chunkFile();

    auto delta = Comparer().delta(original_chunks, new_chunks, true);

    std::for_each(delta.begin(), delta.end(), [](DifferenceDescriptor& el) {
        std::cout<< (el.action == Action::Delete ? "Remove from " : "Add from ")<< (el.from_file == FileType::New ? "new " : "old")<<" file: ";
        std::cout<<" "<<" "<<el.from_position<<" "<<el.to_position<<endl;
    });

    for(vector<ChunkInfo>::const_iterator it = new_chunks.cbegin(); it!= new_chunks.cend(); it++)
    {
        int found = false;

        for(vector<ChunkInfo>::const_iterator old_it = original_chunks.cbegin(); old_it != original_chunks.cend(); old_it++)
        {
            if( strncmp(it->sha1_hex, old_it->sha1_hex, sizeof(it->sha1_hex)) == 0)
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            std::cout<<"Original file is missing: \n"<< it->data<< "\nAt position "<<it->offset << " with length "<<it->length<<endl;
        }
    }

    cout<<endl;

    for(vector<ChunkInfo>::const_iterator it = original_chunks.cbegin(); it!= original_chunks.cend(); it++)
    {
        int found = false;

        for(vector<ChunkInfo>::const_iterator old_it = new_chunks.cbegin(); old_it != new_chunks.cend(); old_it++)
        {
            if( strncmp(it->sha1_hex, old_it->sha1_hex, sizeof(it->sha1_hex)) == 0 )
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            std::cout<<"New file is missing:\n"<< it->data<< "\nAt position "<<it->offset << " with length "<<it->length<<endl;
        }
    }
}

void test()
{
    string original("original.txt");
    string new_file("new.txt");

    fstream out_stream;

    out_stream.open(original, std::ios_base::out);

    if(out_stream.is_open())
    {
        out_stream<<text();
        cout<<text()<<endl;
        out_stream.flush();
        out_stream.close();
        out_stream.clear();
    }

    out_stream.open(new_file, std::ios_base::out);

    if(out_stream.is_open())
    {
        std::string str(text());
        replaceAll(str, "denounc", "renounc");
        cout<<str<<endl;
        out_stream<<str;
        out_stream.flush();
        out_stream.close();
        out_stream.clear();
    }

    FileStream fstream(original.c_str());
    RollingHash hash(2273, 4);

    Chunker chunker(&fstream, &hash);
    FileStream fstream2(new_file.c_str());
    
    vector<ChunkInfo> original_chunks = chunker.chunkFile();
    vector<ChunkInfo> new_chunks = Chunker(&fstream2, &hash).chunkFile();

    cout<<"New file chunks count "<<new_chunks.size()<< " Original file chunks count "<<original_chunks.size()<<endl;

    for(vector<ChunkInfo>::const_iterator it = new_chunks.cbegin(); it!= new_chunks.cend(); it++)
    {
        int found = false;

        for(vector<ChunkInfo>::const_iterator old_it = original_chunks.cbegin(); old_it != original_chunks.cend(); old_it++)
        {
            
            if( strncmp(it->sha1_hex, old_it->sha1_hex, sizeof(it->sha1_hex)) == 0)
            {
                found = true;
                break;
            }
        }
        cout<<endl;

        if(!found)
        {
            std::cout<<"Original file is missing: "<< it->data<< " At position "<<it->offset << " with length "<<it->length<<endl;
        }
    }

    for(vector<ChunkInfo>::const_iterator it = original_chunks.cbegin(); it!= original_chunks.cend(); it++)
    {
        int found = false;

        for(vector<ChunkInfo>::const_iterator old_it = new_chunks.cbegin(); old_it != new_chunks.cend(); old_it++)
        {
            if( strncmp(it->sha1_hex, old_it->sha1_hex, sizeof(it->sha1_hex)) == 0 )
            {
                found = true;
                break;
            }
        }
        cout<<endl;

        if(!found)
        {
            std::cout<<"New file is missing: "<< it->data<< " At position "<<it->offset << " with length "<<it->length<<endl;
        }
    }

    remove(original.c_str());
    remove(new_file.c_str());
}

int main()
{
    test2();
    return 0;
}