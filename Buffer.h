#pragma once
#include <mutex>
#include <unordered_map>
#include <condition_variable>

#include "md5.h"
MD5 md5;

typedef std::unordered_map<std::string, std::string> MAP;


/*******************************************************************************************
* @brief A Recursive method that generates all strings based on an charList Array 
* @params char set[] The array has all the characters that the method uses to generate all string
* @params string prefix The prefix is added to all newly created strings, if it is empty the prefix is not added
* @params int n The size of the Characters passed in the set[] array
* @params int k The length of the strings to be created
* @params MAP &buffer the data structure to store hash key and string value
**/



void printAllKLengthRec(char set[], std::string prefix, int n, int k, MAP &buffer)
{

    // Base case: k is 0, 
    // print prefix 
    if (k == 0)
    {
        std::cout << (prefix) << std::endl;
        char* c = const_cast<char*>(prefix.c_str());
        buffer[md5.digestString(c)] = prefix;
        puts(md5.digestString(c));
        return;
    }

    // One by one add all characters  
    // from set and recursively  
    // call for k equals to k-1 
    for (int i = 0; i < n; i++)
    {
        std::string newPrefix;

        // Next character of input added 
        newPrefix = prefix + set[i];

        // k is decreased, because  
        // we have added a new character 
        printAllKLengthRec(set, newPrefix, n, k - 1, buffer);
    }

}

/****************************************************
*Buffer that Adds and Removes hash key and string values from unordered map 
********************************************************/




class Buffer
{
public:

    /**************************************************************
    *@brief adds the hash keys and values to the buffer
    *@params charList[] array of all characters to generate strings
    *@params index the starting point in the charList array 
    *@params charSetSize the end index point in the charList array
    *@params charListSize the total size of the charList array
    *@params wordLen length of the word to generate strings
    ******************************************************************/
    void add(char charList[], int index, int charSetSize, int charListSize, int wordLen) {
               
        while (true) {
            std::unique_lock<std::mutex> locker(mu);
            
            if (charSetSize == 0)
                charSetSize = 1;
            while (index < charSetSize) {
                std::string prefix(1, charList[index]);
                std::cout << prefix << '\n';
                printAllKLengthRec(charList, prefix, charListSize, wordLen - 1, buffer_);
                index++;
            }
            locker.unlock();
            cond.notify_all();
            return;
        }
    }

    /******************************************************
    *@brief Checks if the hash to check is in the buffer
      and response accordingly
    *******************************************************/
    bool remove() {
        bool cracked = false;
        while (true)
        {
            //buffer_.size() > 0;
            std::unique_lock<std::mutex> locker(mu);
            cond.wait(locker, [this]() {return buffer_.size() > 0; });
            const auto itrHash = buffer_.find(hash_);
            if (itrHash == buffer_.end()) {
                buffer_.erase(itrHash, buffer_.end());
            }
            else {
                std::cout.flush();
                std::cout << "\t*********\n\t SUCCESS :: The Password of the Hash to Check is " << itrHash->first << " is " << itrHash->second;
                cracked = true;
            }
            locker.unlock();
            cond.notify_all();
            return cracked;
        }
    }

    /******************************
     *Get the size of the Map Buffer
     *****************************/
    int getBufferSize() const {
        return buffer_size;
    }

    /*********************************
     *Set the size of the Map Buffer
     *********************************/

    void setBufferSize(int size) {
        buffer_size = size;
    }

    Buffer(int size, std::string hash):buffer_size(size), hash_(hash){}
    Buffer():buffer_size(0){}
private:
    
    std::mutex mu;
    std::condition_variable cond;

    MAP buffer_;
    int  buffer_size;
    std::string hash_; //hash to check
};