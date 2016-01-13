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

#include "IOT_Base64Tester.h"
#include "IOT_Base64.h"
#include <string>


static const std::string testInputNoPadding  = "This is test input";
static const std::string testOutputNoPadding = "VGhpcyBpcyB0ZXN0IGlucHV0";

static const std::string testInput2Pad   = "This is test inputX";
static const std::string testOutput2Pad  = "VGhpcyBpcyB0ZXN0IGlucHV0WA==";

static const std::string testInput1Pad  = "This is test inputXX";
static const std::string testOutput1Pad = "VGhpcyBpcyB0ZXN0IGlucHV0WFg=";

static const std::string testInput1  = "f";
static const std::string testOutput1 = "Zg==";

static const std::string testInput2  = "fo";
static const std::string testOutput2 = "Zm8=";

static const std::string testInput3  = "foo";
static const std::string testOutput3 = "Zm9v";


CPPUNIT_TEST_SUITE_REGISTRATION( IOT_Base64Tester );


void IOT_Base64Tester::testEncode()
{
    std::string result = IOT_Base64::encode((uint8_t*)testInputNoPadding.c_str(), testInputNoPadding.length());
    CPPUNIT_ASSERT(result == testOutputNoPadding);

    result = IOT_Base64::encode((uint8_t*)testInput1Pad.c_str(), testInput1Pad.length());
    CPPUNIT_ASSERT(result == testOutput1Pad);

    result = IOT_Base64::encode((uint8_t*)testInput2Pad.c_str(), testInput2Pad.length());
    CPPUNIT_ASSERT(result == testOutput2Pad);
}

void IOT_Base64Tester::testEncodeShort()
{
    std::string result = IOT_Base64::encode((uint8_t*)testInput1.c_str(), testInput1.length());
    CPPUNIT_ASSERT(result == testOutput1);

    result = IOT_Base64::encode((uint8_t*)testInput2.c_str(), testInput2.length());
    CPPUNIT_ASSERT(result == testOutput2);

    result = IOT_Base64::encode((uint8_t*)testInput3.c_str(), testInput3.length());
    CPPUNIT_ASSERT(result == testOutput3);
}

void IOT_Base64Tester::testEncodeEmpty()
{
    std::string empty = "";
    std::string result = IOT_Base64::encode((uint8_t*)empty.c_str(), empty.length());
    CPPUNIT_ASSERT(result == "");
}

void IOT_Base64Tester::testDecode()
{
    std::vector<uint8_t> decoded;
    CPPUNIT_ASSERT(IOT_Base64::decode(testOutputNoPadding, decoded));
    CPPUNIT_ASSERT(CompareDecoded(testInputNoPadding, decoded));

    CPPUNIT_ASSERT(IOT_Base64::decode(testOutput1Pad, decoded));
    CPPUNIT_ASSERT(CompareDecoded(testInput1Pad, decoded));

    CPPUNIT_ASSERT(IOT_Base64::decode(testOutput2Pad, decoded));
    CPPUNIT_ASSERT(CompareDecoded(testInput2Pad, decoded));
}

void IOT_Base64Tester::testDecodeShort()
{
    std::vector<uint8_t> decoded;
    CPPUNIT_ASSERT(IOT_Base64::decode(testOutput1, decoded));
    CPPUNIT_ASSERT(CompareDecoded(testInput1, decoded));

    CPPUNIT_ASSERT(IOT_Base64::decode(testOutput2, decoded));
    CPPUNIT_ASSERT(CompareDecoded(testInput2, decoded));

    CPPUNIT_ASSERT(IOT_Base64::decode(testOutput2, decoded));
    CPPUNIT_ASSERT(CompareDecoded(testInput2, decoded));
}

void IOT_Base64Tester::testDecodeEmpty()
{
    std::vector<uint8_t> decoded;
    CPPUNIT_ASSERT(IOT_Base64::decode("", decoded));
    CPPUNIT_ASSERT(decoded.size() == 0);
}

bool IOT_Base64Tester::CompareDecoded(const std::string& original, const std::vector<uint8_t>& decoded) const
{
    if(original.size() != decoded.size())
        return false;

    for(size_t i=0; i<original.size(); ++i) {
        if(static_cast<uint8_t>(original.at(i)) != decoded.at(i))
            return false;
    }

    return true;
}


