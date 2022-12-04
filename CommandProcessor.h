//
// Created by Adam k on 11/22/2022.
//

#ifndef GISPROJECT_COMMANDPROCESSOR_H
#define GISPROJECT_COMMANDPROCESSOR_H
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Logger.h"
#include "CoordinateIndex.h"
#include "NameIndex.h"
#include "BufferPool.h"


using namespace std;

enum Command {
    world,
    import,
    debug,
    quit,
    what_is_at,
    what_is,
    what_is_in
        };
class CommandProcessor {
public:
    CommandProcessor(const string &scriptFileName, const string &dbName, Logger &logger) : logger(logger) {
        dbFileName = dbName;
        dbFile.open(dbName);
        string line;
        ifstream infile(scriptFileName);
        int count = 0;
        while (getline(infile, line))
        {
            if(line[0] == ';'){
                logger.log(line);
                continue;
            }
            stringstream s(line);
            string com, args;
            s >> com >> ws;
            Command command = convertToCommand(com);
            logger.breakLine();
            logger.log("Command " + to_string(count++) + ": " + line+"\n");
            getline(s, args);
            execute(command, args);
        }
    }

private:
    Logger& logger;
    ofstream dbFile;
    string dbFileName;
    string westBoundLongDMS;
    string eastBoundLongDMS;
    string southBoundLatDMS;
    string northBoundLatDMS;
    int westBoundLongSec;
    int eastBoundLongSec;
    int southBoundLatSec;
    int northBoundLatSec;
    CoordinateIndex coordinateIndex;
    NameIndex nameIndex;
    BufferPool bufferPool;
    void execute(Command command, const string& arguments){
        switch(command){
            case quit:
                logger.shutdown();
                break;
            case world: {
                stringstream s(arguments);
                s >> westBoundLongDMS >> eastBoundLongDMS >> southBoundLatDMS >> northBoundLatDMS;
                westBoundLongSec = DMStoS(westBoundLongDMS);
                eastBoundLongSec = DMStoS(eastBoundLongDMS);
                southBoundLatSec = DMStoS(southBoundLatDMS);
                northBoundLatSec = DMStoS(northBoundLatDMS);
                if(northBoundLatSec < southBoundLatSec || eastBoundLongSec < westBoundLongSec){
                    logger.log("cannot handle looped areas");
                    logger.shutdown();
                }
                string worldLog = "World boundaries are set to:\n";
                worldLog += "\t\t"+ to_string(northBoundLatSec)+'\n';
                worldLog+= to_string(westBoundLongSec)+"\t\t\t"+ to_string(eastBoundLongSec)+'\n';
                worldLog+= "\t\t"+ to_string(southBoundLatSec);
                logger.log(worldLog);
                coordinateIndex = CoordinateIndex(westBoundLongSec, northBoundLatSec, eastBoundLongSec, southBoundLatSec);
                nameIndex = NameIndex();
                bufferPool = BufferPool();
                break;
            }
            case import: {
                string line;
                ifstream infile(arguments);
                getline(infile, line);
                int count = 0;
                int largestProbeSeq = 0;
                while (getline(infile, line)) {
                    if(is_inside_world(line)){
                        count++;
                        coordinateIndex.insert(line, count);
                        int probeSeq = nameIndex.insert(line, count);
                        largestProbeSeq = max(largestProbeSeq, probeSeq);
                        dbFile << line << endl;
                    }
                }
                logger.log("num inserted into db: " + to_string(count));
                logger.log("longest probe sequence: " + to_string(largestProbeSeq));
                infile.close();
                break;
            }
            case debug:
                if (arguments == "quad"){
                    logger.log(coordinateIndex.debug());
                } else if(arguments == "hash"){
                    logger.log(nameIndex.debug());
                } else if (arguments == "pool"){
                    logger.log(bufferPool.debug());
                }
                break;
            case what_is_at: {
                stringstream s(arguments);
                string lat, lon;
                s >> lat >> lon;
                vector<int> offsets = coordinateIndex.find(DMStoS(lon), DMStoS(lat), 0, 0);
                vector<GISRecord> records = offsetsToGISRecords(offsets);
                bufferPool.add(records);
                if(records.empty()){
                    logger.log("No matches found");
                } else {
                    logger.log("The following "+ to_string(records.size()) +" feature(s) were found:");
                }
                for(const GISRecord& record : records){
                    logger.log('\t'+to_string(record.offset)+":\t\""+record.featureName+"\" \""+record.countyName+"\" \""+record.stateAlpha+"\" \""+record.primaryLatitudeDMS+"\" \""+record.primaryLongitudeDMS+"\"");
                }
                break;
            }
            case what_is:{
                vector<int> results = nameIndex.find(arguments);
                vector<GISRecord> records = offsetsToGISRecords(results);
                bufferPool.add(records);
                if(records.empty()){
                    logger.log("No matches found");
                } else {
                    logger.log("The following "+ to_string(records.size()) +" feature(s) were found:");
                }
                for(const GISRecord& record : records){
                    logger.log('\t'+to_string(record.offset)+":\t\""+record.countyName+"\" \""+record.primaryLatitudeDMS+"\" \""+record.primaryLongitudeDMS+"\"");
                }
                break;
            }

            case what_is_in:{
                stringstream s(arguments);
                string lat, lon, latArea, lonArea, filter, filterType;
                bool filtered = false;
                bool longtype = false;
                s >> filter;
                if(filter == "-long"){
                    longtype = true;
                    s >> lat >> lon >> latArea >> lonArea;
                } else if(filter == "-filter"){
                    filtered = true;
                    s >> filterType >> lat >> lon >> latArea >> lonArea;
                }else{
                    lat = filter;
                    s >> lon >> latArea >> lonArea;
                }
                vector<int> offsets = coordinateIndex.find(DMStoS(lon), DMStoS(lat), stoi(lonArea), stoi(latArea));
                vector<GISRecord> records = offsetsToGISRecords(offsets);
                if(filtered){
                    records = filterGISRecords(records,filterType);
                }
                bufferPool.add(records);
                if(records.empty()){
                    logger.log("No matches found");
                } else {
                    logger.log("The following "+ to_string(records.size()) +" feature(s) were found:");
                }
                for(const GISRecord& record : records){
                    if(longtype){
                        logger.log(record.str());
                    } else{
                        logger.log('\t'+to_string(record.offset)+":\t\""+record.featureName+"\" \""+record.countyName+"\" \""+record.stateAlpha+"\" \""+record.primaryLatitudeDMS+"\" \""+record.primaryLongitudeDMS+"\"");
                    }
                }
                break;
            }

        }
    }

    bool is_inside_world(const string& line) const{
        stringstream ss(line);
        string token;
        for(int i = 0; i < 8; i++) {
            getline(ss, token, '|');
        }
        int latitudeSec = DMStoS(token);
        getline(ss, token, '|');
        int longitudeSec = DMStoS(token);
        bool insideHorizontally = false;
        bool insideVertically = false;
        if(longitudeSec > westBoundLongSec && longitudeSec < eastBoundLongSec){
            insideHorizontally = true;
        }
        if(latitudeSec > southBoundLatSec && latitudeSec < northBoundLatSec){
            insideVertically = true;
        }
        return insideVertically && insideHorizontally;
    }

    static Command convertToCommand(const string& com){
        if(com == "world") return world;
        if(com == "import") return import;
        if(com == "debug") return debug;
        if(com == "quit") return quit;
        if(com == "what_is_at") return what_is_at;
        if(com == "what_is") return what_is;
        if(com == "what_is_in") return what_is_in;
        throw invalid_argument(com + " : does not match any known commands");
    }

    static int DMStoS(const string& dms){
        int deg, min, sec;
        int neg = 1;
        int lon = 0;
        if ((dms[dms.length() - 1] == 'W') || (dms[dms.length() - 1] == 'E'))
        {
            lon++;
        }
        if ((dms[dms.length() - 1] == 'W') || (dms[dms.length() - 1] == 'S'))
        {
            neg = -1;
        }
        deg = stoi(dms.substr(0,2+lon));
        min = stoi(dms.substr(2+lon, 2));
        sec = stoi(dms.substr(4+lon, 2));
        int total = (((deg*60+min) * 60) + sec) * neg;
        return total;
    }

    vector<GISRecord> offsetsToGISRecords(vector<int> offsets){
        vector<GISRecord> records;
        std::sort(offsets.begin(), offsets.end());
        string line;
        int lineNumber=0;
        ifstream infile(dbFileName);
        for(int offset : offsets){
            for(; lineNumber < offset; lineNumber++){
                getline(infile, line);
            }
            GISRecord record(line);
            record.offset = offset;
            records.emplace_back(record);
        }
        return records;
    }

    static vector<GISRecord> filterGISRecords(const vector<GISRecord>& records, const string& filter){
        vector<GISRecord> filteredRecords;
        for(const GISRecord& record : records){
            if(filter == classToType(record.featureClass)){
                filteredRecords.push_back(record);
            }
        }
        return filteredRecords;
    }

    static string classToType(const string& c){
        string structure[12] = {"Airport", "Bridge", "Building", "Church", "Dam", "Hospital", "Levee", "Park", "Post Office", "School", "Tower", "Tunnel"};
        string water[17] = {"Arroyo",  "Bay", "Bend", "Canal", "Channel", "Falls", "Glacier", "Gut", "Harbor", "Lake", "Rapids", "Reservoir", "Sea", "Spring", "Stream", "Swamp", "Well"};
        string pop[1] = {"Populated Place"};
        for(const string& s : structure){
            if(c == s){
                return "structure";
            }
        }
        for(const string& s : water){
            if(c == s){
                return "water";
            }
        }
        for(const string& s : pop){
            if(c == s){
                return "pop";
            }
        }
        return "";
    }
};
#endif //GISPROJECT_COMMANDPROCESSOR_H
