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
 * @brief Scale implementation of TransactionReceipt
 * @file ScaleTransactionReceipt.cpp
 * @author: yujiechen
 * @date: 2021-03-18
 */
#include "ScaleTransactionReceipt.h"
#include <bcos-framework/libcodec/scale/Scale.h>

using namespace bcos;
using namespace bcos::protocol;
using namespace bcos::crypto;
using namespace bcos::codec::scale;

ScaleTransactionReceipt::ScaleTransactionReceipt(
    bcos::crypto::CryptoSuite::Ptr _cryptoSuite, bytes const& _receiptData)
  : m_cryptoSuite(_cryptoSuite)
{
    decode(_receiptData);
}

ScaleTransactionReceipt::ScaleTransactionReceipt(bcos::crypto::CryptoSuite::Ptr _cryptoSuite,
    int32_t _version, h256 const& _stateRoot, u256 const& _gasUsed, Address const& _contractAddress,
    LogEntriesPtr _logEntries, TransactionStatus _status, bytes const& _output)
  : m_cryptoSuite(_cryptoSuite),
    m_version(_version),
    m_stateRoot(_stateRoot),
    m_gasUsed(_gasUsed),
    m_contractAddress(_contractAddress),
    m_logEntries(_logEntries),
    m_status((int32_t)_status),
    m_output(_output),
    m_bloom(generateBloom(_logEntries, _cryptoSuite))
{}

void ScaleTransactionReceipt::decode(bytes const& _data)
{
    m_receiptDataCache.assign(_data.data(), _data.data() + _data.size());
    ScaleDecoderStream stream(_data);
    stream >> m_version >> m_status >> m_output >> m_contractAddress >> m_stateRoot >> m_gasUsed >>
        m_bloom >> m_logEntries;
}
void ScaleTransactionReceipt::encode(bytes& _encodeReceiptData)
{
    ScaleEncoderStream stream;
    stream << m_version << m_status << m_output << m_contractAddress << m_stateRoot
           << (CompactInteger)m_gasUsed << m_bloom << m_logEntries;
    _encodeReceiptData = stream.data();
}

h256 const& ScaleTransactionReceipt::hash()
{
    UpgradableGuard l(x_hash);
    if (m_hash != h256())
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