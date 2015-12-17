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


#ifndef __EMSHA_EMSHA_HH
#define __EMSHA_EMSHA_HH


#include <cstdint>


namespace emsha {


// EMSHA_CHECK is used for sanity checks in certain parts of the code.
#ifdef NDEBUG
	// If asserts are turned off, expand the check to an if
	// statement that will return with retval if the condition
	// isn't met.
	#define EMSHA_CHECK(condition, retval) if (!(condition)) { return (retval); }
#else
	// If asserts are turned on, the check is expanded to an
	// assertion that the condition holds. In this case, retval is
	// not used.
	#define	EMSHA_CHECK(condition, retval) (assert((condition)))
#endif


	// SHA256_HASH_SIZE is the output length of SHA-256 in bytes.
	const std::uint32_t	SHA256_HASH_SIZE = 32;


	// The EMSHA_RESULT type is used to indicate whether an
	// operation succeeded, and if not, what the general fault type
	// was.
	typedef enum _EMSHA_RESULT_: std::uint8_t {
		// All operations have completed successfully so far.
		EMSHA_ROK = 0,

		// A self test or unit test failed.
		EMSHA_TEST_FAILURE = 1,

		// A null pointer was passed in as a buffer where it shouldn't
                // have been.
		EMSHA_NULLPTR = 2,

		// The Hash is in an invalid state.
		EMSHA_INVALID_STATE = 3,

                // The input to SHA256::update is too large.
                SHA256_INPUT_TOO_LONG = 4,

		// The self tests have been disabled, but a self-test function
		// was called.
		EMSHA_SELFTEST_DISABLED = 5
	} EMSHA_RESULT;


	// A Hash is generalised superclass supporting concrete classes
	// that produce digests of data.
	class Hash {
		public:
			virtual ~Hash() =0;

			// reset should bring the Hash back into its initial
			// state. That is, the idea is that
			//
			//     hash->reset(); hash->update(...)...;
			//     hash->result(...);
			//
			// is idempotent, assuming the inputs to update and
			// result are constant. The implications of this for a
			// given concrete class should be described in that
			// class's documentation, but in general, it has the
			// effect of preserving any initial state while removing
			// any data written to the Hash via the update method.
			virtual EMSHA_RESULT	reset(void) =0;

			// update is used to write message data into the Hash.
			virtual EMSHA_RESULT	update(const std::uint8_t *m,
						       std::uint32_t ml) =0;

			// finalize should carry out any final operations on the
			// Hash; after a call to finalize, no more data can be
			// written.  Additionally, it transfers out the
			// resulting hash into its argument.
			virtual EMSHA_RESULT	finalize(std::uint8_t *d) =0;

			// result is used to transfer out the hash to the
			// argument. This implies that the Hash must keep enough
			// state for repeated calls to result to work.
                        virtual EMSHA_RESULT	result(std::uint8_t *d) =0;

			// size should return the output size of the Hash; this
			// is, how large the buffers written to by result should
			// be.
			virtual std::uint32_t	size(void) =0;
	};

	// hash_equal provides a constant time function for comparing two
	// hashes. The caller *must* ensure that both a and b are the same
	// size. The recommended approach is to use fixed-size buffers of
	// emsha::SHA256_HASH_SIZE length:
	//
	// uint8_t	expected[emsha::SHA256_HASH_SIZE];
	// uint8_t	actual[emsha::SHA256_HASH_SIZE];
	//
	// // Fill in expected and actual using the Hash operations.
	//
	// if (hash_equal(expected, actual)) {
	//         proceed();
	// }
	//
	// Inputs:
	//     a, b: byte arrays that MUST contain at least
	//     emsha::SHA256_HASH_SIZE bytes. Only the first
	//     emsha::SHA256_HASH_SIZE bytes will be compared.
	//
	// Outputs:
	//     true iff both byte arrays match
	//
	//     false if the arrays do not match
	//
	bool	hash_equal(const std::uint8_t *a, const std::uint8_t *b);

#ifndef EMSHA_NO_HEXSTRING
	// hexstring writes a hex-encoded version of the src byte
	// array into dest. The caller *must* ensure that dest is
	// srclen * 2 bytes or longer.
	void	hexstring(std::uint8_t *dest, std::uint8_t *src, std::uint32_t srclen);
#endif // EMSHA_NO_HEXSTRING
} // end of namespace emsha


#endif // __EMSHA_EMSHA_HH
