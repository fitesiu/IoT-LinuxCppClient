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

#ifndef IOT_BASE64TESTER_H
#define IOT_BASE64TESTER_H

#include "cppunit/extensions/HelperMacros.h"
#include <vector>

class IOT_Base64Tester : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( IOT_Base64Tester );
    CPPUNIT_TEST( testEncode );
    CPPUNIT_TEST( testEncodeShort );
    CPPUNIT_TEST( testEncodeEmpty );
    CPPUNIT_TEST( testDecode );
    CPPUNIT_TEST( testDecodeShort );
    CPPUNIT_TEST( testDecodeEmpty );
    CPPUNIT_TEST_SUITE_END();

    public:
        void testEncode();
        void testEncodeShort();
        void testEncodeEmpty();

        void testDecode();
        void testDecodeShort();
        void testDecodeEmpty();

private:
        bool CompareDecoded(const std::string& base64, const std::vector<uint8_t>& decoded) const;
};


#endif // IOT_BASE64TESTER_H
