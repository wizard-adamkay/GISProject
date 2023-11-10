//
// Created by Adam k on 11/22/2022.
//

#ifndef GISPROJECT_BUFFERPOOL_H
#define GISPROJECT_BUFFERPOOL_H

#include <string>
#include <list>
#include "GISRecord.h"

using namespace std;

class BufferPool {
public:
    void clearPool(){
        GISList.clear();
    }
    void add(const vector<GISRecord>& records){
        for(const GISRecord& record : records){
            for(const GISRecord& record1 : GISList) {
                if (record == record1) {
                    GISList.remove(record1);
                    break;
                }
            }
            if(GISList.size() >= poolMax){
                GISList.pop_back();
            }
            GISList.push_front(record);
        }
    }
    string debug(){
        string s = "MRU\n";
        for(const GISRecord& record : GISList){
            s+='\t'+to_string(record.offset)+":\t\""+record.featureName+"\" \""+record.countyName+"\" \""+record.stateAlpha+"\" \""+record.primaryLatitudeDMS+"\" \""+record.primaryLongitudeDMS+"\"\n";
        }
        s+= "LRU";
        return s;
    }
private:
    int poolMax = 15;
    list<GISRecord> GISList;

};
#endif //GISPROJECT_BUFFERPOOL_H
