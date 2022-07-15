#include <iostream>
#include <string>
#include <string.h>
#include <cmath>
#include <vector>

using namespace std;

typedef unsigned char byte;

const int base = 257;
const int mod = 1000000007;

int rolling_hash_code(const char* data)
{
    long long ret = 0;
    size_t length = strlen(data);

    for (int i = 0; i < length; i++)
    {
        ret = ret * base + int(data[i]);
        ret %= mod;
    }

    return ret < 0 ? ret + mod : ret;

    // size_t length = strlen(data);
    // int result = 0;
    // for(int i = 0;i <= length-1; i++)
    // {
    //     int char_code = (data[i] - 'a' + 1);
    //     result += char_code * std::pow(base, length - 1 - i);
    // }

    // return (result % mod + mod) % mod;
}

int rabin_karp_algo(const string& pattern, const string& data)
{
    long long hash1 = rolling_hash_code(pattern.c_str());
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

    	if (i >= pattern.size()-1 && hash1 == hash2){
            cout<<"Found match at position "<< i - (pattern.size()-1) <<endl;
        }
    }

    return -1;

}

int get_next_rolling_hash(char data, int hash, char new_data, int window_length)
{
    int first_char_code = (int)data;
    int last_new_char_code = int(new_data);
    int result = (hash - first_char_code * std::pow(base, window_length - 1)) * base + last_new_char_code;

    return (result % mod + mod) % mod;
}

int main()
{
    string s1 = "this is phenomenalomen";
    string sMatch = "is";

    rabin_karp_algo(sMatch, s1);

    int a[] = {1,2,3};
    char str[] = "this is phenomenalomen";
    vector<char> v(str, str + strlen(str));
    strlen(str);
    const char* pattern = "ome";
    int target_hash = rolling_hash_code(pattern);
    cout<<"Target hash: "<<target_hash<<endl;
    int window_length = strlen(pattern);

    char current_substr[strlen(pattern)];
    memset(current_substr,0, strlen(current_substr));

    strncpy(current_substr, str, strlen(pattern));
    int current_hash = rolling_hash_code(current_substr);

    if(target_hash == current_hash)
    {
        cout<<"Found substring starting at position 0"<<endl;
    }

    char substr[3] = {0};
    for(int i=window_length; i< strlen(str); i++)
    {
        current_hash = get_next_rolling_hash(str[i - window_length], current_hash, str[i], window_length);
        std::copy(v.begin() + i, v.begin() + i + window_length, current_substr);
        int alternative_current_hash = rolling_hash_code(current_substr);

        cout<<"Hashes for "<< current_substr <<" : "<< current_hash << " " << alternative_current_hash<<endl;

        if(target_hash == current_hash){
            cout<<"Found substring starting at position "<<i - window_length + 1<<endl;
        }
    }

    std::cout<<"Xsss";
    int x=1;
    x++;

    return 0;
}