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
 * @file: TransactionWrapper.h
 * @author: yujiechen
 * @date 2021-03-01
 */
#pragma once
#include "generated/Transaction.pb.h"
#include <libutilities/Common.h>
#include <libutilities/FixedBytes.h>

namespace bcos
{
namespace protocol
{

class TransactionWrapper
{
public:
    using Ptr = std::shared_ptr<TransactionWrapper>;
    explicit TransactionWrapper(std::string const& _txData, bool _checkTransaction);
    TransactionWrapper(Transaction const&) = delete;

    h256 hash() const;

    void encode(std::shared_ptr<std::string> _txData);
    void decode(std::string const& _txData, bool _checkTransaction);
private:
    std::shared_ptr<Transaction> m_transaction;
};
}
}

