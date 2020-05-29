python-q3huff [![Build Status](https://travis-ci.com/jkent/python-q3huff.svg?branch=master)](https://travis-ci.com/jkent/python-q3huff)
=============

  This is a python module for encoding and decoding data using huffman
  compression as found in the ioquake3 source.

## Requirements
  Python 3.5 or greater

## Installing the latest release:
    $ pip install q3huff

## Installing from source:
  Make sure you have python-dev setup.  For Windows, this means you need
  [Visual Studio 2015](https://stackoverflow.com/a/44290942/6844002).

    $ pip install .

## Module Documentation
### q3huff.__compress(__ bytes __)__ → bytes
> Compresses `bytes` and returns result

### q3huff.__decompress(__ bytes __)__ → bytes
> Decompresses `bytes` and returns result

### q3huff.__Reader(__ bytes __)__ → reader
> Reader objects are for reading primitive types from a `bytes` object that
> may or may not be huffman compressed, depending on the value of
> `reader.oob`.

reader.__reset(__ bytes __)__ → None

reader.__read_bits(__ num_bits __)__ → integer

reader.__read_char()__ → integer

reader.__read_byte()__ → integer

reader.__lookahead_byte()__ → integer

reader.__read_data(__ num_bytes __)__ → bytes

reader.__read_short()__ → integer

reader.__read_long()__ → integer

reader.__read_float()__ → float

reader.__read_string()__ → string

reader.__read_bigstring()__ → string

reader.__read_string_line()__ → string

reader.__read_angle()__ → float

reader.__read_angle16()__ → float

reader.__read_delta(__ old_value, num_bits __)__ → integer

reader.__read_delta_float(__ old_value __)__ → float

reader.__read_delta_key(__ key, old_value, num_bits __)__ → integer

reader.__read_delta_key_float(__ key, old_value __)__ → float

reader.__oob__
> (R/W) Boolean flag that determines if input should be huffman compressed
> or not.

### q3huff.__Writer()__ → writer
> Writer objects are for writing primitive types to a buffer that may or may
> or may not be huffman compressed, depending on the value of `writer.oob`.
> The buffer can be read using `writer.data`.

writer.__reset()__

writer.__write_bits(__ integer, num_bits __)__

writer.__write_char(__ integer __)__

writer.__write_byte(__ integer __)__

writer.__write_data(__ bytes __)__

writer.__write_short(__ integer __)__

writer.__write_long(__ integer __)__

writer.__write_float(__ float __)__

writer.__write_string(__ string __)__

writer.__write_bigstring(__ string __)__

writer.__write_angle(__ float __)__

writer.__write_angle16(__ float __)__

writer.__write_delta(__ old_value, new_value, num_bits __)__

writer.__write_delta_float(__ old_value, new_value __)__

writer.__write_delta_key(__ key, old_value, new_value, num_bits __)__

writer.__write_delta_key_float(__ key, old_value, new_value __)__

writer.__data__
> (R) Output buffer.

writer.__oob__
> (R/W) Boolean flag that determines if output should be huffman compressed
> or not.

writer.__overflow__
> (R) Boolean flag that indicates if the output buffer has overflowed
