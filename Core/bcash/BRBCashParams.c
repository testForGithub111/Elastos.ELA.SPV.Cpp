#include <stdlib.h>
#include <string.h>
#include "BRBCashParams.h"
#include "BRInt.h"
#include "BRPeer.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static const char *BRBCashDNSSeeds[] = {
	"seed-abc.breadwallet.com.", "seed.bitcoinabc.org.", "seed-abc.bitcoinforks.org.", "seed.bitcoinunlimited.info.",
	"seed.bitprim.org.", "seed.deadalnix.me.", NULL
};

static const char *BRBCashTestNetDNSSeeds[] = {
	"testnet-seed.bitcoinabc.org", "testnet-seed-abc.bitcoinforks.org", "testnet-seed.bitprim.org",
	"testnet-seed.deadalnix.me", "testnet-seeder.criptolayer.net", NULL
};

static const BRMerkleBlock *_medianBlock(const BRMerkleBlock *b0, const BRSet *blockSet)
{
	const BRMerkleBlock *b, *b1 = NULL, *b2 = NULL;

	b1 = (b0) ? (BRMerkleBlock *)BRSetGet(blockSet, &b0->prevBlock) : NULL;
	b2 = (b1) ? (BRMerkleBlock *)BRSetGet(blockSet, &b1->prevBlock) : NULL;
	if (b0 && b2 && b0->timestamp > b2->timestamp) b = b0, b0 = b2, b2 = b;
	if (b0 && b1 && b0->timestamp > b1->timestamp) b = b0, b0 = b1, b1 = b;
	if (b1 && b2 && b1->timestamp > b2->timestamp) b = b1, b1 = b2, b2 = b;
	return (b0 && b1 && b2) ? b1 : NULL;
}

static int BRBCashVerifyDifficulty(const BRMerkleBlock *block, const BRSet *blockSet)
{
	int size, i, r = 1;
	const BRMerkleBlock *b, *first, *last;
	uint64_t target = 0, work = 0;
	uint32_t time;

	assert(block != NULL);
	assert(blockSet != NULL);

	if (block->height >= 504032) { // D601 hard fork height: https://reviews.bitcoinabc.org/D601
		first = (block) ? (const BRMerkleBlock *)BRSetGet(blockSet, &block->prevBlock) : NULL;
		first = _medianBlock(first, blockSet);

		for (i = 0, last = block; last && i < 144; i++) {
			last = (const BRMerkleBlock *)BRSetGet(blockSet, &last->prevBlock);
		}

		last = _medianBlock(last, blockSet);
		time = (first && last) ? first->timestamp - last->timestamp : 0;
		if (time > 288*10*60) time = 288*10*60;
		if (time < 72*10*60) time = 72*10*60;

		for (b = first; b && b != last;) {
			b = (const BRMerkleBlock *)BRSetGet(blockSet, &b->prevBlock);

			// work += 2^256/(target + 1)
			size = (b) ? b->target >> 24 : 0;
			target = (b) ? b->target & 0x007fffff : 0;
			work += (1ULL << (32 - size)*8) - (target + 1);
		}

		// work = work*10*60/time
		work = work*10*60/time;

		// target = (2^256/work) - 1
		size = 0;
		while (((work + 1) >> size*8) != 0) size++;
		target = (1ULL << (32 - size)*8) - (work + 1);
		target |= ((32 - size) << 24);

		if (target > 0x1d00ffff) target = 0x1d00ffff; // max proof-of-work
		if (first && last && block->target != target) r = 1;
	}

	return r;
}

static int BRBCashTestNetVerifyDifficulty(const BRMerkleBlock *block, const BRSet *blockSet)
{
	return 1; // XXX skip testnet difficulty check for now
}

// blockchain checkpoints - these are also used as starting points for partial chain downloads, so they must be at
// difficulty transition boundaries in order to verify the block difficulty at the immediately following transition
const BRCheckPoint BRBCashMainNetCheckpoints[] = {
	{      0, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0xd6, 0x68, 0x9c, 0x08, 0x5a, 0xe1, 0x65, 0x83, 0x1e, 0x93,
		0x4f, 0xf7, 0x63, 0xae, 0x46, 0xa2, 0xa6, 0xc1, 0x72, 0xb3, 0xf1, 0xb6, 0x0a, 0x8c, 0xe2, 0x6f }
	}, 1231006505, 0x1d00ffff },
	{  20160, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x0f, 0x1a, 0xef, 0x56, 0x19, 0x0a, 0xee, 0x63, 0xd3, 0x3a, 0x37, 0x3e,
		0x64, 0x87, 0x13, 0x2d, 0x52, 0x2f, 0xf4, 0xcd, 0x98, 0xcc, 0xfc, 0x96, 0x56, 0x6d, 0x46, 0x1e }
	}, 1248481816, 0x1d00ffff },
	{  40320, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x45, 0x86, 0x1e, 0x16, 0x9b, 0x5a, 0x96, 0x1b, 0x70, 0x34, 0xf8, 0xde,
		0x9e, 0x98, 0x02, 0x2e, 0x7a, 0x39, 0x10, 0x0d, 0xde, 0x3a, 0xe3, 0xea, 0x24, 0x0d, 0x72, 0x45 }
	}, 1266191579, 0x1c654657 },
	{  60480, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x06, 0x32, 0xe2, 0x2c, 0xe7, 0x3e, 0xd3, 0x8f, 0x46, 0xd5, 0xb4, 0x08,
		0xff, 0x1c, 0xff, 0x2c, 0xc9, 0xe1, 0x0d, 0xaa, 0xf4, 0x37, 0xdf, 0xd6, 0x55, 0x15, 0x38, 0x37 }
	}, 1276298786, 0x1c0eba64 },
	{  80640, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x7c, 0x80, 0xb8, 0x7e, 0xdf, 0x9f, 0x6a, 0x06, 0x97, 0xe2,
		0xf0, 0x1d, 0xb6, 0x7e, 0x51, 0x8c, 0x8a, 0x4d, 0x60, 0x65, 0xd1, 0xd8, 0x59, 0xa3, 0xa6, 0x59 }
	}, 1284861847, 0x1b4766ed },
	{ 100800, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe3, 0x83, 0xd4, 0x3c, 0xc4, 0x71, 0xc6, 0x4a, 0x9a, 0x4a,
		0x46, 0x79, 0x40, 0x26, 0x98, 0x9e, 0xf4, 0xff, 0x96, 0x11, 0xd5, 0xac, 0xb7, 0x04, 0xe4, 0x7a }
	}, 1294031411, 0x1b0404cb },
	{ 120960, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x92, 0x0c, 0xf7, 0xe4, 0x40, 0x6b, 0x96, 0x9a, 0xe9,
		0xc8, 0x07, 0xb5, 0xc4, 0xf2, 0x71, 0xf4, 0x90, 0xca, 0x3d, 0xe1, 0xb0, 0x77, 0x08, 0x36, 0xfc }
	}, 1304131980, 0x1b0098fa },
	{ 141120, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xd2, 0x14, 0xe1, 0xaf, 0x08, 0x5e, 0xda, 0x0a, 0x78,
		0x0a, 0x84, 0x46, 0x69, 0x8a, 0xb5, 0xc0, 0x12, 0x8b, 0x63, 0x92, 0xe1, 0x89, 0x88, 0x61, 0x14 }
	}, 1313451894, 0x1a094a86 },
	{ 161280, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x91, 0x1f, 0xe2, 0x62, 0x09, 0xde, 0x7f, 0xf5, 0x10,
		0xa8, 0x30, 0x64, 0x75, 0xb7, 0x5c, 0xef, 0xfd, 0x43, 0x4b, 0x68, 0xdc, 0x31, 0x94, 0x3b, 0x99 }
	}, 1326047176, 0x1a0d69d7 },
	{ 181440, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe5, 0x27, 0xfc, 0x19, 0xdf, 0x09, 0x92, 0xd5, 0x8c,
		0x12, 0xb9, 0x8e, 0xf5, 0xa1, 0x75, 0x44, 0x69, 0x6b, 0xbb, 0xa6, 0x78, 0x12, 0xef, 0x0e, 0x64 }
	}, 1337883029, 0x1a0a8b5f },
	{ 201600, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xa5, 0xe2, 0x8b, 0xef, 0x30, 0xad, 0x31, 0xf1, 0xf9,
		0xbe, 0x70, 0x6e, 0x91, 0xae, 0x9d, 0xda, 0x54, 0x17, 0x9a, 0x95, 0xc9, 0xf9, 0xcd, 0x9a, 0xd0 }
	}, 1349226660, 0x1a057e08 },
	{ 221760, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x85, 0xdd, 0x77, 0xea, 0x5e, 0xd6, 0x02, 0x0f,
		0x9e, 0x33, 0x35, 0x89, 0x39, 0x25, 0x60, 0xb4, 0x09, 0x08, 0xd3, 0x26, 0x4b, 0xd1, 0xf4, 0x01 }
	}, 1361148470, 0x1a04985c },
	{ 241920, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb7, 0x9f, 0x25, 0x9a, 0xd1, 0x46, 0x35, 0x73, 0x9a,
		0xaf, 0x0c, 0xc4, 0x88, 0x75, 0x87, 0x4b, 0x6a, 0xee, 0xcc, 0x73, 0x08, 0x26, 0x7b, 0x50, 0xfa }
	}, 1371418654, 0x1a00de15 },
	{ 262080, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xa7, 0x7b, 0xe1, 0xc3, 0x3d, 0xea, 0xc6, 0xb8,
		0xd3, 0xb7, 0xb0, 0x75, 0x7d, 0x02, 0xce, 0x72, 0xff, 0xfd, 0xdc, 0x76, 0x82, 0x35, 0xd0, 0xe2 }
	}, 1381070552, 0x1916b0ca },
	{ 282240, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xef, 0x9e, 0xe7, 0x52, 0x96, 0x07, 0x28, 0x66,
		0x69, 0x76, 0x37, 0x63, 0xe0, 0xc4, 0x6a, 0xcf, 0xde, 0xfd, 0x8a, 0x23, 0x06, 0xde, 0x5c, 0xa8 }
	}, 1390570126, 0x1901f52c },
	{ 302400, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x21, 0x32, 0xc4, 0xda, 0xaf, 0x35, 0x8a,
		0xca, 0xf4, 0x61, 0xff, 0x1c, 0x3e, 0x96, 0x57, 0x7a, 0x74, 0xe5, 0xeb, 0xf9, 0x1b, 0xb1, 0x70 }
	}, 1400928750, 0x18692842 },
	{ 322560, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xdf, 0x2d, 0xd9, 0xd4, 0xfe, 0x05, 0x78,
		0x39, 0x2e, 0x51, 0x96, 0x10, 0xe3, 0x41, 0xdd, 0x09, 0x02, 0x54, 0x69, 0xf1, 0x01, 0xcf, 0xa1 }
	}, 1411680080, 0x181fb893 },
	{ 342720, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x9c, 0xfe, 0xce, 0x84, 0x94, 0x80, 0x0d,
		0x3d, 0xcb, 0xf9, 0x58, 0x32, 0x32, 0x82, 0x5d, 0xa6, 0x40, 0xc8, 0x70, 0x3b, 0xcd, 0x27, 0xe7 }
	}, 1423496415, 0x1818bb87 },
	{ 362880, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x89, 0x8b, 0x8e, 0x65, 0x38, 0x39, 0x27,
		0x02, 0xff, 0xb9, 0x45, 0x0f, 0x90, 0x4c, 0x80, 0xeb, 0xf9, 0xd8, 0x2b, 0x51, 0x9a, 0x77, 0xd5 }
	}, 1435475246, 0x1816418e },
	{ 383040, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x97, 0x4f, 0xa1, 0xa3, 0xf8, 0x40, 0x55,
		0xad, 0x5e, 0xf0, 0xb2, 0xf9, 0x63, 0x28, 0xbc, 0x96, 0x31, 0x0c, 0xe8, 0x3d, 0xa8, 0x01, 0xc9 }
	}, 1447236692, 0x1810b289 },
	{ 403200, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc4, 0x27, 0x2a, 0x5c, 0x68, 0xb4, 0xf5,
		0x5e, 0x5a, 0xf7, 0x34, 0xe8, 0x8c, 0xea, 0xb0, 0x9a, 0xbf, 0x73, 0xe9, 0xac, 0x3b, 0x6d, 0x01 }
	}, 1458292068, 0x1806a4c3 },
	{ 423360, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x63, 0x05, 0x46, 0xcd, 0xe8, 0x48, 0x2c,
		0xc1, 0x83, 0x70, 0x8f, 0x07, 0x6a, 0x5e, 0x4d, 0x6f, 0x51, 0xcd, 0x24, 0x51, 0x8e, 0x8f, 0x85 }
	}, 1470163842, 0x18057228 },
	{ 443520, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x45, 0xd0, 0xc7, 0x89, 0x0b, 0x2c, 0x81,
		0xab, 0x51, 0x39, 0xc6, 0xe8, 0x34, 0x00, 0xe5, 0xbe, 0xd0, 0x0d, 0x23, 0xa1, 0xf8, 0xd2, 0x39 }
	}, 1481765313, 0x18038b85 },
	{ 463680, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0x1a, 0x2f, 0x46, 0x19, 0xaf, 0xe6,
		0x23, 0x57, 0xcd, 0x16, 0x58, 0x9b, 0x63, 0x8b, 0xb6, 0x38, 0xf2, 0x99, 0x20, 0x58, 0xd8, 0x8e }
	}, 1493259601, 0x18021b3e },
	{ 483840, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x98, 0x96, 0x32, 0x51, 0xfc, 0xfc, 0x19,
		0xd0, 0xfa, 0x2e, 0xf0, 0x5c, 0xf2, 0x29, 0x36, 0xa1, 0x82, 0x60, 0x9f, 0x8d, 0x65, 0x03, 0x46 }
	}, 1503802540, 0x1803c5d5 },
	{ 504000, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6c, 0xde, 0xec, 0xe5, 0x71, 0x6c, 0x9c,
		0x70, 0x0f, 0x34, 0xad, 0x98, 0xcb, 0x0e, 0xd0, 0xad, 0x2c, 0x57, 0x67, 0xbb, 0xe0, 0xbc, 0x8c }
	}, 1510516839, 0x18021abd }
};

const BRChainParams BRBCashMainNetParams = {
	BRBCashDNSSeeds,
	8333,                // standardPort
	0xe8f3e1e3,          // magicNumber
	SERVICES_NODE_BCASH, // services
	BRBCashVerifyDifficulty,
	BRBCashMainNetCheckpoints,
	sizeof(BRBCashMainNetCheckpoints)/sizeof(*BRBCashMainNetCheckpoints),
};

const BRCheckPoint BRBCashTestNetCheckpoints[] = {
	{       0, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x09, 0x33, 0xea, 0x01, 0xad, 0x0e, 0xe9, 0x84, 0x20, 0x97, 0x79, 0xba,
		0xae, 0xc3, 0xce, 0xd9, 0x0f, 0xa3, 0xf4, 0x08, 0x71, 0x95, 0x26, 0xf8, 0xd7, 0x7f, 0x49, 0x43 }
	}, 1296688602, 0x1d00ffff },
	{  100800, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0xa3, 0x31, 0x12, 0xf8, 0x6f, 0x3f, 0x7b, 0x0a, 0xa5, 0x90, 0xcb,
		0x49, 0x49, 0xb8, 0x4c, 0x2d, 0x9c, 0x67, 0x3e, 0x9e, 0x30, 0x32, 0x57, 0xb3, 0xbe, 0x90, 0x00 }
	}, 1376543922, 0x1c00d907 },
	{  201600, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x6b, 0xb7, 0x13, 0x14, 0x32, 0x1c, 0x45, 0xde, 0x30, 0x15,
		0xfe, 0x95, 0x85, 0x43, 0xaf, 0xcb, 0xad, 0xa2, 0x42, 0xa3, 0xb1, 0xb0, 0x72, 0x49, 0x8e, 0x38 }
	}, 1393813869, 0x1b602ac0 },
	{  302400, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x93, 0xeb, 0xe0, 0xa7, 0xc3, 0x34, 0x26, 0xe8, 0xed,
		0xb9, 0x75, 0x55, 0x05, 0x53, 0x7e, 0xf9, 0x30, 0x3a, 0x02, 0x3f, 0x80, 0xbe, 0x29, 0xd3, 0x2d }
	}, 1413766239, 0x1a33605e },
	{  403200, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0xef, 0x8b, 0x05, 0xda, 0x54, 0x71, 0x1e, 0x21, 0x06, 0x90, 0x77,
		0x37, 0x74, 0x1a, 0xc0, 0x27, 0x8d, 0x59, 0xf3, 0x58, 0x30, 0x3c, 0x71, 0xd5, 0x00, 0xf3, 0xc4 }
	}, 1431821666, 0x1c02346c },
	{  504000, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5d, 0x10, 0x54, 0x73, 0xc9, 0x16, 0xcd, 0x9d, 0x16, 0x33,
		0x4f, 0x01, 0x73, 0x68, 0xaf, 0xea, 0x6b, 0xce, 0xe7, 0x16, 0x29, 0xe0, 0xfc, 0xf2, 0xf4, 0xf5 }
	}, 1436951946, 0x1b00ab86 },
	{  604800, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x65, 0x3c, 0x7e, 0x5c, 0x00, 0xc7, 0x03, 0xc5, 0xa9,
		0xd5, 0x3b, 0x31, 0x88, 0x37, 0xbb, 0x1b, 0x35, 0x86, 0xa3, 0xd0, 0x60, 0xce, 0x6f, 0xff, 0x2e }
	}, 1447484641, 0x1a092a20 },
	{  705600, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x4e, 0xe3, 0xbc, 0x2e, 0x2d, 0xd0, 0x6c, 0x31, 0xf2, 0xd7, 0xa9,
		0xc3, 0xe4, 0x71, 0xec, 0x02, 0x51, 0x92, 0x4f, 0x59, 0xf2, 0x22, 0xe5, 0xe9, 0xc3, 0x7e, 0x12 }
	}, 1455728685, 0x1c0ffff0 },
	{  806400, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xaf, 0x11, 0x4f, 0xf2, 0x9d, 0xf6, 0xdb, 0xac, 0x96,
		0x9c, 0x6b, 0x4a, 0x3b, 0x40, 0x7c, 0xd7, 0x90, 0xd3, 0xa1, 0x27, 0x42, 0xb5, 0x0c, 0x23, 0x98 }
	}, 1462006183, 0x1a34e280 },
	{  907200, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x69, 0x38, 0xe6, 0xf1, 0x72, 0xa2, 0x1f, 0xe6, 0x9f, 0xe3,
		0x35, 0xe3, 0x35, 0x65, 0x53, 0x9e, 0x74, 0xbf, 0x74, 0xee, 0xb0, 0x0d, 0x20, 0x22, 0xc2, 0x26 }
	}, 1469705562, 0x1c00ffff },
	{ 1008000, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x0a, 0xca, 0x61, 0x67, 0x46, 0xa9, 0x45, 0x6a, 0x0d,
		0x64, 0xc1, 0xbd, 0x73, 0x66, 0x1f, 0xd6, 0x0a, 0x51, 0xb5, 0xbf, 0x1c, 0x92, 0xba, 0xe5, 0xa0 }
	}, 1476926743, 0x1a52ccc0 },
	{ 1108800, { .u8 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x88, 0xd9, 0xa2, 0x19, 0x41, 0x9d, 0x06, 0x07, 0xfb, 0x67,
		0xcc, 0x32, 0x4d, 0x4b, 0x6d, 0x29, 0x45, 0xca, 0x81, 0xea, 0xa5, 0xe7, 0x39, 0xfa, 0xb8, 0x1e }
	}, 1490751239, 0x1b09ecf0 }
};

const BRChainParams BRBCashTestNetParams = {
	BRBCashTestNetDNSSeeds,
	18333,               // standardPort
	0xf4f3e5f4,          // magicNumber
	SERVICES_NODE_BCASH, // services
	BRBCashTestNetVerifyDifficulty,
	BRBCashTestNetCheckpoints,
	sizeof(BRBCashTestNetCheckpoints)/sizeof(*BRBCashTestNetCheckpoints)
};

