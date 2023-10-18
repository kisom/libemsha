///
/// \file emsha/hmac.h
/// \author K. Isom <kyle@imap.cc>
/// \date 2015-12-17
/// \brief Declares an interface for HMAC tagging.
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

#ifndef EMSHA_HMAC_H
#define EMSHA_HMAC_H


#include <cstdint>

#include "emsha.h"
#include "sha256.h"


namespace emsha {

const uint32_t HMAC_KEY_LENGTH = SHA256_MB_SIZE;

/// HMAC is a keyed hash that can be used to produce an
/// authenticated hash of some data. The HMAC is built on
/// (and uses internally) the SHA256 class; it's helpful to
/// note that faults that occur in the SHA-256 code will be
/// propagated up as the return value from many of the HMAC
/// functions.
class HMAC : Hash {
public:
	/// \brief Construct an HMAC with its initial key.
	///
	/// An HMAC is constructed with a key and the length of the
	/// key. This key is stored in the HMAC context, and is wiped
	/// by the HMAC destructor.
	///
	/// \param k The HMAC key.
	/// \param kl THe length of the HMAC key.
	HMAC(const uint8_t *k, uint32_t kl);

	/// \brief Clear any data written to the HMAC.
	///
	/// This is equivalent to constructing a new HMAC, but it
	/// preserves the keys.
	///
	/// \return EMSHAResult::OK is returned if the reset occurred
	/// 	    without (detected) fault. If a fault occurs with
	///	    the underlying SHA256 context, the error code is
	///	    returned.
	EMSHAResult Reset() override;

	/// \brief Write data into the context.
	///
	/// While there is an upper limit on the size of data that the
	/// underlying hash can operate on, this package is designed
	/// for small systems that will not approach that level of data
	/// (which is on the order of 2 exabytes), so it is not a
	/// concern for this library.
	///
	/// \param message A byte array containing the message
	///	     to be written.
	/// \param messageLength The message length, in bytes.
	/// \return An ::EMSHAResult describing the result of the
	///         operation.
	///
	///        - EMSHAResult::NullPointer is returned if m is NULL
	///          and ml is nonzero.
	///        - EMSHAResult::InvalidState is returned if the
	///          update is called after a call to finalize.
	///        - EMSHAResult::InputTooLong is returned if too much
	///          data has been written to the context.
	///        - EMSHAResult::OK is returned if the data was
	///          successfully written into the HMAC context.
	EMSHAResult Update(const std::uint8_t *message, std::uint32_t messageLength) override;

	/// \brief Complete the HMAC computation.
	///
	/// \note Once #Finalise is called, the context cannot be
	///       updated unless the context is reset.
	///
	/// \param digest A byte buffer that must be at least #HMAC.Size()
	///          in length.
	/// \return An EMSHAResult describing the result of this
	///         method:
	///
	///         - EMSHAResult::NullPointer is returned if d is a
	///           null pointer.
	///         - EMSHAResult::InvalidState is returned if the HMAC
	///           context is in an invalid state, such as if there
	///           were errors in previous updates.
	///         - EMSHAResult::OK is returned if the context was
	///           successfully finalised and the digest copied to d.
	///
	EMSHAResult Finalise(std::uint8_t *digest) override;

	/// \brief Copy the current digest into a destination buffer.
	///
	/// Copy the current digest from the HMAC context into
	/// `digest`, running #Finalise if needed. Once called, the
	/// context cannot be updated until the context is reset.
	///
	/// \param digest A byte buffer that must be at least #HMAC.size()
	///               in length.
	/// \return An ::EMSHAResult describing the result of this
	///         method:
	///
	///         - EMSHAResult::NullPointer is returned if d is a
	///           null pointer.
	///         - EMSHAResult::InvalidState is returned if the HMAC
	///           context is in an invalid state, such as if there
	///           were errors in previous updates.
	///         - EMSHAResult::OK is returned if the context was
	///           successfully finalised and the digest copied to d.
	EMSHAResult Result(std::uint8_t *digest) override;


	/// \brief Returns the output size of HMAC-SHA-256.
	///
	/// The buffers passed to #Update and #Finalise should be at
	/// least this size.
	///
	/// \return The expected size of buffers passed to result and
	///         finalize.
	std::uint32_t Size() override;

	/// When an HMAC context is destroyed, it is reset and
	/// the key material is zeroised using the STL `fill`
	/// function.
	~HMAC();
private:
	uint8_t hstate;
	SHA256  ctx;
	uint8_t k[HMAC_KEY_LENGTH];
	uint8_t	buf[SHA256_HASH_SIZE];

	EMSHAResult reset();
	inline EMSHAResult	finalResult(uint8_t *d);
};


/// \brief Perform a single-pass HMAC computation over a message.
///
/// \param k A byte buffer containing the HMAC key.
/// \param kl The length of the HMAC key.
/// \param m The message data over which the HMAC is to be computed.
/// \param ml The length of the message.
/// \param d Byte buffer that will be used to store the resulting
///          HMAC. It should be emsha::SHA256_HASH_SIZE bytes in size.
/// \return An ::EMSHAResult describing the result of the HMAC operation.
EMSHAResult
ComputeHMAC(const uint8_t *k, const uint32_t kl,
	    const uint8_t *m, const uint32_t ml,
	    uint8_t *d);


} // end of namespace emsha


#endif // EMSHA_HMAC_H
