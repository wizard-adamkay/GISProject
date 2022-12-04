//
// Created by Adam k on 11/22/2022.
//

#ifndef GISPROJECT_NAMEINDEX_H
#define GISPROJECT_NAMEINDEX_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct entry{
    vector<int> offsets;
    string nameAndAbbrv;
};

using namespace std;
class HashTable {
public:
    HashTable(){
        tableSize = 1024;
        numEntries = 0;
        entries.resize(tableSize);
    }

    int insert(const string& s, int offset){
        int numTries = 0;
        size_t h = hash(s);
        if(entries[h].nameAndAbbrv.empty()){
            numEntries++;
            entries[h].nameAndAbbrv = s;
            entries[h].offsets.push_back(offset);
        } else if (entries[h].nameAndAbbrv == s){
            entries[h].offsets.push_back(offset);
        } else {
            while(true){
                numTries++;
                size_t h2 = quadraticProbe(numTries, h);
                if(entries[h2].nameAndAbbrv.empty()){
                    numEntries++;
                    entries[h2].nameAndAbbrv = s;
                    entries[h2].offsets.push_back(offset);
                    break;
                } else if (entries[h2].nameAndAbbrv == s) {
                    entries[h2].offsets.push_back(offset);
                    break;
                }
            }
        }
        if(float(numEntries) / float(tableSize) > .7){
            rehash();
        }
        return ++numTries;
    }

    vector<int> find(const string& s){
        vector<int> results;
        size_t h = hash(s);
        if(entries[h].nameAndAbbrv.empty()){
            return results;
        } else if (entries[h].nameAndAbbrv == s){
            return entries[h].offsets;
        } else {
            int numTries = 0;
            while(true){
                numTries++;
                size_t h2 = quadraticProbe(numTries, h);
                if(entries[h2].nameAndAbbrv.empty()){
                    return results;
                } else if (entries[h2].nameAndAbbrv == s){
                    return entries[h2].offsets;
                }
            }
        }
    }
    string debug(){
        string s;
        for(int i = 0; i < tableSize; i++){
            if(entries[i].nameAndAbbrv.empty()){
                continue;
            }
            s+='['+ to_string(i)+"]: "+entries[i].nameAndAbbrv + "(";
            for(int j : entries[i].offsets){
                s+= to_string(j)+',';
            }
            s+=")\n";
        }
        return s;
    }
    int tableSize;
    int numEntries;
private:
    vector<entry> entries;

    size_t quadraticProbe(int n, size_t hash) const{
        return (((n*n+n)/2+hash)%tableSize);
    }

    //elf hash from https://www.programmingalgorithms.com/algorithm/elf-hash/cpp/
    size_t hash(const string& s) const{
        size_t hash = 0;
        size_t x;
        for (char i : s){
            hash = (hash << 4) + i;
            if ((x = hash & 0xF0000000) != 0)
            {
                hash ^= (x >> 24);
            }
            hash &= ~x;
        }
        return hash % tableSize;
    }

    void insert(const string& s, const vector<int>& offsets){
        size_t h = hash(s);
        if(entries[h].nameAndAbbrv.empty()){
            numEntries++;
            entries[h].nameAndAbbrv = s;
            entries[h].offsets = offsets;
        } else {
            int numTries = 0;
            while(true){
                numTries++;
                size_t h2 = quadraticProbe(numTries, h);
                if(entries[h2].nameAndAbbrv.empty()){
                    numEntries++;
                    entries[h2].nameAndAbbrv = s;
                    entries[h2].offsets = offsets;
                    break;
                }
            }
        }
    }

    void rehash() {
        vector<entry> oldEntries = entries;
        tableSize *= 2;
        entries.resize(tableSize);
        entries.clear();
        numEntries = 0;
        for (auto &entry: oldEntries)
            insert(entry.nameAndAbbrv, entry.offsets);
    }
};
class NameIndex {
public:
    NameIndex(){
        hashTable = HashTable();
    }
    int insert(const string& line, int offset){
        vector<string> s = processLine(line);
        string featureName = s[1];
        string stateAbbrv = s[3];
        return hashTable.insert(featureName+'\t'+stateAbbrv, offset);
    }
    vector<int> find(const string& s){
        return hashTable.find(s);
    }
    string debug(){
        return hashTable.debug();
    }
private:
    HashTable hashTable;

    static vector<string> processLine(const string& line){
        string holder;
        vector<string> splitInputs;
        for(char i : line) {
            if (i != '|') {
                holder += i;
            } else {
                splitInputs.push_back(holder);
                holder = "";
            }
        }
        return splitInputs;
    }
};
#endif //GISPROJECT_NAMEINDEX_H
