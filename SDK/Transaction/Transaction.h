// Copyright (c) 2012-2018 The Elastos Open Source Project
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef __ELASTOS_SDK_TRANSACTION_H__
#define __ELASTOS_SDK_TRANSACTION_H__

#include <boost/shared_ptr.hpp>

#include "Wrapper.h"
#include "CMemBlock.h"
#include "SharedWrapperList.h"
#include "TransactionOutput.h"
#include "Key.h"
#include "WrapperList.h"
#include "Program.h"
#include "ELATransaction.h"
#include "SDK/Plugin/Interface/ELAMessageSerializable.h"
#include "ELACoreExt/Attribute.h"
#include "ELACoreExt/Payload/IPayload.h"
#include "ELACoreExt/ELATransaction.h"


namespace Elastos {
	namespace ElaWallet {

		class Wallet;

		class Transaction :
				public Wrapper<BRTransaction>,
				public ELAMessageSerializable {

		public:
			Transaction();

			Transaction(ELATransaction *transaction, bool manageRaw = true);

			Transaction(const ELATransaction &tx);

			Transaction(const Transaction &tx);

			Transaction &operator=(const Transaction &tx);

			Transaction(const CMBlock &buffer);

			Transaction(const CMBlock &buffer, uint32_t blockHeight, uint32_t timeStamp);

			~Transaction();

			virtual std::string toString() const;

			virtual BRTransaction *getRaw() const;

			virtual void Serialize(ByteStream &ostream) const;

			virtual bool Deserialize(ByteStream &istream);

			uint64_t calculateFee(uint64_t feePerKb);

			uint64_t getTxFee(const boost::shared_ptr<Wallet> &wallet);

			bool isRegistered() const;

			bool &isRegistered();

			UInt256 getHash() const;

			void resetHash();

			uint32_t getVersion() const;

			std::vector<std::string> getInputAddresses();

			const std::vector<TransactionOutput *> &getOutputs() const;

			std::vector<std::string> getOutputAddresses();

			void setTransactionType(ELATransaction::Type type);

			ELATransaction::Type getTransactionType() const;

			/**
			 * The transaction's lockTime
			 *
			 * @return the lock time as a long (from a uint32_t)
			 */
			uint32_t getLockTime();

			void setLockTime(uint32_t lockTime);

			/**
			 * The transaction's blockHeight.
			 *
			 * @return the blockHeight as a long (from a uint32_t).
			 */
			uint32_t getBlockHeight();

			/**
			 * The transacdtion's timestamp.
			 *
			 * @return the timestamp as a long (from a uint32_t).
			 */
			uint32_t getTimestamp();

			void setTimestamp(uint32_t timestamp);

			void addOutput(TransactionOutput *output);

			void shuffleOutputs();

			/**
			 * The the transactions' size in bytes if signed, or the estimated size assuming
			 * compact pubkey sigs
		
			 * @return the size in bytes.
			 */
			size_t getSize();

			/**
			 * The transaction's standard fee which is the minimum transaction fee needed for the
			 * transaction to relay across the bitcoin network.
			 * *
			 * @return the fee (in Satoshis)?
			 */
			uint64_t getStandardFee();

			/**
			 * Returns true if all the transaction's signatures exists.  This method does not verify
			 * the signatures.
			 *
			 * @return true if all exist.
			 */
			bool isSigned();


			bool sign(const WrapperList<Key, BRKey> &keys, int forkId);

			bool sign(const Key &key, int forkId);

			/**
			 * Return true if this transaction satisfied the rules in:
			 *      https://bitcoin.org/en/developer-guide#standard-transactions
			 *
			 * @return true if standard; false otherwise
			 */
			bool isStandard();

			UInt256 getReverseHash();

			virtual nlohmann::json toJson() const;

			virtual void fromJson(const nlohmann::json &jsonData);

			static uint64_t getMinOutputAmount();

			const IPayload *getPayload() const;

			IPayload *getPayload();

			void addAttribute(Attribute *attribute);

			void addProgram(Program *program);

			const std::vector<Attribute *> &getAttributes() const;

			const std::vector<Program *> &getPrograms() const;

			const std::string getRemark() const;

			void setRemark(const std::string &remark);

			void generateExtraTransactionInfo(nlohmann::json &rawTxJson, const boost::shared_ptr<Wallet> &wallet, uint32_t blockHeight);

		private:
			IPayload *newPayload(ELATransaction::Type type);

			void serializeUnsigned(ByteStream &ostream) const;

			bool transactionSign(int forkId, const WrapperList<Key, BRKey> keys);

			std::string getConfirmInfo(uint32_t blockHeight);

			std::string getStatus(uint32_t blockHeight);

		private:
			bool _isRegistered;
			bool _manageRaw;
			ELATransaction *_transaction;
		};

		typedef boost::shared_ptr<Transaction> TransactionPtr;

	}
}

#endif //__ELASTOS_SDK_TRANSACTION_H__
