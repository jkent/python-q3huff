#!/usr/bin/env python

import os
import q3huff
import random
import unittest

class Q3HuffTestCase(unittest.TestCase):
    def test(self):
        for _ in range(1000):
            random_bytes = os.urandom(random.randint(0, 1000))
            compressed = q3huff.compress(random_bytes)
            decompressed = q3huff.decompress(compressed)
            assert random_bytes == decompressed

if __name__ == '__main__':
    unittest.main()
