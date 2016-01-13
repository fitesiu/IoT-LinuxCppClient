/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Wapice Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <IOT_API.h>
#include <string>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <sys/sysinfo.h>
#include <iomanip>

static const std::string SERVER_ADDRESS  = "https://my.iot-ticket.com/api/v1/";
static const long LOGGING_INTERVAL_US    = 500000;
static const long SERVER_SEND_INERVAL_US = 1000000;


static std::mutex iotDataLock;
static std::vector<IOT_WriteData> iotData;

void printQuota(IOT_API& api);
void printDatanodes(std::string devID, IOT_API& api);
void readMeasurements();
void dataSenderThread(std::string devID, IOT_API& api);


int main(int argc, char* argv[])
{
    std::string user;
    std::string pass;
    std::string devID;

    int opt = 0;
    while ((opt = getopt(argc, argv, "u:p:d:")) != -1)
    {
        switch (opt)
        {
        case 'u':
            user = std::string(optarg);
            break;
        case 'p':
            pass = std::string(optarg);
            break;
        case 'd':
            devID = std::string(optarg);
            break;
        default:
            break;
        }
    }

    if(user.empty() || pass.empty()) {
        std::cout << "Usage: " << argv[0] << "-u USER -p PASSWORD [-d DEVICE_ID]" << std::endl;
        return -1;
    }

    IOT_API api(SERVER_ADDRESS, user, pass);
    printQuota(api);
    printDatanodes(devID, api);

    if(devID.empty()) {
        IOT_RegDevice dev;
        dev.SetName("LinuxPC-DemoApp");
        dev.SetManufacturer("Wapice");
        if(api.RegisterDevice(dev, devID) != IOTAPI::IOT_ERR_OK) {
            return false;
        }
        std::cout << "Registered new device, ID=" << devID << std::endl;
    }

    std::thread t(dataSenderThread, devID, std::ref(api));

    readMeasurements();
    return 0;
}


void printQuota(IOT_API& api)
{
    IOT_Quota quota;
    if(api.GetQuota(quota) != IOTAPI::IOT_ERR_OK) {
        std::cerr << "Failed to get quota" << std::endl;
        return;
    }
    else {
        double usedPercentage = static_cast<double>(quota.GetUsedStorage()) /
                static_cast<double>(quota.GetMaxStorage()) * 100.0d;

        std::cout << "Quota:" << std::endl;
        std::cout << "Devices:              " << quota.GetTotalDevices() << std::endl;
        std::cout << "Max Devices:          " << quota.GetMaxDevicesAllowed() << std::endl;
        std::cout << "Max Nodes Per Device: " << quota.GetMaxNodesPerDevice() << std::endl;
        std::cout << "Used Storage:         " << quota.GetUsedStorage();
        std::cout << std::setprecision(3) << " (" << usedPercentage << " %)" << std::endl;
        std::cout << "Max Storage:          " << quota.GetMaxStorage() << std::endl;
        std::cout << std::endl;
    }
}


void printDatanodes(std::string devID, IOT_API& api)
{
    std::vector<IOT_ReadData> nodes;

    if(api.GetDatanodes(devID, nodes) != IOTAPI::IOT_ERR_OK) {
        std::cerr << "Failed to get datanodes" << std::endl;
        return;
    }

    std::cout << "Existing datanodes:" << std::endl;
    for(size_t i=0; i<nodes.size(); ++i) {
        std::cout << "  " << nodes.at(i).GetPath() << "/" << nodes.at(i).GetName()
                  << std::endl;
    }
    std::cout << std::endl;
}


void readMeasurements()
{
    IOT_WriteData load;
    IOT_WriteData uptime;
    IOT_WriteData freeram;
    IOT_WriteData processes;

    load.SetName("Load Average");
    uptime.SetName("Uptime");
    uptime.SetUnit("s");
    freeram.SetName("Free RAM");
    freeram.SetUnit("Mb");
    processes.SetName("Processes");

    std::cout << "Start measurements" << std::endl;
    while(true) {
        struct sysinfo info;

        if(sysinfo(&info) != 0) {
            std::cerr << "Failed to read sysinfo!" << std::endl;
            return;
        }

        load.SetValue(static_cast<double>(info.loads[0] / static_cast<double>(1 << SI_LOAD_SHIFT) ));
        load.SetTimeToNow();

        uptime.SetValue(static_cast<double>(info.uptime));
        uptime.SetTimeToNow();

        double ram = (info.freeram * info.mem_unit) / 1000000.0d;
        freeram.SetValue(ram);
        freeram.SetTimeToNow();

        processes.SetValue(static_cast<double>(info.procs));
        processes.SetTimeToNow();

        iotDataLock.lock();
        iotData.push_back(load);
        iotData.push_back(uptime);
        iotData.push_back(freeram);
        iotData.push_back(processes);
        iotDataLock.unlock();

        usleep(LOGGING_INTERVAL_US);
    }
}


void dataSenderThread(std::string devID, IOT_API& api)
{
    std::cout << "Start server sender thread" << std::endl;
    while(true) {
        std::vector<IOT_WriteData> localData;

        iotDataLock.lock();
        if(!iotData.empty()) {
            localData.swap(iotData);
        }
        iotDataLock.unlock();

        if(localData.empty()) {
            usleep(SERVER_SEND_INERVAL_US);
            continue;
        }

        if(api.SendData(devID, localData) != IOTAPI::IOT_ERR_OK) {
            std::cerr << "Failed to send data!" << std::endl;
        }

        usleep(SERVER_SEND_INERVAL_US);
    }
}

