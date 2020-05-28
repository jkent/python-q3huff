#!/usr/bin/env bash

set -e

for PYBIN in /opt/python/*/bin; do
  "${PYBIN}/python" -m pip install wheel
  "${PYBIN}/python" setup.py bdist_wheel
done
find dist -name "*.whl" -exec auditwheel repair {} \;