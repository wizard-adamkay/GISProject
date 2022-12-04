#include <iostream>
#include <string>
#include "GISRecord.h"
#include "Logger.h"
#include "CommandProcessor.h"

using namespace std;

bool is_file_exist(const string& fileName)
{
    ifstream infile(fileName);
    return infile.good();
}

int main(int argc, char **argv) {
    if(argc != 4){
        cout << argc << endl;
        cout << "use ./GIS <database_file_name> <command_script_file_name> <log_file_name>" << endl;
        return -1;
    }
    Logger logger(argv[3], argv[1], argv[2]);
    string databaseFileName = argv[1];
    string commandScriptFileName = argv[2];
    string logFileName = argv[3];
    if (!is_file_exist(commandScriptFileName)){
        cout << commandScriptFileName << " does not exist" << endl;
        exit(0);
    }
    CommandProcessor commandProcessor = CommandProcessor(commandScriptFileName,databaseFileName, logger);
    return 0;
}
