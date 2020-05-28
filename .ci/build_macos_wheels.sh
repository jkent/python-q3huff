#!/usr/bin/env bash

set -e

brew update
brew outdated pyenv || brew upgrade pyenv
export PATH=~/.pyenv/shims:$PATH
for PYVER in "3.5.9" "3.6.10" "3.7.7" "3.8.3"; do
  pyenv install ${PYVER}
  pyenv global ${PYVER}
  python -m pip install wheel
  python setup.py bdist_wheel
done
