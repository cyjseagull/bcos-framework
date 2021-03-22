/*
 *  Copyright (C) 2021 FISCO BCOS.
 *  SPDX-License-Identifier: Apache-2.0
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @brief test for Transaction
 * @file PBTransactionTest.h
 * @author: yujiechen
 * @date: 2021-03-16
 */
#include <bcos-crypto/hash/Keccak256.h>
#include <bcos-crypto/hash/SM3.h>
#include <bcos-framework/libprotocol/protobuf/PBTransactionReceipt.h>
#include <bcos-framework/libprotocol/protobuf/ScaleTransactionReceipt.h>
#include <bcos-framework/libutilities/Common.h>
#include <bcos-test/libutils/TestPromptFixture.h>
#include <boost/test/unit_test.hpp>

using namespace bcos;
using namespace bcos::protocol;
using namespace bcos::crypto;

namespace bcos
{
namespace test
{
BOOST_FIXTURE_TEST_SUITE(ScaleTransationReceiptTest, TestPromptFixture)
LogEntriesPtr fakeLogEntries(Hash::Ptr _hashImpl, size_t _size)
{
    LogEntriesPtr logEntries = std::make_shared<LogEntries>();
    for (size_t i = 0; i < _size; i++)
    {
        auto topic = _hashImpl->hash(std::to_string(i));
        h256s topics;
        topics.push_back(topic);
        auto address = right160(topic);
        bytes output = topic.asBytes();
        auto logEntry = std::make_shared<LogEntry>(address, topics, output);
        logEntries->push_back(logEntry);
    }
    return logEntries;
}
BOOST_AUTO_TEST_CASE(testScaleTransactionReceipt)
{
    auto hashImpl = std::make_shared<Keccak256>();
    auto cryptoSuite = std::make_shared<CryptoSuite>(hashImpl, nullptr, nullptr);
    int32_t version = 1;
    auto stateRoot = hashImpl->hash((std::string) "stateRoot");
    u256 gasUsed = 12343242342;
    auto contractAddress = toAddress("5fe3c4c3e2079879a0dba1937aca95ac16e68f0f");
    auto logEntries = fakeLogEntries(hashImpl, 2);
    TransactionStatus status = TransactionStatus::BadJumpDestination;
    bytes output = contractAddress.asBytes();
    for (int i = 0; i < 1000; i++)
    {
        output += contractAddress.asBytes();
    }
    auto receipt = std::make_shared<ScaleTransactionReceipt>(
        cryptoSuite, version, stateRoot, gasUsed, contractAddress, logEntries, status, output);
    // encode
    std::shared_ptr<bytes> encodedData = std::make_shared<bytes>();
    auto start = utcTime();
    for (size_t i = 0; i < 20000; i++)
    {
        receipt->encode(*encodedData);
    }
    std::cout << "##### ScaleReceipt encodeT: " << (utcTime() - start)
              << ", encodedData size:" << encodedData->size() << std::endl;

    // decode
    std::shared_ptr<ScaleTransactionReceipt> decodedReceipt;
    start = utcTime();
    for (size_t i = 0; i < 20000; i++)
    {
        decodedReceipt = std::make_shared<ScaleTransactionReceipt>(cryptoSuite, *encodedData);
    }
    std::cout << "##### ScaleReceipt decodeT: " << (utcTime() - start) << std::endl;

    // check the decodedReceipt
    BOOST_CHECK(decodedReceipt->version() == receipt->version());
    BOOST_CHECK(decodedReceipt->stateRoot() == receipt->stateRoot());
    BOOST_CHECK(decodedReceipt->gasUsed() == receipt->gasUsed());
    BOOST_CHECK(decodedReceipt->contractAddress() == receipt->contractAddress());
    BOOST_CHECK(decodedReceipt->status() == receipt->status());
    BOOST_CHECK(decodedReceipt->output().toBytes() == receipt->output().toBytes());
    BOOST_CHECK(decodedReceipt->hash() == receipt->hash());
    // check LogEntries
    BOOST_CHECK(decodedReceipt->logEntries()->size() == 2);
    BOOST_CHECK(decodedReceipt->logEntries()->size() == receipt->logEntries()->size());
    auto logEntry = (*(decodedReceipt->logEntries()))[1];
    auto expectedTopic = hashImpl->hash(std::to_string(1));
    BOOST_CHECK(logEntry->topics()[0] == expectedTopic);
    BOOST_CHECK(logEntry->address() == right160(expectedTopic));
    BOOST_CHECK(logEntry->data() == expectedTopic.asBytes());
}

BOOST_AUTO_TEST_CASE(testPBTransactionReceipt)
{
    auto hashImpl = std::make_shared<Keccak256>();
    auto cryptoSuite = std::make_shared<CryptoSuite>(hashImpl, nullptr, nullptr);
    int32_t version = 1;
    auto stateRoot = hashImpl->hash((std::string) "stateRoot");
    u256 gasUsed = 12343242342;
    auto contractAddress = toAddress("5fe3c4c3e2079879a0dba1937aca95ac16e68f0f");
    auto logEntries = fakeLogEntries(hashImpl, 2);
    TransactionStatus status = TransactionStatus::BadJumpDestination;
    bytes output = contractAddress.asBytes();
    for (int i = 0; i < 1000; i++)
    {
        output += contractAddress.asBytes();
    }
    auto receipt = std::make_shared<PBTransactionReceipt>(
        cryptoSuite, version, stateRoot, gasUsed, contractAddress, logEntries, status, output);
    // encode
    std::shared_ptr<bytes> encodedData = std::make_shared<bytes>();
    auto start = utcTime();
    for (size_t i = 0; i < 20000; i++)
    {
        receipt->encode(*encodedData);
    }
    std::cout << "##### PBReceipt encodeT: " << (utcTime() - start)
              << ", encodedData size:" << encodedData->size() << std::endl;

    // decode
    std::shared_ptr<PBTransactionReceipt> decodedReceipt;
    start = utcTime();
    for (size_t i = 0; i < 20000; i++)
    {
        decodedReceipt = std::make_shared<PBTransactionReceipt>(cryptoSuite, *encodedData);
    }
    std::cout << "##### PBReceipt decodeT: " << (utcTime() - start) << std::endl;

    // check the decodedReceipt
    BOOST_CHECK(decodedReceipt->version() == receipt->version());
    BOOST_CHECK(decodedReceipt->stateRoot() == receipt->stateRoot());
    BOOST_CHECK(decodedReceipt->gasUsed() == receipt->gasUsed());
    BOOST_CHECK(decodedReceipt->contractAddress() == receipt->contractAddress());
    BOOST_CHECK(decodedReceipt->status() == receipt->status());
    BOOST_CHECK(decodedReceipt->output().toBytes() == receipt->output().toBytes());
    BOOST_CHECK(decodedReceipt->hash() == receipt->hash());
    std::cout << "###output size:" << decodedReceipt->output().size() << std::endl;
    // check LogEntries
    BOOST_CHECK(decodedReceipt->logEntries()->size() == 2);
    BOOST_CHECK(decodedReceipt->logEntries()->size() == receipt->logEntries()->size());
    auto logEntry = (*(decodedReceipt->logEntries()))[1];
    auto expectedTopic = hashImpl->hash(std::to_string(1));
    BOOST_CHECK(logEntry->topics()[0] == expectedTopic);
    BOOST_CHECK(logEntry->address() == right160(expectedTopic));
    BOOST_CHECK(logEntry->data() == expectedTopic.asBytes());
}
BOOST_AUTO_TEST_SUITE_END()
}  // namespace test
}  // namespace bcos