#ifndef SIMPLE_STREAM_READER_H_
#define SIMPLE_STREAM_READER_H_

/*
The MIT License (MIT)

Copyright (c) 2019 Syoyo Fujita.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <cstdint>

namespace simple_stream {

///
/// Simple stream reader in C++11
///
class StreamReader {
  static inline void swap2(unsigned short *val) {
    unsigned short tmp = *val;
    uint8_t *dst = reinterpret_cast<uint8_t *>(val);
    uint8_t *src = reinterpret_cast<uint8_t *>(&tmp);

    dst[0] = src[1];
    dst[1] = src[0];
  }

  static inline void swap4(uint32_t *val) {
    uint32_t tmp = *val;
    uint8_t *dst = reinterpret_cast<uint8_t *>(val);
    uint8_t *src = reinterpret_cast<uint8_t *>(&tmp);

    dst[0] = src[3];
    dst[1] = src[2];
    dst[2] = src[1];
    dst[3] = src[0];
  }

  static inline void swap4(int *val) {
    int tmp = *val;
    uint8_t *dst = reinterpret_cast<uint8_t *>(val);
    uint8_t *src = reinterpret_cast<uint8_t *>(&tmp);

    dst[0] = src[3];
    dst[1] = src[2];
    dst[2] = src[1];
    dst[3] = src[0];
  }

  static inline void swap8(uint64_t *val) {
    uint64_t tmp = (*val);
    uint8_t *dst = reinterpret_cast<uint8_t *>(val);
    uint8_t *src = reinterpret_cast<uint8_t *>(&tmp);

    dst[0] = src[7];
    dst[1] = src[6];
    dst[2] = src[5];
    dst[3] = src[4];
    dst[4] = src[3];
    dst[5] = src[2];
    dst[6] = src[1];
    dst[7] = src[0];
  }

  static inline void swap8(int64_t *val) {
    int64_t tmp = (*val);
    uint8_t *dst = reinterpret_cast<uint8_t *>(val);
    uint8_t *src = reinterpret_cast<uint8_t *>(&tmp);

    dst[0] = src[7];
    dst[1] = src[6];
    dst[2] = src[5];
    dst[3] = src[4];
    dst[4] = src[3];
    dst[5] = src[2];
    dst[6] = src[1];
    dst[7] = src[0];
  }

  static void cpy4(int *dst_val, const int *src_val) {
    unsigned char *dst = reinterpret_cast<unsigned char *>(dst_val);
    const unsigned char *src = reinterpret_cast<const unsigned char *>(src_val);

    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
  }

  static void cpy4(uint32_t *dst_val, const uint32_t *src_val) {
    unsigned char *dst = reinterpret_cast<unsigned char *>(dst_val);
    const unsigned char *src = reinterpret_cast<const unsigned char *>(src_val);

    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
  }

 public:
  explicit StreamReader(const uint8_t *binary, const size_t length,
                        const bool swap_endian)
      : binary_(binary), length_(length), swap_endian_(swap_endian), idx_(0) {
    (void)pad_;
  }

  bool seek_set(const uint64_t offset) {
    if (offset > length_) {
      return false;
    }

    idx_ = offset;
    return true;
  }

  bool seek_from_currect(const int64_t offset) {
    if ((int64_t(idx_) + offset) < 0) {
      return false;
    }

    if (size_t((int64_t(idx_) + offset)) > length_) {
      return false;
    }

    idx_ = size_t(int64_t(idx_) + offset);
    return true;
  }

  size_t read(const size_t n, const uint64_t dst_len, uint8_t *dst) {
    size_t len = n;
    if ((idx_ + len) > length_) {
      len = length_ - idx_;
    }

    if (len > 0) {
      if (dst_len < len) {
        // dst does not have enough space. return 0 for a while.
        return 0;
      }

      memcpy(dst, &binary_[idx_], len);
      idx_ += len;
      return len;

    } else {
      return 0;
    }
  }

  bool read1(uint8_t *ret) {
    if ((idx_ + 1) > length_) {
      return false;
    }

    const uint8_t val = binary_[idx_];

    (*ret) = val;
    idx_ += 1;

    return true;
  }

  bool read_bool(bool *ret) {
    if ((idx_ + 1) > length_) {
      return false;
    }

    const char val = static_cast<const char>(binary_[idx_]);

    (*ret) = bool(val);
    idx_ += 1;

    return true;
  }

  bool read1(char *ret) {
    if ((idx_ + 1) > length_) {
      return false;
    }

    const char val = static_cast<const char>(binary_[idx_]);

    (*ret) = val;
    idx_ += 1;

    return true;
  }

#if 0
  bool read2(unsigned short *ret) {
    if ((idx_ + 2) > length_) {
      return false;
    }

    unsigned short val =
        *(reinterpret_cast<const unsigned short *>(&binary_[idx_]));

    if (swap_endian_) {
      swap2(&val);
    }

    (*ret) = val;
    idx_ += 2;

    return true;
  }
#endif

  bool read4(uint32_t *ret) {
    if ((idx_ + 4) > length_) {
      return false;
    }

    // use cpy4 considering unaligned access.
    const uint32_t *ptr = reinterpret_cast<const uint32_t *>(&binary_[idx_]);
    uint32_t val;
    cpy4(&val, ptr);

    if (swap_endian_) {
      swap4(&val);
    }

    (*ret) = val;
    idx_ += 4;

    return true;
  }

  bool read4(int *ret) {
    if ((idx_ + 4) > length_) {
      return false;
    }

    // use cpy4 considering unaligned access.
    const int32_t *ptr = reinterpret_cast<const int32_t *>(&binary_[idx_]);
    int32_t val;
    cpy4(&val, ptr);

    if (swap_endian_) {
      swap4(&val);
    }

    (*ret) = val;
    idx_ += 4;

    return true;
  }

#if 0
  bool read8(uint64_t *ret) {
    if ((idx_ + 8) > length_) {
      return false;
    }

    uint64_t val = *(reinterpret_cast<const uint64_t *>(&binary_[idx_]));

    if (swap_endian_) {
      swap8(&val);
    }

    (*ret) = val;
    idx_ += 8;

    return true;
  }

  bool read8(int64_t *ret) {
    if ((idx_ + 8) > length_) {
      return false;
    }

    int64_t val = *(reinterpret_cast<const int64_t *>(&binary_[idx_]));

    if (swap_endian_) {
      swap8(&val);
    }

    (*ret) = val;
    idx_ += 8;

    return true;
  }
#endif

  bool read_float(float *ret) {
    if (!ret) {
      return false;
    }

    float value;
    if (!read4(reinterpret_cast<int *>(&value))) {
      return false;
    }

    (*ret) = value;

    return true;
  }

#if 0
  bool read_double(double *ret) {
    if (!ret) {
      return false;
    }

    double value;
    if (!read8(reinterpret_cast<uint64_t *>(&value))) {
      return false;
    }

    (*ret) = value;

    return true;
  }
#endif

  bool read_string(std::string *ret) {
    if (!ret) {
      return false;
    }

    std::string value;

    // read untile '\0' or end of stream.
    for (;;) {
      char c;
      if (!read1(&c)) {
        return false;
      }

      value.push_back(c);

      if (c == '\0') {
        break;
      }
    }

    (*ret) = value;

    return true;
  }

#if 0
  bool read_value(Value *inout) {
    if (!inout) {
      return false;
    }

    if (inout->Type() == VALUE_TYPE_FLOAT) {
      float value;
      if (!read_float(&value)) {
        return false;
      }

      (*inout) = Value(value);
    } else if (inout->Type() == VALUE_TYPE_INT) {
      int value;
      if (!read4(&value)) {
        return false;
      }

      (*inout) = Value(value);
    } else {
      TINYVDBIO_ASSERT(0);
      return false;
    }

    return true;
  }
#endif

  size_t tell() const { return idx_; }

  const uint8_t *data() const { return binary_; }

  bool swap_endian() const { return swap_endian_; }

  size_t size() const { return length_; }

 private:
  const uint8_t *binary_;
  const size_t length_;
  bool swap_endian_;
  char pad_[7];
  uint64_t idx_;
};

} // namespace simple_stream

#endif // SIMPLE_STREAM_READER_H_
