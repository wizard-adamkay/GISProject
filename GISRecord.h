//
// Created by Adam k on 11/22/2022.
//

// FEATURE_ID|FEATURE_NAME|FEATURE_CLASS|
// STATE_ALPHA|STATE_NUMERIC|COUNTY_NAME|
// COUNTY_NUMERIC|PRIMARY_LAT_DMS|PRIM_LONG_DMS|
// PRIM_LAT_DEC|PRIM_LONG_DEC|SOURCE_LAT_DMS|
// SOURCE_LONG_DMS|SOURCE_LAT_DEC|SOURCE_LONG_DEC|
// ELEV_IN_M|ELEV_IN_FT|MAP_NAME|
// DATE_CREATED|DATE_EDITED
#ifndef GISPROJECT_GISRECORD_H
#define GISPROJECT_GISRECORD_H

#include <string>
#include <iostream>
#include <vector>

using namespace std;
class GISRecord {
public:
    GISRecord(const string& input){
        string holder;
        vector<string> splitInputs;
        for(char i : input) {
            if (i != '|') {
                holder += i;
            } else {
                splitInputs.push_back(holder);
                holder = "";
            }
        }
        if(splitInputs.size() == 19){
            splitInputs.emplace_back("");
        }
        featureID = stol(splitInputs[0]);
        featureName = splitInputs[1];
        featureClass = splitInputs[2];
        stateAlpha = splitInputs[3];
        stateNumeric = splitInputs[4];
        countyName = splitInputs[5];
        countyNumeric = splitInputs[6];
        primaryLatitudeDMS = splitInputs[7];
        primaryLongitudeDMS = splitInputs[8];
        primaryLatitudeDEC = stod(splitInputs[9]);
        primaryLongitudeDEC = stod(splitInputs[10]);
        sourceLatitudeDMS = splitInputs[11];
        sourceLongitudeDMS = splitInputs[12];
        if(splitInputs[13] != ""){
            sourceLatitudeDEC = stod(splitInputs[13]);
        }
        if(splitInputs[14] != ""){
            sourceLongitudeDEC = stod(splitInputs[13]);
        }
        elevationMeters = stoi(splitInputs[15]);
        elevationFeet = stoi(splitInputs[16]);
        mapName = splitInputs[17];
        dateCreated = splitInputs[18];
        dateEdited = splitInputs[19];
    }
    long featureID;
    string featureName;
    string featureClass;
    string stateAlpha;
    string stateNumeric;
    string countyName;
    string countyNumeric;
    string primaryLatitudeDMS;
    string primaryLongitudeDMS;
    double primaryLatitudeDEC;
    double primaryLongitudeDEC;
    string sourceLatitudeDMS;
    string sourceLongitudeDMS;
    double sourceLatitudeDEC;
    double sourceLongitudeDEC;
    int elevationMeters;
    int elevationFeet;
    string mapName;
    string dateCreated;
    string dateEdited;

    friend ostream &operator<<(ostream &os, const GISRecord &record) {
        os << "featureID: " << record.featureID << " featureName: " << record.featureName << " featureClass: "
           << record.featureClass << " stateAlpha: " << record.stateAlpha << " stateNumeric: " << record.stateNumeric
           << " countyName: " << record.countyName << " countyNumeric: " << record.countyNumeric
           << " primaryLatitudeDMS: " << record.primaryLatitudeDMS << " primaryLongitudeDMS: "
           << record.primaryLongitudeDMS << " primaryLatitudeDEC: " << record.primaryLatitudeDEC
           << " primaryLongitudeDEC: " << record.primaryLongitudeDEC << " sourceLatitudeDMS: "
           << record.sourceLatitudeDMS << " sourceLongitudeDMS: " << record.sourceLongitudeDMS << " sourceLatitudeDEC: "
           << record.sourceLatitudeDEC << " sourceLongitudeDEC: " << record.sourceLongitudeDEC << " elevationMeters: "
           << record.elevationMeters << " elevationFeet: " << record.elevationFeet << " mapName: " << record.mapName
           << " dateCreated: " << record.dateCreated << " dateEdited: " << record.dateEdited;
        return os;
    }
};
#endif //GISPROJECT_GISRECORD_H
