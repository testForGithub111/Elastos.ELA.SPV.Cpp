/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_elastos_spvcore_CorePaymentProtocolACK */

#ifndef _Included_com_elastos_spvcore_CorePaymentProtocolACK
#define _Included_com_elastos_spvcore_CorePaymentProtocolACK
#ifdef __cplusplus
extern "C" {
#endif
	/*
	 * Class:     com_elastos_spvcore_CorePaymentProtocolACK
	 * Method:    getCustomerMemo
	 * Signature: ()Ljava/lang/String;
	 */
	JNIEXPORT jstring JNICALL Java_com_elastos_spvcore_CorePaymentProtocolACK_getCustomerMemo
		(JNIEnv *, jobject);

	/*
	 * Class:     com_elastos_spvcore_CorePaymentProtocolACK
	 * Method:    getMerchantData
	 * Signature: ()[B
	 */
	JNIEXPORT jbyteArray JNICALL Java_com_elastos_spvcore_CorePaymentProtocolACK_getMerchantData
		(JNIEnv *, jobject);

	/*
	 * Class:     com_elastos_spvcore_CorePaymentProtocolACK
	 * Method:    getTransactions
	 * Signature: ()[Lcom/breadwallet/core/BRCoreTransaction;
	 */
	JNIEXPORT jobjectArray JNICALL Java_com_elastos_spvcore_CorePaymentProtocolACK_getTransactions
		(JNIEnv *, jobject);

	/*
	 * Class:     com_elastos_spvcore_CorePaymentProtocolACK
	 * Method:    getRefundTo
	 * Signature: ()[Lcom/breadwallet/core/BRCoreTransactionOutput;
	 */
	JNIEXPORT jobjectArray JNICALL Java_com_elastos_spvcore_CorePaymentProtocolACK_getRefundTo
		(JNIEnv *, jobject);

	/*
	 * Class:     com_elastos_spvcore_CorePaymentProtocolACK
	 * Method:    getMerchantMemo
	 * Signature: ()Ljava/lang/String;
	 */
	JNIEXPORT jstring JNICALL Java_com_elastos_spvcore_CorePaymentProtocolACK_getMerchantMemo
		(JNIEnv *, jobject);

	/*
	 * Class:     com_elastos_spvcore_CorePaymentProtocolACK
	 * Method:    createPaymentProtocolACK
	 * Signature: ([B)J
	 */
	JNIEXPORT jlong JNICALL Java_com_elastos_spvcore_CorePaymentProtocolACK_createPaymentProtocolACK
		(JNIEnv *, jclass, jbyteArray);

	/*
	 * Class:     com_elastos_spvcore_CorePaymentProtocolACK
	 * Method:    serialize
	 * Signature: ()[B
	 */
	JNIEXPORT jbyteArray JNICALL Java_com_elastos_spvcore_CorePaymentProtocolACK_serialize
		(JNIEnv *, jobject);

	/*
	 * Class:     com_elastos_spvcore_CorePaymentProtocolACK
	 * Method:    disposeNative
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_com_elastos_spvcore_CorePaymentProtocolACK_disposeNative
		(JNIEnv *, jobject);

	/*
	 * Class:     com_elastos_spvcore_CorePaymentProtocolACK
	 * Method:    initializeNative
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_com_elastos_spvcore_CorePaymentProtocolACK_initializeNative
		(JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif
#endif
