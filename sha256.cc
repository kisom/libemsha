/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 K. Isom <coder@kyleisom.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * copy of this  software and associated documentation  files (the "Software"),
 * to deal  in the Software  without restriction, including  without limitation
 * the rights  to use,  copy, modify,  merge, publish,  distribute, sublicense,
 * and/or  sell copies  of the  Software,  and to  permit persons  to whom  the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS  PROVIDED "AS IS", WITHOUT WARRANTY OF  ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING  BUT NOT  LIMITED TO  THE WARRANTIES  OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS  OR COPYRIGHT  HOLDERS BE  LIABLE FOR  ANY CLAIM,  DAMAGES OR  OTHER
 * LIABILITY,  WHETHER IN  AN ACTION  OF CONTRACT,  TORT OR  OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */


#include <cassert>
#include <cstdint>
#include <cstring>

#include <emsha/emsha.h>
#include <emsha/sha256.h>
#include <emsha/internal.h>
#include <algorithm>
#include <iostream>


namespace emsha {


/*
 * SHA-256 constants, from FIPS 180-4 page 11.
 */
static constexpr uint32_t sha256K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};


/*
 * SHA-256 initialisation vector, from FIPS 180-4 page 15.
 */
static constexpr uint32_t emsha256H0[] = {
    0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
    0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
};


EMSHA_RESULT
sha256Digest(const uint8_t *m, uint32_t ml, uint8_t *d)
{
	SHA256       h;
	EMSHA_RESULT ret;

	if (EMSHA_ROK != (ret = h.Update(m, ml))) {
		return ret;
	}

	return h.Finalise(d);
}


SHA256::SHA256()
    : mlen(), hStatus(), hComplete(), mbi()
{
	this->reset();
}


SHA256::~SHA256()
{
	memset(this->mb, 0, SHA256_MB_SIZE);
}


EMSHA_RESULT
SHA256::addLength(const uint32_t l)
{
	uint32_t        tmp = this->mlen + l;

	if (tmp < this->mlen) {
		return SHA256_INPUT_TOO_LONG;
	}

	this->mlen = tmp;
	assert(this->mlen > 0);

	return EMSHA_ROK;
}


EMSHA_RESULT
SHA256::Reset()
{
	return this->reset();
}

EMSHA_RESULT
SHA256::reset()
{
	// The message block is set to the initial hash vector.
	this->i_hash[0] = emsha256H0[0];
	this->i_hash[1] = emsha256H0[1];
	this->i_hash[2] = emsha256H0[2];
	this->i_hash[3] = emsha256H0[3];
	this->i_hash[4] = emsha256H0[4];
	this->i_hash[5] = emsha256H0[5];
	this->i_hash[6] = emsha256H0[6];
	this->i_hash[7] = emsha256H0[7];

	this->mbi       = 0;
	this->hStatus   = EMSHA_ROK;
	this->hComplete = 0;
	this->mlen      = 0;
	memset(this->mb, 0, SHA256_MB_SIZE);

	return this->hStatus;
}


// Read 32 bits from the byte buffer chunk as an unsigned 32-bit integer.
static uint32_t
chunkToUint32(const uint8_t *chunk)
{
	return ((*chunk) << 24) |
	       ((*(chunk + 1)) << 16) |
	       ((*(chunk + 2)) << 8) |
	       (*(chunk + 3));
}


// Copy an unsigned 32-bit integer into the start of the byte buffer chunk.
static void
uint32ToChunk(uint32_t x, uint8_t *chunk)
{
	chunk[0] = (x & 0xff000000) >> 24;
	chunk[1] = (x & 0x00ff0000) >> 16;
	chunk[2] = (x & 0x0000ff00) >> 8;
	chunk[3] = (x & 0x000000ff);
}


// FIPS 180-4, page 22.
void
SHA256::updateMessageBlock()
{
	uint32_t w[64];
	uint32_t i     = 0;
	uint32_t chunk = 0;
	uint32_t a     = 0;
	uint32_t b     = 0;
	uint32_t c     = 0;
	uint32_t d     = 0;
	uint32_t e     = 0;
	uint32_t f     = 0;
	uint32_t g     = 0;
	uint32_t h     = 0;

	while (i < 16) {
		w[i++] = chunkToUint32(this->mb + chunk);
		chunk += 4;
	}
	this->mbi = 0;

	for (i = 16; i < 64; i++) {
		w[i] = sha_sigma1(w[i - 2]) + w[i - 7] +
		       sha_sigma0(w[i - 15]) + w[i - 16];
	}

	a = this->i_hash[0];
	b = this->i_hash[1];
	c = this->i_hash[2];
	d = this->i_hash[3];
	e = this->i_hash[4];
	f = this->i_hash[5];
	g = this->i_hash[6];
	h = this->i_hash[7];

	for (i = 0; i < 64; i++) {
		uint32_t t1 = 0;
		uint32_t t2 = 0;
		t1 = h + sha_Sigma1(e) + sha_ch(e, f, g) + sha256K[i] + w[i];
		t2 = sha_Sigma0(a) + sha_maj(a, b, c);
		h  = g;
		g  = f;
		f  = e;
		e  = d + t1;
		d  = c;
		c  = b;
		b  = a;
		a  = t1 + t2;
	}

	this->i_hash[0] += a;
	this->i_hash[1] += b;
	this->i_hash[2] += c;
	this->i_hash[3] += d;
	this->i_hash[4] += e;
	this->i_hash[5] += f;
	this->i_hash[6] += g;
	this->i_hash[7] += h;
}


EMSHA_RESULT
SHA256::Update(const uint8_t *m, uint32_t ml)
{
	// Checking invariants:
	// If the message length is zero, there's nothing to be done.
	if (0 == ml) { return EMSHA_ROK; }

	// The message passed in cannot be the null pointer if the
	// message length is greater than 0.
	if (nullptr == m) { return EMSHA_NULLPTR; }

	// If the SHA256 object is in a bad state, don't proceed.
	if (EMSHA_ROK != this->hStatus) { return this->hStatus; }

	// If the hash has been finalised, don't proceed.
	if (0 != this->hComplete) { return EMSHA_INVALID_STATE; }
	// Invariants satisfied by here.

	for (uint32_t i = 0; i < ml; i++) {
		this->mb[this->mbi] = *(m + i);
		mbi++;

		if (EMSHA_ROK == this->addLength(8)) {
			if (SHA256_MB_SIZE == this->mbi) {
				this->updateMessageBlock();

				// Assumption: following the message block
				// write, the context should still be in a good
				// state.
				assert(EMSHA_ROK == this->hStatus);
			}
		}
	}

	return this->hStatus;
}


inline void
SHA256::padMessage(uint8_t pc)
{
	// Assumption: the context is not in a corrupted state.
	assert(EMSHA_ROK == this->hStatus);

	if (this->mbi < (SHA256_MB_SIZE - 8)) {
		this->mb[this->mbi++] = pc;
	} else {
		bool pc_add = false;

		if (this->mbi < SHA256_MB_SIZE - 1) {
			this->mb[this->mbi++] = pc;
			pc_add = true;
		}

		while (this->mbi < SHA256_MB_SIZE) {
			this->mb[this->mbi++] = 0;
		}

		this->updateMessageBlock();
		if (!pc_add) {
			this->mb[this->mbi++] = pc;
		}

		// Assumption: updating the message block has not left the
		// context in a corrupted state.
		assert(EMSHA_ROK == this->hStatus);
	}

	while (this->mbi < (SHA256_MB_SIZE - 8)) {
		this->mb[this->mbi++] = 0;
	}

	// lstart marks the starting point for the length packing.
	uint32_t const lstart = SHA256_MB_SIZE - 8;

	this->mb[lstart] = static_cast<uint8_t>(this->mlen >> 56);
	this->mb[lstart + 1] =
	    static_cast<uint8_t>((this->mlen & 0x00ff000000000000L) >> 48);
	this->mb[lstart + 2] =
	    static_cast<uint8_t>((this->mlen & 0x0000ff0000000000L) >> 40);
	this->mb[lstart + 3] =
	    static_cast<uint8_t>((this->mlen & 0x000000ff00000000L) >> 32);
	this->mb[lstart + 4] =
	    static_cast<uint8_t>((this->mlen & 0x00000000ff000000L) >> 24);
	this->mb[lstart + 5] =
	    static_cast<uint8_t>((this->mlen & 0x0000000000ff0000L) >> 16);
	this->mb[lstart + 6] =
	    static_cast<uint8_t>((this->mlen & 0x000000000000ff00L) >> 8);
	this->mb[lstart + 7] =
	    static_cast<uint8_t>(this->mlen & 0x00000000000000ffL);

	this->updateMessageBlock();

	// Assumption: updating the message block has not left the context in a
	// corrupted state.
	assert(EMSHA_ROK == this->hStatus);
}


EMSHA_RESULT
SHA256::Finalise(uint8_t *d)
{
	// Check invariants.
	// The digest cannot be a null pointer; this library allocates
	// no memory of its own.
	if (nullptr == d) { return EMSHA_NULLPTR; }

	// If the SHA256 object is in a bad state, don't proceed.
	if (EMSHA_ROK != this->hStatus) { return this->hStatus; }

	// If the hash has been finalised, don't proceed.
	if (0 != this->hComplete) { return EMSHA_INVALID_STATE; }
	// Invariants satisfied by here.

	this->padMessage(0x80);

	// Assumption: padding the message block has not left the context in a
	// corrupted state.
	assert(EMSHA_ROK == this->hStatus);
	std::fill(this->mb, this->mb + SHA256_MB_SIZE, 0);

	this->hComplete = 1;
	this->mlen      = 0;

	uint32ToChunk(this->i_hash[0], d);
	uint32ToChunk(this->i_hash[1], d + 4);
	uint32ToChunk(this->i_hash[2], d + 8);
	uint32ToChunk(this->i_hash[3], d + 12);
	uint32ToChunk(this->i_hash[4], d + 16);
	uint32ToChunk(this->i_hash[5], d + 20);
	uint32ToChunk(this->i_hash[6], d + 24);
	uint32ToChunk(this->i_hash[7], d + 28);

	return EMSHA_ROK;
}


EMSHA_RESULT
SHA256::Result(uint8_t *d)
{
	// Check invariants.

	// The digest cannot be a null pointer; this library allocates
	// no memory of its own.
	if (nullptr == d) { return EMSHA_NULLPTR; }

	// If the SHA256 object is in a bad state, don't proceed.
	if (EMSHA_ROK != this->hStatus) { return this->hStatus; }

	// Invariants satisfied by here.

	if (this->hComplete == 0U) {
		return this->Finalise(d);
	}

	uint32ToChunk(this->i_hash[0], d);
	uint32ToChunk(this->i_hash[1], d + 4);
	uint32ToChunk(this->i_hash[2], d + 8);
	uint32ToChunk(this->i_hash[3], d + 12);
	uint32ToChunk(this->i_hash[4], d + 16);
	uint32ToChunk(this->i_hash[5], d + 20);
	uint32ToChunk(this->i_hash[6], d + 24);
	uint32ToChunk(this->i_hash[7], d + 28);

	return EMSHA_ROK;
}


std::uint32_t
SHA256::Size()
{
	return SHA256_HASH_SIZE;
}


#ifndef EMSHA_NO_SELFTEST
static const uint8_t emptyVector[] = {
    0xe3, 0xb0, 0xc4, 0x42,
    0x98, 0xfc, 0x1c, 0x14,
    0x9a, 0xfb, 0xf4, 0xc8,
    0x99, 0x6f, 0xb9, 0x24,
    0x27, 0xae, 0x41, 0xe4,
    0x64, 0x9b, 0x93, 0x4c,
    0xa4, 0x95, 0x99, 0x1b,
    0x78, 0x52, 0xb8, 0x55
};


static const uint8_t helloWorld[] = {
    0x09, 0xca, 0x7e, 0x4e,
    0xaa, 0x6e, 0x8a, 0xe9,
    0xc7, 0xd2, 0x61, 0x16,
    0x71, 0x29, 0x18, 0x48,
    0x83, 0x64, 0x4d, 0x07,
    0xdf, 0xba, 0x7c, 0xbf,
    0xbc, 0x4c, 0x8a, 0x2e,
    0x08, 0x36, 0x0d, 0x5b,
};

constexpr uint32_t EMSHA_SELF_TEST_ITERS = 4;

static EMSHA_RESULT
runTest(const uint8_t *input, uint32_t input_len, const uint8_t *expected)
{
	uint8_t		    hexString[65]{0};
	uint8_t             d[SHA256_HASH_SIZE]{0};
	emsha::SHA256       ctx;
	emsha::EMSHA_RESULT res;

	res = ctx.Update(input, input_len);
	if (EMSHA_ROK != res) {
		return res;
	}

	for (uint32_t n = 0; n < EMSHA_SELF_TEST_ITERS; n++) {
		res = ctx.Result(d);
		if (EMSHA_ROK != res) {
			return res;
		}

		for (uint32_t i = 0; i < SHA256_HASH_SIZE; i++) {
			if (expected[i] != d[i]) {
				HexString(hexString, const_cast<uint8_t *>(d), 32);
				std::cerr << "[!] have: " << hexString << "\n";
				HexString(hexString, const_cast<uint8_t *>(helloWorld), 32);
				std::cerr << "[!] want: " << hexString << "\n";
				return EMSHA_TEST_FAILURE;
			}
		}
	}

	return EMSHA_ROK;
}


EMSHA_RESULT
sha256SelfTest()
{
	EMSHA_RESULT res;

	res = runTest(reinterpret_cast<const uint8_t *>(""), 0, emptyVector);
	if (EMSHA_ROK == res) {
		res = runTest(reinterpret_cast<const uint8_t *>("hello, world"), 12, helloWorld);
		if (res != EMSHA_ROK) {
			std::cerr << "[!] failed on hello, world.\n";
		}
	} else {
		std::cerr << "[!] failed on empty vector\n";
	}

	return res;
}


#else // #ifdef EMSHA_NO_SELFTEST
EMSHA_RESULT
sha256_self_test()
{
	return EMSHA_SELFTEST_DISABLED;
}


#endif // EMSHA_NO_SELFTEST


} // end namespace emsha
