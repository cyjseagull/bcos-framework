/**
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
 * @brief: Wrapper for the transaction
 *
 * @file: TransactionWrapper.cpp
 * @author: yujiechen
 * @date 2021-03-01
 */
#include "TransactionWrapper.h"

using namespace bcos::protocol;
using namespace bcos;

TransactionWrapper::TransactionWrapper(std::string const& _txData, bool _checkTransaction):m_transaction(std::make_shared<Transaction>())
{
    decode(_txData, _checkTransaction);
}

h256 TransactionWrapper::hash() const
{
    return h256();
}

void TransactionWrapper::encode(std::shared_ptr<std::string> _txData)
{
    m_transaction->SerializeToString(_txData.get());
}

void TransactionWrapper::decode(std::string const& _txData, bool _checkTransaction)
{
    m_transaction->ParseFromString(_txData);
}