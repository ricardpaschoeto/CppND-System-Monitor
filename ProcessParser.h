#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <bits/stdc++.h> 
#include <boost/algorithm/string.hpp>
#include "constants.h"
//#include "util.h"


using namespace std;

class ProcessParser{
private:
    std::ifstream stream;
    public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static std::string getVmSize(string pid);
    static std::string getCpuPercent(string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getNumberOfCores();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid);
};

// TODO: Define all of the above functions below:
string ProcessParser::getVmSize(string pid)
{
    string line;

    string name = "VmData";
    string value;
    float result;

    ifstream stream;
    Util::getStream((Path::basePath() + pid + Path::statusPath()), stream);
    while(std::getline(stream, line)){

        if (line.compare(0, name.size(),name) == 0) {

            vector<string> values;
            boost::split(values, line , boost::is_any_of(":"));
            result = (stof(values[1])/float(1024*1024));
            break;
        }
    }
    return to_string(result);
}

string ProcessParser::getCpuPercent(string pid)
{
    string line;
    string value;
    float result;
    ifstream stream;
    Util::getStream((Path::basePath()+ pid + "/" + Path::statPath()), stream);
    getline(stream, line);

    vector<string> values;
    boost::split(values, line, boost::is_any_of(" "));

    float utime = stof(ProcessParser::getProcUpTime(pid));
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float starttime = stof(values[21]);
    float uptime = ProcessParser::getSysUpTime();
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime/freq);
    result = 100.0*((total_time/freq)/seconds);
    return to_string(result);
}

string ProcessParser::getProcUpTime(string pid)
{
    string line;
    string value;
    float result;
    ifstream stream;
    Util::getStream((Path::basePath() + pid + "/" +  Path::statPath()), stream);
    getline(stream, line);
    vector<string> values;
    boost::split(values, line, boost::is_any_of(" "));

    return to_string(float(stof(values[13])/sysconf(_SC_CLK_TCK)));
}

long int ProcessParser::getSysUpTime()
{
    string line;
    ifstream stream;
    Util::getStream((Path::basePath() + Path::upTimePath()), stream);
    getline(stream,line);
    vector<string> values;
    boost::split(values, line, boost::is_any_of(" "));
    return stoi(values[0]);
}

string ProcessParser::getProcUser(string pid)
{
    string line;
    string name = "Uid:";
    string result ="";
    ifstream stream, user_stream;
    Util::getStream((Path::basePath() + pid + Path::statusPath()), stream);
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
            vector<string> values;
            boost::split(values, line, boost::is_any_of("\t"));
            result =  values[1];
            break;
        }
    }
    Util::getStream("/etc/passwd", user_stream);
    name =("x:" + result);
    while (std::getline(user_stream, line)) {
        if (line.find(name) != std::string::npos) {
            result = line.substr(0, line.find(":"));
            return result;
        }
    }
    return "";
}

bool validPid(string pidname){

    for(char c: pidname){
        if(!isdigit(c))
            return false;
    }

    return true;    
}

vector<string> ProcessParser::getPidList()
{
    DIR* dir;

    vector<string> pids;
    if(!(dir = opendir("/proc")))
        throw std::runtime_error(std::strerror(errno));

    while (dirent* dirp = readdir(dir)) {

        if(dirp->d_type != DT_DIR)
            continue;

        if (validPid(dirp->d_name)) {
            pids.push_back(dirp->d_name);
        }
    }

    if(closedir(dir))
        throw std::runtime_error(std::strerror(errno));
    return pids;
}

string ProcessParser::getCmd(string pid)
{
    string line;
    ifstream stream;
    Util::getStream((Path::basePath() + pid + Path::cmdPath()), stream);
    std::getline(stream, line);
    return line;
}

int ProcessParser::getNumberOfCores()
{
    string line;
    string name = "cpu cores";
    ifstream stream;
    Util::getStream((Path::basePath() + "cpuinfo"), stream);
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
            vector<string> values;
            boost::split(values, line, boost::is_any_of(":"));
            return stoi(values[1]);
        }
    }
    return 0;
}

vector<string> ProcessParser::getSysCpuPercent(string coreNumber)
{
    string line;
    string name = "cpu" + coreNumber;
    ifstream stream;
    Util::getStream((Path::basePath() + Path::statPath()), stream);
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            return values;
        }
    }
    return (vector<string>());
}

float getSysActiveCpuTime(vector<string> values)
{
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}

float getSysIdleCpuTime(vector<string>values)
{
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

string ProcessParser::PrintCpuStats(vector<string> values1, vector<string> values2)
{
    float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
    float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
    float totalTime = activeTime + idleTime;
    float result = 100.0*(activeTime / totalTime);
    return to_string(result);
}

float findMemParameters(string line, string name)
{
    if(line.compare(0, name.size(), name) == 0){
        vector<string> totalMem;
        boost::split(totalMem, line, boost::is_any_of(":")); 
        vector<string> valueAvailable;
        boost::algorithm::trim_left(totalMem[1]);
        boost::split(valueAvailable, totalMem[1], boost::is_any_of(" "));
        return stof(valueAvailable[0]);
    }

    return -1;
}

float ProcessParser::getSysRamPercent()
{
    string line;
    string name1 = "MemAvailable:";
    string name2 = "MemFree:";
    string name3 = "Buffers:";

    float totalMem = -1;
    float freeMem = -1;
    float buffers = -1;

    ifstream totalMemStream;
    Util::getStream((Path::basePath() + Path::memInfoPath()), totalMemStream);
    while(getline(totalMemStream, line)){
        totalMem = findMemParameters(line, name1);
        if (totalMem != -1)
            break;
    }

    ifstream freeMemStream;
    Util::getStream((Path::basePath() + Path::memInfoPath()), freeMemStream);
    while(getline(freeMemStream, line)){
        freeMem = findMemParameters(line, name2);

        if (freeMem != -1)
            break;
    }

    ifstream buffersStream;
    Util::getStream((Path::basePath() + Path::memInfoPath()), buffersStream);
    while(getline(buffersStream, line)){
        buffers = findMemParameters(line, name3);

        if (buffers != -1)
            break;
    }

    return float(100.0*(1-(freeMem/(totalMem-buffers))));
}

string ProcessParser::getSysKernelVersion()
{
    string name = "Linux version";
    string line;
    ifstream stream;
    vector<string> values;
    Util::getStream((Path::basePath() + Path::versionPath()), stream);
    while(getline(stream, line)){
        if(line.compare(0, name.size(), name) == 0){
            boost::split(values, line, boost::is_any_of(" "));
        }
    }

    return values[2];
}

string ProcessParser::getOSName()
{
    string name = "PRETTY_NAME=";
    string line;
    ifstream stream;
    vector<string> values;
    Util::getStream(("/etc/os-release"), stream);
    while(getline(stream, line)){
        if(line.compare(0, name.size(), name) == 0){
            boost::split(values, line, boost::is_any_of("="));
        }
    }

    return values[1];
}

int ProcessParser::getTotalThreads()
{
    vector<string> pids =  getPidList();
    vector<string> values;
    string name = "Threads";
    string line;
    int result = 0;

    for(string pid : pids){
        ifstream stream;
        Util::getStream((Path::basePath() + pid + Path::statusPath()), stream);
        while(getline(stream, line)){
            if(line.compare(0, name.size(), name) == 0){
                boost::split(values, line, boost::is_any_of(":"));
                result += stoi(values[1]);
            }
        }
    }
    cout << "\n";

    return result;
}

int ProcessParser::getTotalNumberOfProcesses()
{
    string name = "processes";
    string line;
    ifstream stream;
    int result;
    vector<string> values;
    Util::getStream((Path::basePath() + Path::statPath()), stream);
    while(getline(stream, line)){
        if(line.compare(0, name.size(), name) == 0){
            boost::split(values, line, boost::is_any_of(" "));
            result = stoi(values[1]);
            break;
        }
    }
    return result;
}

int ProcessParser::getNumberOfRunningProcesses()
{
    string name = "procs_running";
    string line;
    ifstream stream;
    int result;
    vector<string> values;
    Util::getStream((Path::basePath() + Path::statPath()), stream);
    while(getline(stream, line)){
        if(line.compare(0, name.size(), name) == 0){
            boost::split(values, line, boost::is_any_of(" "));
            result = stoi(values[1]);
            break;
        }
    }
    return result;
}

bool ProcessParser::isPidExisting(string pid)
{
    vector<string> pids = getPidList();
    vector<string>::iterator it = find(pids.begin(), pids.end(), pid);

    if (it != pids.end())
        return true;

    return false;

}



