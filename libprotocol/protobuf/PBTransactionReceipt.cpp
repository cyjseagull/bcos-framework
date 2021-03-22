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
 * @brief PBImplementation of transaction receipt
 * @file PBTransactionReceipt.cpp
 * @author: yujiechen
 * @date: 2021-03-18
 */
#include "PBTransactionReceipt.h"

using namespace bcos;
using namespace bcos::crypto;
using namespace bcos::protocol;
PBTransactionReceipt::PBTransactionReceipt(
    bcos::crypto::CryptoSuite::Ptr _cryptoSuite, bytes const& _receiptData)
  : PBTransactionReceipt(_cryptoSuite)
{
    decode(_receiptData);
}

PBTransactionReceipt::PBTransactionReceipt(bcos::crypto::CryptoSuite::Ptr _cryptoSuite,
    int32_t _version, h256 const& _stateRoot, u256 const& _gasUsed, Address const& _contractAddress,
    LogEntriesPtr _logEntries, TransactionStatus _status, bytes const& _output)
  : PBTransactionReceipt(_cryptoSuite)
{
    m_stateRoot = _stateRoot;
    m_gasUsed = _gasUsed;
    m_contractAddress = _contractAddress;
    m_logEntries = _logEntries;
    m_bloom = generateBloom(m_logEntries, _cryptoSuite);
    // set version
    m_receipt->set_version(_version);
    // set status
    m_receipt->set_status((int32_t)_status);
    // set output
    m_receipt->set_ouptput(_output.data(), _output.size());
}

void PBTransactionReceipt::decode(bytes const& _receiptData)
{
    m_receiptDataCache.assign(_receiptData.data(), _receiptData.data() + _receiptData.size());
    // decode receipt
    if (!m_receipt->ParseFromArray(_receiptData.data(), _receiptData.size()))
    {
        BOOST_THROW_EXCEPTION(
            ReceiptDecodeException() << errinfo_comment(
                "decode transaction receipt failed, receiptData:" + *toHexString(_receiptData)));
    }
    m_stateRoot =
        h256((byte const*)m_receipt->stateroot().data(), h256::ConstructorType::FromPointer);
    m_gasUsed = fromBigEndian<u256>(
        bytesConstRef((const byte*)m_receipt->gasused().data(), m_receipt->gasused().size()));
    m_contractAddress = Address((byte const*)m_receipt->mutable_contractaddress()->data(),
        Address::ConstructorType::FromPointer);
    m_bloom = LogBloom(
        (byte const*)m_receipt->mutable_bloom()->data(), LogBloom::ConstructorType::FromPointer);
    // decode log entries
    decodeLogEntries();
}

void PBTransactionReceipt::decodeLogEntries()
{
    auto logEntriesSize = m_receipt->logentries_size();
    m_logEntries->clear();
    for (int index = 0; index < logEntriesSize; index++)
    {
        m_logEntries->emplace_back(createLogEntry(m_receipt->logentries(index)));
    }
}

LogEntry::Ptr PBTransactionReceipt::createLogEntry(PBRawLogEntry const& _logEntry)
{
    auto address =
        Address((byte const*)_logEntry.address().data(), Address::ConstructorType::FromPointer);
    auto data = bytesConstRef((byte const*)_logEntry.data().data(), _logEntry.data().size());
    h256s topics;
    // decode topics
    for (int i = 0; i < _logEntry.topics_size(); i++)
    {
        topics.emplace_back(
            h256((byte const*)_logEntry.topics(i).data(), h256::ConstructorType::FromPointer));
    }
    return std::make_shared<LogEntry>(address, topics, data.toBytes());
}

void PBTransactionReceipt::encode(bytes& _encodeReceiptData)
{
    if (m_receiptDataCache.size() != 0)
    {
        _encodeReceiptData = m_receiptDataCache;
    }
    // set stateRoot
    m_receipt->set_stateroot(m_stateRoot.data(), h256::size);
    // set gasUsed
    auto gasUsedBytes = toBigEndian(m_gasUsed);
    m_receipt->set_gasused(gasUsedBytes.data(), gasUsedBytes.size());
    // set contractAddress
    m_receipt->set_contractaddress(m_contractAddress.data(), Address::size);
    // set bloom
    m_receipt->set_bloom(m_bloom.data(), LogBloom::size);
    // set logEntries
    encodeLogEntries();
    // encode receiptHashField
    auto receiptDataLen = m_receipt->ByteSizeLong();
    _encodeReceiptData.resize(receiptDataLen);
    if (!m_receipt->SerializeToArray(_encodeReceiptData.data(), receiptDataLen))
    {
        BOOST_THROW_EXCEPTION(
            ReceiptEncodeException() << errinfo_comment("encode transactionReceipt failed"));
    }
}

void PBTransactionReceipt::encodeLogEntries()
{
    if ((size_t)m_receipt->logentries_size() < m_logEntries->size())
    {
        for (size_t i = m_receipt->logentries_size(); i < m_logEntries->size(); i++)
        {
            m_receipt->add_logentries();
        }
    }
    // set logEntries
    size_t index = 0;
    for (auto log : *m_logEntries)
    {
        auto logEntry = m_receipt->mutable_logentries(index++);
        // set address
        logEntry->set_address(log->address().data(), Address::size);
        // set topics
        for (size_t i = logEntry->topics_size(); i < log->topics().size(); i++)
        {
            logEntry->add_topics();
        }
        size_t topicIndex = 0;
        for (auto const& topic : log->topics())
        {
            logEntry->set_topics(topicIndex++, topic.data(), h256::size);
        }
        // set data
        logEntry->set_data(log->data().data(), log->data().size());
    }
}

h256 const& PBTransactionReceipt::hash()
{
    UpgradableGuard l(x_hash);
    if (h256() != m_hash)
    {
        return m_hash;
    }
    if (m_receiptDataCache.size() == 0)
    {
        encode(m_receiptDataCache);
    }
    UpgradeGuard ul(l);
    m_hash = m_cryptoSuite->hash(ref(m_receiptDataCache));
    return m_hash;
}

bytesConstRef PBTransactionReceipt::output() const
{
    return bytesConstRef(
        (byte const*)m_receipt->mutable_ouptput()->data(), m_receipt->mutable_ouptput()->size());
}