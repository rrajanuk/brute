// brute.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma warning(disable : 4996)
#include <iostream>
#include <stdlib.h>
#include <thread>

#include "Buffer.h"

/******************************/
/*Character Array that stores
all letters in the range passed
through command line arguements*/
/*******************************/
char* charList;


/*@brief The method that equally divides the work between threads
* @params wordLen The length of the word
* @params threadCount Based on number of Cores 
* @buffObj That stores all the hashs and the respective values
* @&threads Reference to list of Threads
**/

void createThreads(int wordLen, int threadCount, std::shared_ptr<Buffer> &bufObj, std::vector<std::thread*> &threads) {
    
    int  index = 0;
    int charListSize = std::strlen(charList);
    int charSetSize = charListSize / threadCount;
    int difference = charListSize % threadCount;
    int equallydividedWork = charSetSize;
    // The character list is divided into equal set of characters
    // Each character Set is then used to generate strings that creates hashes
    size_t i = 0;
    while (index < charListSize) {
        if (charSetSize == 0)
            charSetSize = 1;
        if(i + 1 == threadCount) 
            charSetSize += difference;
        
        // Threads are created 
        threads[i] = new std::thread(&Buffer::add, bufObj, charList, index, charSetSize, charListSize, wordLen);
      
        index = charSetSize;
        charSetSize = charSetSize + equallydividedWork;
        if (charSetSize >= charListSize) {
            charSetSize = charListSize;
        }
        i++;
    }
    
    // Once all threads are created the join() method is called to wait
    for (size_t j = 0; j < threads.size(); ++j) { // Wait for all threads to finish
        threads[j]->join();
        delete threads[i];
    }
    std::cout << "\n Finished executing Threads";
    if (!bufObj->remove()) {
        return;
    }
}

/****************************************************
* @brief The method creates the character list
* based on the range given in the commandline arguement
* @note if in the string a ":" is not found  , the method
* considers the string range as invalid.
* @params Arguement number 3 the in the command line
******************************************************/



bool createCharList(std::string range) {
    bool valid = false;
    int itr = range.find(":");
    if (itr != std::string::npos) {
        std::string firstStr = range.substr(0, itr);
        std::string secondStr = range.substr(itr + 1);
        int startFirst = atoi(firstStr.substr(0, firstStr.find("-")).c_str());
        int endFirst = atoi(firstStr.substr(firstStr.find("-") + 1).c_str());
        int startSecond = atoi(secondStr.substr(0, secondStr.find("-")).c_str());
        int endSecond = atoi(secondStr.substr(secondStr.find("-") + 1).c_str());

        int arraySize = endSecond - startSecond + endFirst - startFirst + 2 ;
        charList = new char(arraySize);
        
        std::cout <<'\t'<< startFirst << '\t' << endFirst << '\t' << startSecond << '\t' << endSecond << '\n' << arraySize;
        charList[arraySize] = '\0';
        bool run = true;
        while (run) {
            size_t i = 0;
            for (int j = startFirst; j <= endFirst; j++) {
                charList[i] = (char)j;
                i++;
            }
            std::cout << i << '\n';
            for (int k = startSecond; k <= endSecond; k++) {
                charList[i] = (char)k;
                i++;
            }
            std::cout << i << '\n';
            run = false;
            valid = true;
        }
        
        int sizeofArr = std::strlen(charList);
        std::cout << "The characters to check : \t";
        for (int j = 0; j < (int)std::strlen(charList); j++) {
            std::cout << charList[j];
        }
        std::cout << "\n";
    }
    return valid;
}


/****************************************************************
* @brief The main function that receives the command line interface
* it receives command line arguements in this format.
* C:\\Debug>brute.exe 3 10 65-90:97-122 hash_to_check
*****************************************************************/





int main(int argc, char* argv[])
{  

    // Checks if all arguements are passed if not it tells the usage
    if (argc < 5) {
        std::cout << "\n CORRECT USAGE : brute.exe <keyspace Minimum Size>  <keyspace Maximum Size> <Range of Characters> <md5 hash>\n\n";
        std::cout << "\tFor Example: To specifiy a keyspace with size ranging from 3 to 10 character \n";
        std::cout << "with characters 'A' to 'Z' and 'a' to 'z' on the command line can be specified as below\n";
        std::cout << "*******\n";
        std::cout << "\t C:\\Debug>brute.exe 3 10 65-90:97-122 hash_to_check \n";
        std::cout << "*******";
    } 
    // checks if the passed hash has the correct length
    else if (strlen(argv[4]) != 32) {
        std::cout << "\n ERROR : MD5 Hash passed is not a valid hash, Hash has incorrect length \n";
        std::cout << "\n Correct usage: brute.exe <keyspace Minimum Size>  <keyspace Maximum Size> <Range of Characters> <md5 hash>\n\n";
        
    }
    // finds the number of processors 
    int processor_count = std::thread::hardware_concurrency();
    std::cout << "The number of cores is " << processor_count << '\n';
    if (processor_count == 0)
        std::cout << "\n \t ERROR: Unable to detect number of cores to determine number of threads.";
    std::cout << argv[3];
    std::string range = argv[3];

    // Creates the list of characters based on range
    bool valid = createCharList(range);
    if (!valid) {
        std::cout << "\t C:\\Debug>brute.exe 3 10 65-90:97-122 hash_to_check \n";
        std::cout << "***** The Third Arguement in the command line should be in this format 65-90:97-122 ***** \n";
    }
   
    
    int minLen = atoi(argv[1]);
    int maxLen = atoi(argv[2]);
    std::shared_ptr<Buffer> bufObj(new Buffer(0, argv[4]));
    std::vector<std::thread*> threads(processor_count);

    // for each length the Threads are created and once their job is done they destroyed
    for (int length = minLen; length <= maxLen; length++) {
        std::cout << length << "\n";
        createThreads(length, processor_count, bufObj, threads);
    }
    
    return 0;
}


