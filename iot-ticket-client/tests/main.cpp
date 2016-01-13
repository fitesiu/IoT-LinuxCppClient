
#include "cppunit/XmlOutputter.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/ui/text/TestRunner.h"
#include <fstream>
#include <exception>
#include <iostream>
#include <unistd.h>

#include "IOT_Tester.h"

int main(int argc, char* argv[])
{
    std::string user;
    std::string pass;

    int opt = 0;
    while ((opt = getopt(argc, argv, "u:p:")) != -1)
    {
        switch (opt)
        {
        case 'u':
            user = std::string(optarg);
            break;
        case 'p':
            pass = std::string(optarg);
            break;
        default:
            break;
        }
    }

    if(user.empty() || pass.empty()) {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << " -u IOT_API_USER -p IOT_API_PWD" << std::endl;
        return -1;
    }

    IOT_Tester::SetCredentials(user, pass);

    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry& registry = CppUnit::TestFactoryRegistry::getRegistry();
    std::ofstream outputFile("iot-ticket-test-results.xml");
    CppUnit::XmlOutputter* outputter = new CppUnit::XmlOutputter( &runner.result(), outputFile );
    runner.setOutputter(outputter);
    runner.addTest( registry.makeTest() );
    runner.run();
    outputFile.close();
    return 0;
}
