#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from setuptools import Extension, setup

__version__ = '0.4.1'

huffman_src = ['src/hufflib.c',
               'src/huffman.c',
               'src/msg.c',
               'src/q_shared.c']
huffman_dep = ['src/qcommon.h',
               'src/q_shared.h']

huffman_ext = Extension('q3huff', huffman_src, depends=huffman_dep,
                        extra_compile_args=['-std=c99'])

setup(name='q3huff', version = __version__,
      ext_modules = [huffman_ext],
      description = 'Python 3 C API wrapper for Huffman code found in ioquake3.',
      author = 'Jeff Kent',
      author_email = 'jeff@jkent.net',
      maintainer = 'Jeff Kent',
      maintainer_email = 'jeff@jkent.net',
      url = "https://github.com/jkent/python-q3huff",
      license = 'GNU GPLv2',
      classifiers = [
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU General Public License v2 or later (GPLv2+)',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: C',
        'Programming Language :: Python :: 3',
      ]
)
