libemsha
========

This library is an MIT-licensed HMAC-SHA-256 C++11 library designed
for embedded systems. It is built following the JPL `Power of Ten
<http://spinroot.com/gerard/pdf/P10.pdf>`_ rules. It was written in
response to a need for a standalone HMAC-SHA-256 package that could run
on several platforms.


-------------------------------
Getting and Building the Source
-------------------------------

The source code is available via `Github
<https://github.com/kisom/libemsha/>`_; each version should be git tagged. ::

    git clone https://github.com/kisom/libemsha
    git clone git@github.com:kisom/libemsha

The current release is `1.0.0 <https://github.com/kisom/libemsha/archive/1.0.0.zip>`_.

The project is built using Autotools and ``make``.

When building from a git checkout, the `autobuild` script will bootstrap
the project from the autotools sources (e.g. via ``autoreconf -i``),
run ``configure`` (by default to use clang), and attempt to build the library
and run the unit tests.

Once the autotools infrastructure has been bootstrapped, the following
should work: ::

    ./configure && make && make check && make install

There are three flags to ``configure`` that might be useful:

+ ``--disable-hexstring`` disables the provided ``hexstring`` function;
  while this might be useful in many cases, it also adds extra size to
  the code.

+ ``--disable-hexlut`` disables the larger lookup table used by
  ``hexstring``, which can save around a kilobyte of program space. If
  the ``hexstring`` function is disabled, this option has no effect.

+ ``--disable-selftest`` disables the internal self-tests, which can
  reclaim some additional program space.


-------------
Documentation
-------------

Documentation is currently done with `Sphinx <http://sphinx-doc.org/>`_.
See ``doc/sphinx/``.


See also
--------

+ `FIPS 180-4, FIPS 198-1 <http://csrc.nist.gov/publications/PubsFIPS.html (FIPS 180-4, FIPS 198-1)>`_

