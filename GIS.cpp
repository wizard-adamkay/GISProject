#include <iostream>
#include <string>
#include "GISRecord.h"
#include "Logger.h"
#include "CommandProcessor.h"

using namespace std;

bool is_file_exist(const char *fileName)
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
    for (int i = 0; i < argc; i++) {
        cout << argv[i] << "\n";
    }
    Logger logger(argv[3], argv[1], argv[2]);
    string databaseFileName = argv[1];
    string commandScriptFileName = argv[2];
    string logFileName = argv[3];
    cout << is_file_exist("../../text.txt") << endl;
//    string line;
//    ifstream infile("../GIS/VA_Monterey.txt");
//    getline(infile, line);
//    int count = 0;
//    while (getline(infile, line))
//    {
//        cout << "count: " << ++count << endl;
//        GISRecord rec = GISRecord(line);
//        cout << rec << endl;
//    }
    CommandProcessor commandProcessor = CommandProcessor(commandScriptFileName,databaseFileName, logger);
    return 0;
}
