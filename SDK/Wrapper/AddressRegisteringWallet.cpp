// Copyright (c) 2012-2018 The Elastos Open Source Project
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <climits>
#include <algorithm>
#include "Core/BRArray.h"

#include "AddressRegisteringWallet.h"

namespace Elastos {
	namespace ElaWallet {

		AddressRegisteringWallet::AddressRegisteringWallet(
				const boost::shared_ptr<Listener> &listener,
				const std::vector<std::string> &initialAddrs) {
			_wallet = createRegisterAddress(initialAddrs);

			assert(listener != nullptr);
			_listener = boost::weak_ptr<Listener>(listener);

			BRWalletSetCallbacks((BRWallet *) _wallet, &_listener,
								 balanceChanged,
								 txAdded,
								 txUpdated,
								 txDeleted);
		}

		AddressRegisteringWallet::~AddressRegisteringWallet() {
			if (_wallet != nullptr) {
				ELAWalletFree(_wallet, false);
				_wallet = nullptr;
			}
		}

		void AddressRegisteringWallet::RegisterAddress(const std::string &address) {

			pthread_mutex_lock(&_wallet->Raw.lock);

			if (BRSetContains(_wallet->Raw.allAddrs, address.c_str()) == 0) {
				Address addr(address);
				size_t count = array_count(_wallet->Raw.externalChain);
				array_add(_wallet->Raw.externalChain, *addr.getRaw());
				BRSetAdd(_wallet->Raw.allAddrs, &_wallet->Raw.externalChain[count]);
			}
			pthread_mutex_unlock(&_wallet->Raw.lock);
		}

		ELAWallet *AddressRegisteringWallet::createRegisterAddress(const std::vector<std::string> &initialAddrs) {
			ELAWallet *wallet = nullptr;
			BRTransaction *tx;

			wallet = (ELAWallet *) calloc(1, sizeof(*wallet));
			assert(wallet != nullptr);
			memset(wallet, 0, sizeof(*wallet));
			array_new(wallet->Raw.utxos, 100);
			array_new(wallet->Raw.transactions, 100);
			wallet->Raw.feePerKb = DEFAULT_FEE_PER_KB;
#ifdef TEMPORARY_HD_STRATEGY
			wallet->Cache = nullptr;
#endif
			wallet->Raw.WalletUnusedAddrs = addressRegisteringWalletUnusedAddrs;
			wallet->Raw.WalletAllAddrs = addressRegisteringWalletAllAddrs;
			wallet->Raw.setApplyFreeTx = setApplyFreeTx;
			wallet->Raw.setApplyFreeTx = setApplyFreeTx;
			wallet->Raw.WalletUpdateBalance = Wallet::WalletUpdateBalance;
			wallet->Raw.WalletContainsTx = Wallet::WalletContainsTx;
			wallet->Raw.WalletAddUsedAddrs = Wallet::WalletAddUsedAddrs;
			wallet->Raw.WalletCreateTxForOutputs = Wallet::WalletCreateTxForOutputs;
			wallet->Raw.WalletMaxOutputAmount = Wallet::WalletMaxOutputAmount;
			wallet->Raw.WalletFeeForTx = Wallet::WalletFeeForTx;
			wallet->Raw.TransactionIsSigned = Wallet::TransactionIsSigned;
			wallet->Raw.internalChain = nullptr;
			array_new(wallet->Raw.externalChain, 100);
			array_new(wallet->Raw.balanceHist, 100);
			wallet->Raw.allTx = BRSetNew(BRTransactionHash, BRTransactionEq, 100);
			wallet->Raw.invalidTx = BRSetNew(BRTransactionHash, BRTransactionEq, 10);
			wallet->Raw.pendingTx = BRSetNew(BRTransactionHash, BRTransactionEq, 10);
			wallet->Raw.spentOutputs = BRSetNew(BRUTXOHash, BRUTXOEq, 100);
			wallet->Raw.usedAddrs = BRSetNew(BRAddressHash, BRAddressEq, 100);
			wallet->Raw.allAddrs = BRSetNew(BRAddressHash, BRAddressEq, 100);
			pthread_mutex_init(&wallet->Raw.lock, nullptr);

			wallet->Raw.WalletUnusedAddrs((BRWallet *) wallet, nullptr, SEQUENCE_GAP_LIMIT_EXTERNAL, 0);
			wallet->Raw.WalletUnusedAddrs((BRWallet *) wallet, nullptr, SEQUENCE_GAP_LIMIT_INTERNAL, 1);

			std::vector<std::string> uniqueAddress = initialAddrs;
			std::sort(uniqueAddress.begin(), uniqueAddress.end());
			uniqueAddress.erase(std::unique(uniqueAddress.begin(), uniqueAddress.end()), uniqueAddress.end());
			for (size_t i = 0; i < uniqueAddress.size(); ++i) {
				Address addr(uniqueAddress[i]);
				array_add(wallet->Raw.externalChain, *addr.getRaw());
				BRSetAdd(wallet->Raw.allAddrs, &wallet->Raw.externalChain[i]);
			}

			return wallet;
		}

		size_t AddressRegisteringWallet::addressRegisteringWalletAllAddrs(BRWallet *wallet, BRAddress addrs[],
																		  size_t addrsCount) {
			pthread_mutex_unlock(&wallet->lock);
			size_t externalCount = array_count(wallet->externalChain);
			if (addrs && externalCount > 0) {
				size_t realCount = std::min(externalCount, addrsCount);
				for (int i = 0; i < realCount; ++i) {
					addrs[i] = wallet->externalChain[i];
				}
			}
			pthread_mutex_unlock(&wallet->lock);

			return externalCount;
		}

		size_t AddressRegisteringWallet::addressRegisteringWalletUnusedAddrs(BRWallet *wallet, BRAddress addrs[],
																			 uint32_t gapLimit, int internal) {
			pthread_mutex_unlock(&wallet->lock);
			size_t count = array_count(wallet->externalChain);
			if (addrs && count > 0) {
				addrs[0] = wallet->externalChain[0];
			}
			pthread_mutex_unlock(&wallet->lock);

			return 1;
		}
	}
}