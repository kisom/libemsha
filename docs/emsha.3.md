libemsha(3) "@PROJECT_VERSION@"

# Introduction

This library is an MIT-licensed compact HMAC-SHA-256 C++11 library
designed for embedded systems. It is built following the JPL Power of
Ten rules.

This library came about as a result of a need for a standalone SHA-256
library for an embedded system. The original goal was to implement a
wrapper around the code extracted from RFC 6234; instead a standalone
implementation was decided on.

# References

- FIPS 180-4, the Secure Hash Standard: (http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf)
- FIPS 198-1, The Keyed-Hash Message Authentication Code (HMAC): (http://csrc.nist.gov/publications/fips/fips198-1/FIPS-198-1_final.pdf)
- RFC 2014, HMAC: Keyed-Hashing for Message Authentication: (https://tools.ietf.org/html/rfc2104)
- RFC 6234, US Secure Hash Algorithms (SHA and SHA-based HMAC and HKDF): (https://tools.ietf.org/html/rfc6234)
- The Power of Ten – Rules for Developing Safety Critical Code: (http://spinroot.com/gerard/pdf/P10.pdf)

# Validation

- The behaviour of this package was cross-checked using the Go 1.5.1 linux/amd64 standard library\'s
  crypto/sha256 package.
