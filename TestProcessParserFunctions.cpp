#include "ProcessParser.h"
#include "Process.h"
#include <stdio.h>
#include <iostream>


int main(){

    ProcessParser processparser;
    Process process("2469");

    string vmsize = processparser.getVmSize("2469");
    string cpupercent = processparser.getCpuPercent("2469");
    string procuptimne = processparser.getProcUpTime("2469");
    long int sysuptime = processparser.getSysUpTime();
    string procuser = processparser.getProcUser("2469");
    std::vector<string> pids = processparser.getPidList();
    int cores = processparser.getNumberOfCores();
    std::vector<string> cpus = processparser.getSysCpuPercent("6");
    float cpuactivetime = getSysActiveCpuTime(cpus);
    float cpuidle = getSysIdleCpuTime(cpus);
    float rampercent = processparser.getSysRamPercent();
    string version = processparser.getSysKernelVersion();
    string osname = processparser.getOSName();
    int totalThreads = processparser.getTotalThreads();
    int totalProcesses = processparser.getTotalNumberOfProcesses();
    int totalRunning = processparser.getNumberOfRunningProcesses();
    bool foundPid = processparser.isPidExisting("2469");
    string proc = process.getProcess();

    cout << "-- Results to getVmSize function: " << vmsize << "\n";
    cout << "-- Results to getCpuPercent function: " << cpupercent << "\n";
    cout << "-- Results to getProcUpTime function: " << procuptimne << "\n";
    cout << "-- Results to getSysUpTime function: " << sysuptime << "\n";
    cout << "-- Results to getProcUser function: " << procuser << "\n";
    cout << "-- Results to getPidList function: ";

    for(string pid :pids){
        std::cout << " " << pid;
    }
    std::cout << "\n";

    cout << "-- Results to getNumberOfCores function: " << cores << "\n";
    cout << "-- Results to getSysCpuPercent function: ";

    for(string cpu : cpus){
        std::cout << " " << cpu;
    }
    std::cout << "\n";

    cout << "-- Results to getSysActiveCpuTime function: " << cpuactivetime << "\n";
    cout << "-- Results to getSysIdleCpuTime function: " << cpuidle << "\n";
    cout << "-- Results to getSysRamPercent function: " << rampercent << "\n";
    cout << "-- Results to getSysKernelVersion function: " << version << "\n";
    cout << "-- Results to getOSName function: " << osname << "\n";
    cout << "-- Results to getTotalThreads function: " << totalThreads << "\n";
    cout << "-- Results to getTotalNumberOfProcesses function: " << totalProcesses << "\n";
    cout << "-- Results to getNumberOfRunningProcesses function: " << totalRunning << "\n";
    cout << "-- Results to isPidExisting function: " << foundPid << "\n";
    cout << "-- Results to Class Process -> getProcess function: " << proc << "\n";

}