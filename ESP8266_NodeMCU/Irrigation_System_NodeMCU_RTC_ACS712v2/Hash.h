
Arduino Cryptography Library
Main Page
Related Pages
Classes
Files

Search
File List
libraries
Crypto
Hash.h
    1 /*
    2  * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
    3  *
    4  * Permission is hereby granted, free of charge, to any person obtaining a
    5  * copy of this software and associated documentation files (the "Software"),
    6  * to deal in the Software without restriction, including without limitation
    7  * the rights to use, copy, modify, merge, publish, distribute, sublicense,
    8  * and/or sell copies of the Software, and to permit persons to whom the
    9  * Software is furnished to do so, subject to the following conditions:
   10  *
   11  * The above copyright notice and this permission notice shall be included
   12  * in all copies or substantial portions of the Software.
   13  *
   14  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   15  * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   16  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   17  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   18  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   19  * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   20  * DEALINGS IN THE SOFTWARE.
   21  */
   22 
   23 #ifndef CRYPTO_HASH_h
   24 #define CRYPTO_HASH_h
   25 
   26 #include <inttypes.h>
   27 #include <stddef.h>
   28 
   29 class Hash
   30 {
   31 public:
   32     Hash();
   33     virtual ~Hash();
   34 
   35     virtual size_t hashSize() const = 0;
   36     virtual size_t blockSize() const = 0;
   37 
   38     virtual void reset() = 0;
   39     virtual void update(const void *data, size_t len) = 0;
   40     virtual void finalize(void *hash, size_t len) = 0;
   41 
   42     virtual void clear() = 0;
   43 
   44     virtual void resetHMAC(const void *key, size_t keyLen) = 0;
   45     virtual void finalizeHMAC(const void *key, size_t keyLen, void *hash, size_t hashLen) = 0;
   46 
   47 protected:
   48     void formatHMACKey(void *block, const void *key, size_t len, uint8_t pad);
   49 };
   50 
   51 #endif
Generated on Fri Apr 27 2018 12:01:32 for Arduino Cryptography Library by   doxygen 1.8.6
