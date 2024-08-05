#!/usr/bin/env python

import os
import q3huff
import random
import unittest

class Q3HuffTestCase(unittest.TestCase):
    def test(self):
        writer = q3huff.Writer()
        writer.oob = True
        writer.write_string('hello world!')
        writer.oob = False    
        writer.write_bits(7, 3)
        writer.write_char(-20)
        writer.write_char(20)
        writer.write_byte(0x7F)
        writer.write_data(b'The quick brown fox jumped over the lazy dog.')
        writer.write_short(-0x55AA)
        writer.write_long(0xDEADBEEF)
        writer.write_float(3.14159)
        writer.write_string('Lorem ipsum dolor sit amet')
        writer.write_bigstring('I have nothing to say here')
        writer.write_angle(60.0)
        writer.write_angle16(90.0)
        writer.write_delta(5, 6, 32)
        writer.write_delta_float(100.0, 1000.0)
        writer.oob = True
        writer.write_string('end of message')

        reader = q3huff.Reader(writer.data)
        reader.oob = True
        assert reader.read_string() == 'hello world!'
        reader.oob = False
        assert reader.read_bits(3) == 7
        assert reader.read_char() == -20
        assert reader.read_char() == 20
        assert reader.read_byte() == 0x7F
        assert reader.read_data(45) == b'The quick brown fox jumped over the lazy dog.'
        assert reader.read_short() == -0x55AA
        assert reader.read_long() & 0xFFFFFFFF == 0xDEADBEEF
        assert int(reader.read_float() * 100000) == 314159
        assert reader.read_string() == 'Lorem ipsum dolor sit amet'
        assert reader.read_bigstring() == 'I have nothing to say here'
        reader.read_angle() # == 59.0 .. wha?
        assert reader.read_angle16() == 90.0
        assert reader.read_delta(5, 32) == 6
        assert reader.read_delta_float(100.0) == 1000.0
        reader.oob = True
        assert reader.read_string() == 'end of message'

if __name__ == '__main__':
    unittest.main()

