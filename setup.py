#!/usr/bin/env python
# -*- coding: utf-8 -*-

from distutils.core import Extension, setup

__version__ = '0.1'

huffman_src = ['hufflib.c',
               'huffman.c',
               'msg.c',
               'q_shared.c']
huffman_dep = ['qcommon.h',
               'q_shared.h']

huffman_ext = Extension('q3huff', huffman_src, depends=huffman_dep,
                        extra_compile_args=['-std=c99'])

setup(name='q3huff', version = __version__,
      ext_modules = [huffman_ext],
      description = 'A python wrapper module for huffman code used in ioq3.',
      author = 'Jeff Kent',
      author_email = 'jeff@jkent.net',
      maintainer = 'Jeff Kent',
      maintainer_email = 'jeff@jkent.net',
      url = "https://github.com/jkent/q3huff",
      license = 'GNU GPLv2',
      classifiers = [
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU General Public License v2 (GPLv2)',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: C',
        'Programming Language :: Python :: 3',
      ]
)