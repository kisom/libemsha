///
/// \file emsha/emsha.h
/// \author K. Isom <kyle@imap.cc>
/// \date 2015-12-17
/// \brief Declares an interface for an EMbedded Secure HAshing interface.
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


#ifndef EMSHA_EMSHA_H
#define EMSHA_EMSHA_H


#include <cstdint>


// emsha is an EMbedded Secure HAshing interface.
namespace emsha {


#ifdef NDEBUG
/// EMSHA_CHECK is used for sanity checks in certain parts of
/// the code. If asserts are turned off, expand the check to an
/// if statement that will return with retval if the condition
/// isn't met.
#define EMSHA_CHECK(condition, retval) if (!(condition)) { return (retval); }
#else
/// EMSHA_CHECK is used for sanity checks in certain parts of
/// the code. If asserts are turned on, the check is expanded to
/// an assertion that the condition holds. In this case, retval
/// is not used.
#define        EMSHA_CHECK(condition, retval) (assert((condition)))
#endif

/// SHA256_HASH_SIZE is the output length of SHA-256 in bytes.
const std::uint32_t SHA256_HASH_SIZE = 32;


/// \brief Describe the result of an EMSHA operation.
///
/// The EMSHAResult type is used to indicate whether an operation
/// succeeded, and if not, what the general fault type was.
typedef enum class EMSHAResult : std::uint8_t {
	/// An unknown fault occurred. This is a serious bug in the
	/// program.
	Unknown = 0,

	/// All operations have completed successfully so far.
	OK = 1,

	/// The self-test failed.
	TestFailure = 2,

	/// A null pointer was passed in as a buffer where it shouldn't
	/// have been.
	NullPointer = 3,

	/// The Hash is in an invalid state.
	InvalidState = 4,

	/// The input to SHA256::update is too large.
	InputTooLong = 5,

	/// The self tests have been disabled, but a self-test function
	/// was called.
	SelfTestDisabled = 6
} ;


/// A Hash is an abstract base class supporting concrete classes
/// that produce digests of data.
class Hash {
public:
	virtual ~Hash() = default;

	/// \brief Bring the Hash back to its initial state.
	///
	/// That is, the idea is that
	///
	/// ```
	///     hash->reset();
	///	hash->update(...);
	///     hash->result(...);
	/// ```
	///
	/// is idempotent, assuming the inputs to update
	/// and result are constant. The implications of
	/// this for a given concrete class should be
	/// described in that class's documentation, but
	/// in general, it has the effect of preserving
	/// any initial state while removing any data
	/// written to the Hash via the update method.
	///
	/// \return An ::EMSHAResult describing the status of the
	///         operation.
	virtual EMSHAResult Reset() = 0;

	/// \brief Write message data into the Hash.
	///
	/// \param message The message data to write into the Hash.
	/// \param messageLength The length of the message data.
	/// \return An ::EMSHAResult describing the status of the
	///         operation.
	virtual EMSHAResult Update(const std::uint8_t *message,
				   std::uint32_t messageLength) = 0;

	/// \brief Carry out any final operations on the Hash.
	///
	/// After a call to finalize, no more data can be written.
	/// Additionally, it transfers out the resulting hash into its
	/// argument.
	///
	/// \param digest The buffer to store the hash in.
	/// \return An ::EMSHAResult describing the status of the
	///         operation.
	virtual EMSHAResult Finalise(std::uint8_t *digest) = 0;

	/// \brief Result transfers out the hash to the argument.
	///
	/// The Hash must keep enough state for repeated calls to
	/// result to work.
	///
	/// \param digest The buffer to store the hash in.
	/// \return An ::EMSHAResult describing the status of the
	///         operation.
	virtual EMSHAResult Result(std::uint8_t *digest) = 0;

	/// \brief Return the output size of the Hash.
	///
	/// This is how large the buffers written to by result should
	/// be.
	virtual std::uint32_t Size() = 0;
};

/// \brief Constant-time function for comparing two digests.
///
/// HashEqual provides a constant time function for comparing two
/// digests. The caller *must* ensure that both a and b are the same
/// size. The recommended approach is to use fixed-size buffers of
/// emsha::SHA256_HASH_SIZE length:
///
/// ```c++
/// uint8_t	expected[emsha::SHA256_HASH_SIZE];
/// uint8_t	actual[emsha::SHA256_HASH_SIZE];
///
/// // Fill in expected and actual using the Hash operations.
///
/// if (hash_equal(expected, actual)) {
///         proceed();
/// }
/// ```
///
/// \param a A byte buffer of size Hash::Size().
/// \param b A byte buffer of size Hash::Size().
/// \return True if both byte arrays match.
bool		HashEqual(const std::uint8_t *a, const std::uint8_t *b);


#ifndef EMSHA_NO_HEXSTRING
/// \brief Write a hex-encoded version of a byte string.
///
/// HexString writes a hex-encoded version of the src byte array into
/// dest. The caller **must** ensure that dest is `srclen * 2` bytes
/// or longer.
///
/// \param dest The destination byte array at least (`2*srclen`)
///        bytes in length.
/// \param src  A byte array containing the data to hexify.
/// \param srclen The size in bytes of src.
void HexString(std::uint8_t *dest, std::uint8_t *src, std::uint32_t srclen);
#endif // EMSHA_NO_HEXSTRING


} // end of namespace emsha


#endif // EMSHA_EMSHA_H
