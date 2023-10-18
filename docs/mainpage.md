# libemsha

Version: 1.0.2

Date: 2016-01-28

## Table of Contents

- Introduction
- Getting and Building the Source
- Library Overview
- The Hash interface
- The SHA256 class
- The HMAC class
- Miscellaneous functions
- Test Programs
- References

## Introduction

This library is an MIT-licensed compact HMAC-SHA-256 C++11 library
designed for embedded systems. It is built following the JPL [Power of
Ten](http://spinroot.com/gerard/pdf/P10.pdf) rules.

This library came about as a result of a need for a standalone SHA-256
library for an embedded system. The original goal was to implement a
wrapper around the code extracted from [RFC
6234](https://tools.ietf.org/html/rfc6234); instead a standalone
implementation was decided on.

Additional resources:

- [Github page](https://github.com/kisom/libemsha)
- [Travis CI status](https://travis-ci.org/kisom/libemsha/)
- [Coverity Scan page](https://scan.coverity.com/projects/libemsha-52f2a5fd-e759-43c2-9073-cf6c2ed9abdb)

## Getting and Building the Source

The source code is available via
[Github](https://github.com/kisom/libemsha/); each version should be git
tagged. :

    git clone https://github.com/kisom/libemsha
    git clone git@github.com:kisom/libemsha

The current release is
[1.0.0](https://github.com/kisom/libemsha/archive/1.0.0.zip).

The project is built using Autotools and `make`.

When building from a git checkout, the [autobuild]{.title-ref} script
will bootstrap the project from the autotools sources (e.g. via
`autoreconf -i`), run `configurei` (by default to use clang), and
attempt to build the library and run the unit tests.

Once the autotools infrastructure has been bootstrapped, the following
should work: :

    ./configure && make && make check && make install

There are three flags to `configure` that might be useful:

- `--disable-hexstring` disables the provided `hexstring` function;
  while this might be useful in many cases, it also adds extra size to
  the code.
- `--disable-hexlut` disables the larger lookup table used by
  `hexstring`, which can save around a kilobyte of program space. If
  the `hexstring` function is disabled, this option has no effect.
- `--disable-selftest` disables the internal self-tests, which can
  reclaim some additional program space.

## Library Overview

The package provides a pair of classes, :cpp`SHA256`{.interpreted-text
role="class"} and :cpp`HMAC`{.interpreted-text role="class"}, that both
satisfy a common interface :cpp`Hash`{.interpreted-text role="class"}.
All functionality provided by this library is found under the `emsha`
namespace.

### `EMSHA_RESULT`

The `EMSHA_RESULT` enum is used to convey the result of an operation.
The possible values are:

    // All operations have completed successfully so far.
    EMSHA_ROK = 0,

    // A self test or unit test failed.
    EMSHA_TEST_FAILURE = 1,

    // A null pointer was passed in as a buffer where it
    // shouldn't have been.
    EMSHA_NULLPTR = 2,

    // The Hash is in an invalid state.
    EMSHA_INVALID_STATE = 3,

    // The input to SHA256::update is too large.
    SHA256_INPUT_TOO_LONG = 4,

    // The self tests have been disabled, but a self test
    // function was called.
    EMSHA_SELFTEST_DISABLED = 5

As a convenience, the following `typedef` is also provided.

> `typedef enum _EMSHA_RESULT_` :cpp`EMSHA_RESULT`{.interpreted-text
> role="type"}

## The Hash interface

In general, a [Hash]{.title-ref} is used along the lines of: :

    emsha::EMSHA_RESULT
    hash_single_pass(uint8_t *m, uint32_t ml, uint8_t *digest)
    {
            // Depending on the implementation, the constructor may need
            // arguments.
            emsha::Hash         h;
            emsha::EMSHA_RESULT res;

            res = h.write(m, ml);
            if (emsha::EMSHA_ROK != res) {
                    return res;
            }

            // digest will contain the output of the Hash, and the
            // caller MUST ensure that there is enough space in
            // the buffer.
            return h.result(d);
    }

### Methods

## The SHA256 class

## The HMAC class

## Miscellaneous functions

## Test Programs

Running `make check` builds and runs the test programs. These are:

- `emsha_core_test` runs the core tests.
- `emsha_sha256_test` runs test vectors on the SHA-256 code.
- `emsha_hmac_test` runs test vectors on the HMAC code.

Additionally, the following test programs are built but not run. These
programs do not link with the library as the above programs do; instead,
they compile the object files in to avoid the libtool dance before the
library is installed.

- `emsha_mem_test` and `emsha_static_mem_test` are for memory
  profiling (e.g., with [Valgrind](http://valgrind.org/) during
  development.
- `emsha_static_sha256_test` and `emsha_static_hmac_test` are used to
  facilitate testing and debugging the library. These programs run the
  same tests as the `emsha_sha256_test` and `emsha_hmac_test`
  programs.

### Core Tests

There are three tests run in the core tests: a hexstring test (if
[support is built in](./building.html)) and the constant time check. The
constant time test does not validate that the function is constant time,
only that it correctly verifies that two byte arrays are equal.

### SHA-256 Tests

The SHA-256 checks take a number of test vectors from the Go standard
library\'s SHA-256 library.

### HMAC Tests

The HMAC checks apply the [RFC 4231](http://tools.ietf.org/html/rfc4231)
test vectors to the HMAC code.

## References

- [FIPS 180-4, the Secure Hash
  Standard](http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf)
- [FIPS 198-1, The Keyed-Hash Message Authentication Code
  (HMAC)](http://csrc.nist.gov/publications/fips/fips198-1/FIPS-198-1_final.pdf)
- [RFC 2014, HMAC: Keyed-Hashing for Message
  Authentication](https://tools.ietf.org/html/rfc2104)
- [RFC 6234, US Secure Hash Algorithms (SHA and SHA-based HMAC and
  HKDF)](https://tools.ietf.org/html/rfc6234)[^1]
- The behaviour of this package was cross-checked using the Go 1.5.1
  linux/amd64 standard library\'s
  [crypto/sha256](https://golang.org/src/crypto/sha256/) package.

**Footnotes**

[^1]:
    This library came about after extracting the relevant C code from
    RFC 6234, and needing a C++ version. It draws heavy inspiration from
    that code base.
