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


#include "IOT_RestClientTester.h"
#include "IOT_RestClient.h"
#include <algorithm>
#include <thread>


CPPUNIT_TEST_SUITE_REGISTRATION( IOT_RestClientTester );

static const std::string HTTP_GET_URL = "https://httpbin.org/get?test=testdata";
static const std::string HTTP_GET_RET = "\"url\": \"https://httpbin.org/get?test=testdata\"";

static const std::string HTTP_AUTH_URL = "https://httpbin.org/basic-auth/user/pass";
static const std::string HTTP_AUTH_RET = "{\"authenticated\":true,\"user\":\"user\"}";

static const std::string HTTP_POST_URL  = "https://httpbin.org/post";
static const std::string HTTP_POST_DATA = "This is test POST data";


void IOT_RestClientTester::testHttpGet()
{
    IOT_RestClient client;
    std::string response;

    CPPUNIT_ASSERT(client.GetResource(HTTP_GET_URL, "", "", response) == IOTAPI::IOT_ERR_OK);
    CPPUNIT_ASSERT(response.find(HTTP_GET_RET) != std::string::npos);
}

void IOT_RestClientTester::testOversizedResponse()
{
    IOT_RestClient client;
    std::string response;

    client.SetMaxResponseSize(100);
    CPPUNIT_ASSERT(client.GetResource(HTTP_GET_URL, "", "", response) != IOTAPI::IOT_ERR_OK);
}

void IOT_RestClientTester::testHttpBasicAuth()
{
    IOT_RestClient client;
    std::string response;

    CPPUNIT_ASSERT(client.GetResource(HTTP_AUTH_URL, "user", "pass", response) == IOTAPI::IOT_ERR_OK);
    response.erase( std::remove_if(response.begin(), response.end(), isspace), response.end() );

    CPPUNIT_ASSERT(response == HTTP_AUTH_RET);
}

void IOT_RestClientTester::testHttpAuthFail()
{
    IOT_RestClient client;
    std::string response;

    CPPUNIT_ASSERT(client.GetResource(HTTP_AUTH_URL, "user", "wrong pass", response) != IOTAPI::IOT_ERR_OK);
}

void IOT_RestClientTester::testHttpPost()
{
    IOT_RestClient client;
    std::string response;

    CPPUNIT_ASSERT(client.PostAndReadResponse(HTTP_POST_URL, "", "", HTTP_POST_DATA, response)  == IOTAPI::IOT_ERR_OK);
    CPPUNIT_ASSERT(response.find(HTTP_POST_DATA) != std::string::npos);
}

void IOT_RestClientTester::testMultiThread()
{
    auto func = [](){
        IOT_RestClient client;
        for(size_t i=0; i<30; ++i) {
            std::string response;
            CPPUNIT_ASSERT(client.GetResource(HTTP_GET_URL, "", "", response)  == IOTAPI::IOT_ERR_OK);
            CPPUNIT_ASSERT(response.find(HTTP_GET_RET) != std::string::npos);
        }
    };

    std::thread t1(func);
    std::thread t2(func);
    std::thread t3(func);

    t1.join();
    t2.join();
    t3.join();
}

