///
/// \file emsha/sha256.h
/// \author K. Isom <kyle@imap.cc>
/// \date 2015-12-17
/// \brief Declares an interface for producing SHA-256 hashes.
/// 
/// The MIT License (MIT)
/// 
/// Copyright (c) 2015 K. Isom <coder@kyleisom.net>
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// copy of this  software and associated documentation  files (the "Software"),
/// to deal  in the Software  without restriction, including  without limitation
/// the rights  to use,  copy, modify,  merge, publish,  distribute, sublicense,
/// and/or  sell copies  of the  Software,  and to  permit persons  to whom  the
/// Software is furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS  PROVIDED "AS IS", WITHOUT WARRANTY OF  ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING  BUT NOT  LIMITED TO  THE WARRANTIES  OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS  OR COPYRIGHT  HOLDERS BE  LIABLE FOR  ANY CLAIM,  DAMAGES OR  OTHER
/// LIABILITY,  WHETHER IN  AN ACTION  OF CONTRACT,  TORT OR  OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.
/// 

#ifndef EMSHA_SHA256_H
#define EMSHA_SHA256_H


#include <cstdint>

#include <emsha/emsha.h>
#include <array>


namespace emsha {


/// SHA256_MB_SIZE is the size of a message block.
const uint32_t SHA256_MB_SIZE = 64;

class SHA256 : Hash {
public:
	/// \brief A SHA256 context does not need any special
	///        construction.
	///
	/// It can be declared and immediately start being used.
	SHA256();

	/// The SHA256 destructor will clear out its internal
	/// message buffer; all the members are local and
	/// not resource handles, so cleanup is minimal.
	~SHA256();

	/// \brief Clear the internal state of the SHA256 context,
	///        returning it to its initial state.
	///
	/// \return This should always return EMSHAResult::OK.
	EMSHAResult Reset() override;

	/// \brief Writes data into the SHA256.
	///
	/// While there is an upper limit on the size of data that
	/// SHA-256 can operate on, this package is designed for small
	/// systems that will not approach that level of data (which is
	/// on the order of 2 exabytes), so it is not thought to be a
	/// concern.
	///
	/// \param message A byte array containing the message to be
	///                written. It must not be NULL (unless the
	///                message length is zero).
	/// \param messageLength The message length, in bytes.
	/// \return An ::EMSHAResult describing the result of the
	///         operation.
	///
	///         - EMSHAResult::NullPointer is returned if m is a
	///           nullptr and ml is nonzero.
	///         - EMSHAResult::InvalidState is returned if the
	///           update is called after a call to finalize.
	///         - EMSHAResult::InputTooLong is returned if too much
	///           data has been written to the context.
	///         - EMSHAResult::OK is returned if the data was
	///           successfully added to the SHA-256 context.
	EMSHAResult Update(const std::uint8_t *message, std::uint32_t messageLength) override;

	/// \brief Complete the digest.
	///
	/// Once this method is called, the context cannot be updated
	/// unless the context is reset.
	///
	/// \param digest byte buffer that must be at least
	///               SHA256.size() in length.
	/// \return An ::EMSHAResult describing the result of the
	///         operation.
	///
	///         - EMSHAResult::NullPointer is returned if a nullptr
	///           is passed in.
	///         - EMSHAResult::InvalidState is returned if the
	///           SHA-256 context is in an invalid state, such as
	///           if there were errors in previous updates.
	///         - EMSHAResult::OK is returned if the context was
	///           successfully finalised and the digest copied to
	///           digest.
	EMSHAResult Finalise(std::uint8_t *digest) override;

	/// \brief Copy the result from the SHA-256
	/// context into the buffer pointed to by d,
	/// running #Finalise if needed. Once called,
	/// the context cannot be updated until the
	/// context is reset.
	///
	/// \param digest A byte buffer that must be at least
	///               SHA256.size() in length.
	/// \return An ::EMSHAResult describing the result of the
	///         operation.
	///
	///         - EMSHAResult::NullPointer is returned if a nullptr
	///           is passed in.
	///         - EMSHAResult::InvalidState is returned if the
	///           SHA-256 context is in an invalid state, such as
	///           if there were errors in previous updates.
	///         - EMSHAResult::OK is returned if the context was
	///           successfully finalised and the digest copied to
	///           digest.
	EMSHAResult Result(std::uint8_t *digest) override;

	/// \brief Returns the output size of SHA-256.
	///
	/// The buffers passed to #Update and #Finalise should be at
	/// least this size.
	///
	/// \return The expected size of buffers passed to result and
	///         finalize.
	std::uint32_t Size() override;

private:
	uint64_t mlen; // Current message length.
	uint32_t i_hash[8]; // The intermediate hash is 8x 32-bit blocks.

	// hStatus is the hash status, and hComplete indicates
	// whether the hash has been finalised.
	EMSHAResult hStatus;
	uint8_t     hComplete;

	// mb is the message block, and mbi is the message
	// block index.
	uint8_t mbi;
	std::array<uint8_t, SHA256_MB_SIZE> mb;

	inline EMSHAResult	addLength(const uint32_t);
	inline void  		updateMessageBlock(void);
	inline void  		padMessage(uint8_t pc);
	uint32_t     		chunkToUint32(uint32_t offset);
	uint32_t     		uint32ToChunk(uint32_t offset);
	EMSHAResult		reset();
}; // end class SHA256


/// \brief SHA256Digest performs a single pass hashing of the message
///        passed in.
///
/// \param m Byte buffer containing the message to hash.
/// \param ml The length of m.
/// \param d Byte buffer that will be used to store the resulting hash;
///          it should have at least emsha::SHA256_HASH_SIZE bytes
///          available.
/// \return An ::EMSHAResult describing the result of the operation.
EMSHAResult SHA256Digest(const uint8_t *m, uint32_t ml, uint8_t *d);

/// \brief SHA256SelfTest runs through two test cases to ensure that the
///        SHA-256 functions are working correctly.
///
/// \return The result of the self-test.
///
///         - EMSHAResult::OK is returned if the self tests pass.
///         - EMSHAResult::SelfTestDisabled is returned if the self
///           tests have been disabled (e.g., libemsha was compiled
///           with the EMSHA_NO_SELFTEST #define).
///         - If a fault occurred inside the SHA-256 code, the error
///           code from one of the update, finalize, result, or reset
///           methods is returned.
///         - If the fault is that the output does not match the test
///           vector, EMSHAResult::TestFailure is returned.
EMSHAResult SHA256SelfTest();


} // end of namespace emsha


#endif // EMSHA_SHA256_H
