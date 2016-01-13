#include "IOT_Tester.h"
#include <IOT_API.h>
#include <sstream>
#include <unistd.h>
#include <thread>


CPPUNIT_TEST_SUITE_REGISTRATION( IOT_Tester );

std::string IOT_Tester::m_user = "<your_user>";
std::string IOT_Tester::m_pass = "<your_pass>";
std::string IOT_Tester::m_devId = "<your_dev>";

static std::string SERVER_URL = "https://my.iot-ticket.com/api/v1/";

static std::string INVALID_LEN_NAME(IOTAPI::IOTAPI_MAX_NAME_LEN+1, 'x');
static std::string INVALID_LEN_DESC(IOTAPI::IOTAPI_MAX_DESCRIPTION_LEN+1, 'x');
static std::string INVALID_LEN_ATTR(IOTAPI::IOTAPI_MAX_ATTRIBUTE_LEN+1, 'x');


void IOT_Tester::SetCredentials(const std::string& user, const std::string& pass)
{
    m_user = user;
    m_pass = pass;
}

void IOT_Tester::testRegisterInvalidFields()
{
    IOT_RegDevice dev;

    CPPUNIT_ASSERT(!dev.SetName(INVALID_LEN_NAME));
    CPPUNIT_ASSERT(!dev.SetManufacturer(INVALID_LEN_NAME));
    CPPUNIT_ASSERT(!dev.SetType(INVALID_LEN_NAME));
    CPPUNIT_ASSERT(!dev.SetDescription(INVALID_LEN_DESC));
    CPPUNIT_ASSERT(!dev.AppendAttribute("key", INVALID_LEN_ATTR));
    CPPUNIT_ASSERT(!dev.AppendAttribute(INVALID_LEN_ATTR, "attr"));

    std::map<std::string, std::string> attributes;

    for(size_t i=0; i<IOTAPI::IOTAPI_MAX_NUM_OF_ATTRIBUTES; ++i) {
        std::stringstream ss;
        ss << i;
        attributes[ss.str()] = ss.str();
    }

    CPPUNIT_ASSERT(dev.SetAttributes(attributes));
    CPPUNIT_ASSERT(!dev.AppendAttribute("key", "attr"));

    attributes["X"] = "Y";
    CPPUNIT_ASSERT(!dev.SetAttributes(attributes));
}

void IOT_Tester::testRegisterDevice()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    IOT_RegDevice dev;
    CPPUNIT_ASSERT(dev.SetName("MyTestDevice"));
    CPPUNIT_ASSERT(dev.SetManufacturer("Wapice"));
    CPPUNIT_ASSERT(dev.SetType("IoT-API TEST"));
    CPPUNIT_ASSERT(dev.SetDescription("Test description"));
    CPPUNIT_ASSERT(dev.AppendAttribute("TestAttribute", "12345"));

    std::string devID;
    CPPUNIT_ASSERT(api.RegisterDevice(dev, devID) == IOTAPI::IOT_ERR_OK);
    m_devId = devID;
}

void IOT_Tester::testGetDevices()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    std::vector<IOT_GetDevice> devices;
    CPPUNIT_ASSERT(api.GetDevices(devices) == IOTAPI::IOT_ERR_OK);
}

void IOT_Tester::testGetSingleDevice()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    IOT_GetDevice device;
    CPPUNIT_ASSERT(api.GetDevice(m_devId, device) == IOTAPI::IOT_ERR_OK);
    CPPUNIT_ASSERT(device.GetDeviceID() == m_devId);
    CPPUNIT_ASSERT(device.GetName() == "MyTestDevice");
    CPPUNIT_ASSERT(device.GetManufacturer() == "Wapice");
    CPPUNIT_ASSERT(device.GetType() == "IoT-API TEST");
    CPPUNIT_ASSERT(device.GetDescription() == "Test description");
    CPPUNIT_ASSERT(device.GetAttribute("TestAttribute") == "12345");
    CPPUNIT_ASSERT(!device.GetCreated().empty());
    CPPUNIT_ASSERT(!device.GetHref().empty());
}

void IOT_Tester::testWriteLong()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    std::vector<IOT_WriteData> data;
    IOT_WriteData val;
    CPPUNIT_ASSERT(val.SetName("Value"));
    CPPUNIT_ASSERT(val.SetPath("Test/Path"));
    CPPUNIT_ASSERT(val.SetUnit("U"));

    for(int i=0; i<10; ++i) {
        val.SetValue(static_cast<int64_t>(i));
        val.SetTimeToNow();
        data.push_back(val);
        usleep(1000);
    }

    CPPUNIT_ASSERT(api.SendData(m_devId, data) == IOTAPI::IOT_ERR_OK);
}

void IOT_Tester::testWriteBool()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    IOT_WriteData val;
    CPPUNIT_ASSERT(val.SetName("BoolValue"));
    CPPUNIT_ASSERT(val.SetUnit("B"));

    val.SetValue(true);
    val.SetTimeToNow();

    CPPUNIT_ASSERT(api.SendData(m_devId, val) == IOTAPI::IOT_ERR_OK);
}

void IOT_Tester::testWriteDouble()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    IOT_WriteData val;
    CPPUNIT_ASSERT(val.SetName("DoubleValue"));
    CPPUNIT_ASSERT(val.SetUnit("D"));

    val.SetValue(12.345d);
    val.SetTimeToNow();

    CPPUNIT_ASSERT(api.SendData(m_devId, val) == IOTAPI::IOT_ERR_OK);
}

void IOT_Tester::testWriteString()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    IOT_WriteData val;
    CPPUNIT_ASSERT(val.SetName("StringValue"));
    CPPUNIT_ASSERT(val.SetUnit("S"));

    val.SetValue("test string");
    val.SetTimeToNow();

    CPPUNIT_ASSERT(api.SendData(m_devId, val) == IOTAPI::IOT_ERR_OK);
}

void IOT_Tester::testWriteBinary()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    IOT_WriteData val;
    CPPUNIT_ASSERT(val.SetName("Binary"));
    CPPUNIT_ASSERT(val.SetPath("Test/Path"));

    std::string testData = "This is test binary data!";
    val.SetValue((uint8_t*)testData.c_str(), testData.length());

    CPPUNIT_ASSERT(api.SendData(m_devId, val) == IOTAPI::IOT_ERR_OK);
}

void IOT_Tester::testQuota()
{
    IOT_API api(SERVER_URL, m_user, m_pass);
    IOT_Quota quota;

    CPPUNIT_ASSERT(api.GetQuota(quota) == IOTAPI::IOT_ERR_OK);
    CPPUNIT_ASSERT(quota.GetMaxDevicesAllowed() != 0);
    CPPUNIT_ASSERT(quota.GetMaxNodesPerDevice() != 0);
    CPPUNIT_ASSERT(quota.GetMaxStorage() != 0);
    CPPUNIT_ASSERT(quota.GetTotalDevices() != 0);
    CPPUNIT_ASSERT(quota.GetUsedStorage() != 0);
}

void IOT_Tester::testQuotaDevice()
{
    IOT_API api(SERVER_URL, m_user, m_pass);
    IOT_QuotaDevice quota;

    CPPUNIT_ASSERT(api.GetQuota(m_devId, quota) == IOTAPI::IOT_ERR_OK);
    CPPUNIT_ASSERT(quota.GetDataNodesCount() != 0);
    CPPUNIT_ASSERT(quota.GetDeviceId() == m_devId);
    CPPUNIT_ASSERT(quota.GetMaxRequestsPerDay() != 0);
    CPPUNIT_ASSERT(quota.GetReadRequestsToday() == 0);
    CPPUNIT_ASSERT(quota.GetStorageSize() != 0);
}

void IOT_Tester::testGetDatanodes()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    std::vector<IOT_ReadData> datanodes;
    CPPUNIT_ASSERT(api.GetDatanodes(m_devId, datanodes) == IOTAPI::IOT_ERR_OK);

    CPPUNIT_ASSERT(datanodes.size() == 5);
    CPPUNIT_ASSERT(CheckDatanode("Value", datanodes));
    CPPUNIT_ASSERT(CheckDatanode("BoolValue", datanodes));
    CPPUNIT_ASSERT(CheckDatanode("DoubleValue", datanodes));
    CPPUNIT_ASSERT(CheckDatanode("StringValue", datanodes));
    CPPUNIT_ASSERT(CheckDatanode("Binary", datanodes));
}

void IOT_Tester::testReadLongData()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    IOT_ReadDataFilter filter;
    filter.AddDatanode("Value", "/Test/Path");
    filter.SetFromDate(0);
    filter.SetLimit(10);

    std::vector<IOT_ReadData> readdata;
    CPPUNIT_ASSERT(api.ReadData(m_devId, filter, readdata) == IOTAPI::IOT_ERR_OK);
    CPPUNIT_ASSERT(readdata.size() == 1);

    CPPUNIT_ASSERT(readdata.at(0).ProcessValues() == 10);

    for(size_t i=0; i<readdata.at(0).ProcessValues(); ++i) {
        long val = -1;
        unsigned long ts = 0;
        CPPUNIT_ASSERT(readdata.at(0).GetConvertedValue(i, val));
        CPPUNIT_ASSERT(readdata.at(0).GetTimestamp(i, ts));
        CPPUNIT_ASSERT(val == static_cast<long>(i));
        CPPUNIT_ASSERT(ts != 0);
    }
}

void IOT_Tester::testReadBoolData()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    IOT_ReadDataFilter filter;
    filter.AddDatanode("BoolValue");
    filter.SetFromDate(0);
    filter.SetLimit(1);

    std::vector<IOT_ReadData> readdata;
    CPPUNIT_ASSERT(api.ReadData(m_devId, filter, readdata) == IOTAPI::IOT_ERR_OK);
    CPPUNIT_ASSERT(readdata.size() == 1);

    CPPUNIT_ASSERT(readdata.at(0).ProcessValues() == 1);

    bool val = false;
    CPPUNIT_ASSERT(readdata.at(0).GetConvertedValue(0, val));
    CPPUNIT_ASSERT(val == true);
}

void IOT_Tester::testReadDoubleData()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    IOT_ReadDataFilter filter;
    filter.AddDatanode("DoubleValue");
    filter.SetFromDate(0);
    filter.SetLimit(1);

    std::vector<IOT_ReadData> readdata;
    CPPUNIT_ASSERT(api.ReadData(m_devId, filter, readdata) == IOTAPI::IOT_ERR_OK);
    CPPUNIT_ASSERT(readdata.size() == 1);

    CPPUNIT_ASSERT(readdata.at(0).ProcessValues() == 1);

    double val = 0.0d;
    CPPUNIT_ASSERT(readdata.at(0).GetConvertedValue(0, val));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(12.345d, val, 0.000001);
}

void IOT_Tester::testReadStringData()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    IOT_ReadDataFilter filter;
    filter.AddDatanode("StringValue");
    filter.SetFromDate(0);
    filter.SetLimit(1);

    std::vector<IOT_ReadData> readdata;
    CPPUNIT_ASSERT(api.ReadData(m_devId, filter, readdata) == IOTAPI::IOT_ERR_OK);
    CPPUNIT_ASSERT(readdata.size() == 1);

    CPPUNIT_ASSERT(readdata.at(0).ProcessValues() == 1);

    std::string val;
    CPPUNIT_ASSERT(readdata.at(0).GetConvertedValue(0, val));
    CPPUNIT_ASSERT(val == "test string");
}

void IOT_Tester::testReadBinaryData()
{
    IOT_API api(SERVER_URL, m_user, m_pass);

    IOT_ReadDataFilter filter;
    filter.AddDatanode("Binary");
    filter.SetFromDate(0);
    filter.SetLimit(1);

    std::vector<IOT_ReadData> readdata;
    CPPUNIT_ASSERT(api.ReadData(m_devId, filter, readdata) == IOTAPI::IOT_ERR_OK);
    CPPUNIT_ASSERT(readdata.size() == 1);

    CPPUNIT_ASSERT(readdata.at(0).ProcessValues() == 1);

    std::vector<uint8_t> decoded;
    CPPUNIT_ASSERT(readdata.at(0).GetConvertedValue(0, decoded));

    std::string correct = "This is test binary data!";
    CPPUNIT_ASSERT(correct.size() == decoded.size());

    for(size_t i=0; i<correct.size(); ++i ) {
        CPPUNIT_ASSERT(static_cast<uint8_t>(correct.at(i)) == decoded.at(i));
    }
}

void IOT_Tester::testMultithread()
{
    auto func = [=](std::string name){
        IOT_API api(SERVER_URL, m_user, m_pass);

        IOT_WriteData val;
        CPPUNIT_ASSERT(val.SetName(name));

        for(int i=0; i<10; ++i) {
            val.SetValue(static_cast<int64_t>(i));
            val.SetTimeToNow();
            CPPUNIT_ASSERT(api.SendData(m_devId, val) == IOTAPI::IOT_ERR_OK);
            usleep(1000);
        }
    };

    std::thread t1(func, "Node1");
    std::thread t2(func, "Node2");

    t1.join();
    t2.join();
}

bool IOT_Tester::CheckDatanode(const std::string& name, const std::vector<IOT_ReadData>& nodes) const
{
    for(size_t i=0; i<nodes.size(); ++i) {
        if(nodes.at(i).GetName() == name)
            return true;
    }
    return false;
}


