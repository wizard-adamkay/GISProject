//
// Created by Adam k on 11/22/2022.
//

#ifndef GISPROJECT_COMMANDPROCESSOR_H
#define GISPROJECT_COMMANDPROCESSOR_H
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Logger.h"

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
        loopedHorizontally = false;
        loopedVertically = false;
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
            logger.log("Command " + to_string(++count) + ": " + line);
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
    double westBoundLongDEC;
    double eastBoundLongDEC;
    double southBoundLatDEC;
    double northBoundLatDEC;
    bool loopedHorizontally;
    bool loopedVertically;
    void execute(Command command, string arguments){
        switch(command){
            case quit:
                logger.shutdown();
                break;
            case world: {
                stringstream s(arguments);
                s >> westBoundLongDMS >> eastBoundLongDMS >> southBoundLatDMS >> northBoundLatDMS;
                westBoundLongDEC = DMStoDEC(westBoundLongDMS);
                eastBoundLongDEC = DMStoDEC(eastBoundLongDMS);
                southBoundLatDEC = DMStoDEC(southBoundLatDMS);
                northBoundLatDEC = DMStoDEC(northBoundLatDMS);
                if(northBoundLatDEC < southBoundLatDEC){
                    loopedVertically = true;
                }
                if(eastBoundLongDEC < westBoundLongDEC){
                    loopedHorizontally = true;
                }
                break;
            }
            case import: {
                string line;
                ifstream infile(arguments);
                getline(infile, line);
                int count = 0;
                while (getline(infile, line)) {
                    if(is_inside_world(line)){
                        count++;
                        dbFile << line << endl;
                    }
                }
                cout << "num inserted into db: " << count << endl;
                infile.close();
                break;
            }
            case debug:

                break;
            case what_is_at:

                break;
            case what_is:

                break;
            case what_is_in:

                break;

        }
    }
    bool is_inside_world(const string line){
        stringstream ss(line);
        string token;
        for(int i = 0; i < 10; i++) {
            getline(ss, token, '|');
        }
        double latitudeDEC = stod(token);
        getline(ss, token, '|');
        double longitudeDEC = stod(token);
        bool insideHorizontally = false;
        bool insideVertically = false;
        if(loopedHorizontally){
            if(!(longitudeDEC > eastBoundLongDEC && longitudeDEC < westBoundLongDEC)){
                insideHorizontally = true;
            }
        } else if(longitudeDEC >= westBoundLongDEC && longitudeDEC <= eastBoundLongDEC){
            insideHorizontally = true;
        }
        if(loopedVertically){
            if(!(latitudeDEC > southBoundLatDEC && latitudeDEC < northBoundLatDEC)){
                insideVertically = true;
            }
        } else if(latitudeDEC >= southBoundLatDEC && latitudeDEC <= northBoundLatDEC){
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

    static double DMStoDEC(const std::string& dms)
    {
        string temp = dms;
        bool neg = false;
        if ((dms[dms.length() - 1] == 'W') || (dms[dms.length() - 1] == 'S'))
        {
            neg = true ;
        }
        int pointer = dms.length() - 2;
        string builder = "";
        builder += dms[pointer-1];
        builder += dms[pointer];
        double sec = stod(builder);
        builder = "";
        pointer-=2;
        builder += dms[pointer-1];
        builder += dms[pointer];
        double min = stod(builder);
        builder = "";
        pointer-=2;
        for(; pointer >= 0; pointer--){
            builder = dms[pointer] + builder;
        }
        double deg = stod(builder);
        double ang = deg + ((min + (sec / 60.0)) / 60.0);
        if (neg)
        {
            ang = -ang ;
        }
        return ang;
    }


};
#endif //GISPROJECT_COMMANDPROCESSOR_H
