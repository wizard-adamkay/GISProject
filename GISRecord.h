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
#include <sstream>
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
        sourceLatitudeDEC = 0;
        sourceLongitudeDEC = 0;
        if(splitInputs[13] != ""){
            sourceLatitudeDEC = stod(splitInputs[13]);
        }
        if(splitInputs[14] != ""){
            sourceLongitudeDEC = stod(splitInputs[13]);
        }
        elevationMeters = -9999;
        elevationFeet = -9999;
        if(splitInputs[15] != ""){
            elevationMeters = stoi(splitInputs[15]);
        }
        if(splitInputs[16] != ""){
            elevationFeet = stoi(splitInputs[16]);
        }
        mapName = splitInputs[17];
        dateCreated = splitInputs[18];
        dateEdited = splitInputs[19];
        offset = -1;
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
    int offset;

    string str() const {
        string s;
        if(featureID) s+= "featureID:\t\t" +  to_string(featureID) + "\n";
        if(!featureName.empty()) s+= "featureName:\t\t" +  featureName + "\n";
        if(!featureClass.empty()) s+= "featureClass:\t\t" +  featureClass + "\n";
        if(!stateAlpha.empty()) s+= "stateAlpha:\t\t" +  stateAlpha + "\n";
        if(!stateNumeric.empty()) s+= "stateNumeric:\t\t" +  stateNumeric + "\n";
        if(!countyName.empty()) s+= "countyName:\t\t" +  countyName + "\n";
        if(!countyNumeric.empty()) s+= "countyNumeric:\t\t" +  countyNumeric + "\n";
        if(!primaryLatitudeDMS.empty()) s+= "primaryLatitudeDMS:\t\t" +  primaryLatitudeDMS + "\n";
        if(!primaryLongitudeDMS.empty()) s+= "primaryLongitudeDMS:\t\t" +  primaryLongitudeDMS + "\n";
        s+= "primaryLatitudeDEC:\t\t" + to_string(primaryLatitudeDEC) + "\n";
        s+= "primaryLongitudeDEC:\t\t" +  to_string(primaryLongitudeDEC) + "\n";
        if(!sourceLatitudeDMS.empty()) s+= "sourceLatitudeDMS:\t\t" +  sourceLatitudeDMS + "\n";
        if(!sourceLongitudeDMS.empty()) s+= "sourceLongitudeDMS:\t\t" +  sourceLongitudeDMS + "\n";
        if(sourceLatitudeDEC != 0) s+= "sourceLatitudeDEC:\t\t" +  to_string(sourceLatitudeDEC) + "\n";
        if(sourceLongitudeDEC != 0) s+= "sourceLongitudeDEC:\t\t" +  to_string(sourceLongitudeDEC) + "\n";
        if(elevationMeters != -9999 && elevationFeet != -9999) s+= "elevationMeters:\t\t" + to_string(elevationMeters) + "\nelevationFeet:\t\t" +  to_string(elevationFeet) + "\n";
        if(!mapName.empty()) s+= "mapName:\t\t" +  mapName + "\n";
        if(!dateCreated.empty()) s+= "dateCreated:\t\t" +  dateCreated + "\n";
        if(!dateEdited.empty()) s+= "dateEdited:\t\t" +  dateEdited + "\n";
        if(offset != -1) s+= "offset:\t\t" + to_string(offset) + "\n";
        return s;
    }

    bool operator==(const GISRecord &rhs) const {
        return featureID == rhs.featureID &&
               featureName == rhs.featureName &&
               featureClass == rhs.featureClass &&
               countyName == rhs.countyName &&
               primaryLatitudeDMS == rhs.primaryLatitudeDMS &&
               primaryLongitudeDMS == rhs.primaryLongitudeDMS;
    }

    bool operator!=(const GISRecord &rhs) const {
        return !(rhs == *this);
    }

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
