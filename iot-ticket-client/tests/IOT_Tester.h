#ifndef IOT_TESTER_H
#define IOT_TESTER_H

#include "cppunit/extensions/HelperMacros.h"
#include "IOT_ReadData.h"


class IOT_Tester : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( IOT_Tester );
    CPPUNIT_TEST( testRegisterInvalidFields );
    CPPUNIT_TEST( testRegisterDevice );
    CPPUNIT_TEST( testGetDevices );
    CPPUNIT_TEST( testGetSingleDevice );
    CPPUNIT_TEST( testWriteLong );
    CPPUNIT_TEST( testWriteBool );
    CPPUNIT_TEST( testWriteDouble );
    CPPUNIT_TEST( testWriteString );
    CPPUNIT_TEST( testWriteBinary );
    CPPUNIT_TEST( testQuota );
    CPPUNIT_TEST( testQuotaDevice );
    CPPUNIT_TEST( testGetDatanodes );
    CPPUNIT_TEST( testReadLongData );
    CPPUNIT_TEST( testReadBoolData );
    CPPUNIT_TEST( testReadDoubleData );
    CPPUNIT_TEST( testReadStringData );
    CPPUNIT_TEST( testReadBinaryData );
    CPPUNIT_TEST( testMultithread );
    CPPUNIT_TEST_SUITE_END();

public:
    static void SetCredentials(const std::string& user, const std::string& pass);

    void testRegisterInvalidFields();
    void testRegisterDevice();
    void testGetDevices();
    void testGetSingleDevice();
    void testWriteLong();
    void testWriteBool();
    void testWriteDouble();
    void testWriteString();
    void testWriteBinary();
    void testQuota();
    void testQuotaDevice();
    void testGetDatanodes();
    void testReadLongData();
    void testReadBoolData();
    void testReadDoubleData();
    void testReadStringData();
    void testReadBinaryData();
    void testMultithread();

private:
    bool CheckDatanode(const std::string& name, const std::vector<IOT_ReadData>& nodes) const;

    static std::string m_user;
    static std::string m_pass;
    static std::string m_devId;
};


#endif // IOT_TESTER_H
