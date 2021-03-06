// Copyright (c) 2012-2018 The Elastos Open Source Project
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef __ELASTOS_SDK_IDCHAINSUBWALLET_H__
#define __ELASTOS_SDK_IDCHAINSUBWALLET_H__

#include "Interface/IIdChainSubWallet.h"
#include "SidechainSubWallet.h"

namespace Elastos {
	namespace ElaWallet {

		class IdChainSubWallet : public SidechainSubWallet, public IIdChainSubWallet {
		public:
			virtual ~IdChainSubWallet();

			virtual nlohmann::json CreateIdTransaction(
					const std::string &fromAddress,
					const std::string &toAddress,
					const uint64_t amount,
					const nlohmann::json &payloadJson,
					const nlohmann::json &programJson,
					uint64_t fee,
					const std::string &memo,
					const std::string &remark);

		protected:
			friend class MasterWallet;

			IdChainSubWallet(const CoinInfo &info,
							 const ChainParams &chainParams,
							 const std::string &payPassword,
							 const PluginTypes &pluginTypes,
							 MasterWallet *parent);

			virtual boost::shared_ptr<Transaction> createTransaction(TxParam *param) const;

			virtual void verifyRawTransaction(const TransactionPtr &transaction);

			virtual TransactionPtr completeTransaction(const TransactionPtr &transaction, uint64_t actualFee);

			virtual void onTxAdded(const TransactionPtr &transaction);

			virtual void onTxUpdated(const std::string &hash, uint32_t blockHeight, uint32_t timeStamp);

			virtual void onTxDeleted(const std::string &hash, bool notifyUser, bool recommendRescan);

		};

	}
}

#endif //__ELASTOS_SDK_IDCHAINSUBWALLET_H__
