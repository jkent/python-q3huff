#include <Python.h>
#include <structmember.h>
#include "q_shared.h"
#include "qcommon.h"

/*
 * Writer Object
 */

PyDoc_STRVAR(Writer__doc__, "Writer()");
PyDoc_STRVAR(Writer_reset__doc__, "reset()");
PyDoc_STRVAR(Writer_write_bits__doc__, "write_bits(integer, num_bits)");
PyDoc_STRVAR(Writer_write_char__doc__, "write_char(integer)");
PyDoc_STRVAR(Writer_write_byte__doc__, "write_byte(integer)");
PyDoc_STRVAR(Writer_write_data__doc__, "write_data(bytes)");
PyDoc_STRVAR(Writer_write_short__doc__, "write_short(integer)");
PyDoc_STRVAR(Writer_write_long__doc__, "write_long(integer)");
PyDoc_STRVAR(Writer_write_float__doc__, "write_float(float)");
PyDoc_STRVAR(Writer_write_string__doc__, "write_string(string)");
PyDoc_STRVAR(Writer_write_bigstring__doc__, "write_bigstring(string)");
PyDoc_STRVAR(Writer_write_angle__doc__, "write_angle(float)");
PyDoc_STRVAR(Writer_write_angle16__doc__, "write_angle16(float)");
PyDoc_STRVAR(Writer_write_delta__doc__, "write_delta(old_value, new_value, num_bits)");
PyDoc_STRVAR(Writer_write_delta_float__doc__, "write_delta_float(old_value, new_value)");
PyDoc_STRVAR(Writer_write_delta_key__doc__, "write_delta_key(key, old_value, new_value, num_bits)");
PyDoc_STRVAR(Writer_write_delta_key_float__doc__, "write_delta_key_float(key, old_value, new_value)");
PyDoc_STRVAR(Writer_data__doc__, "output data from write_* functions");
PyDoc_STRVAR(Writer_oob__doc__, "flag tells if data should be written as huffman compressed or not (oob)");
PyDoc_STRVAR(Writer_overflow__doc__, "flag that indicates if the output bufer was overflowed");

typedef struct {
  PyObject_HEAD
  msg_t msgBuf;
  byte buf[MAX_MSGLEN];
} q3huff_WriterObject;

static int
Writer_init(q3huff_WriterObject *self, PyObject *args, PyObject *kwds)
{
  memset(&self->msgBuf, 0, sizeof(self->msgBuf));
  MSG_Init(&self->msgBuf, self->buf, sizeof(self->buf));
  return 0;
}

static void
Writer_dealloc(q3huff_WriterObject *self)
{
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
Writer_Reset(q3huff_WriterObject *self)
{
  memset(&self->msgBuf, 0, sizeof(self->msgBuf));
  MSG_Init(&self->msgBuf, self->buf, sizeof(self->buf));
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteBits(q3huff_WriterObject *self, PyObject *args)
{
  int value, bits;

  if (!PyArg_ParseTuple(args, "II", &value, &bits)) {
    return NULL;
  }

  if (bits < 1 || bits > 32) {
    PyErr_SetString(PyExc_OverflowError, "num_bits must be => 1 and <= 32");
    return NULL;
  }

  MSG_WriteBits(&self->msgBuf, value, bits);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteChar(q3huff_WriterObject *self, PyObject *args)
{
  signed char n;

  if (!PyArg_ParseTuple(args, "B", &n)) {
    return NULL;
  }

  MSG_WriteBits(&self->msgBuf, n, 8);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteByte(q3huff_WriterObject *self, PyObject *args)
{
  unsigned char n;

  if (!PyArg_ParseTuple(args, "b", &n)) {
    return NULL;
  }

  MSG_WriteByte(&self->msgBuf, n);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteData(q3huff_WriterObject *self, PyObject *args) {
  Py_buffer data;

  if (!PyArg_ParseTuple(args, "y*", &data)) {
    return NULL;
  }

  MSG_WriteData(&self->msgBuf, data.buf, data.len);
  PyBuffer_Release(&data);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteShort(q3huff_WriterObject *self, PyObject *args)
{
  short n;

  if (!PyArg_ParseTuple(args, "H", &n)) {
    return NULL;
  }

  MSG_WriteShort(&self->msgBuf, n);
  Py_RETURN_NONE; 
}

static PyObject *
Writer_WriteLong(q3huff_WriterObject *self, PyObject *args)
{
  int n;

  if (!PyArg_ParseTuple(args, "I", &n)) {
    return NULL;
  }

  MSG_WriteLong(&self->msgBuf, n);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteFloat(q3huff_WriterObject *self, PyObject *args)
{
  float n;

  if (!PyArg_ParseTuple(args, "f", &n)) {
    return NULL;
  }

  MSG_WriteFloat(&self->msgBuf, n);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteString(q3huff_WriterObject *self, PyObject *args)
{
  const char *s;

  if (!PyArg_ParseTuple(args, "s", &s)) {
    return NULL;
  }

  MSG_WriteString(&self->msgBuf, s);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteBigString(q3huff_WriterObject *self, PyObject *args)
{
  const char *s;

  if (!PyArg_ParseTuple(args, "s", &s)) {
    return NULL;
  }

  MSG_WriteBigString(&self->msgBuf, s);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteAngle(q3huff_WriterObject *self, PyObject *args)
{
  float n;

  if (!PyArg_ParseTuple(args, "f", &n)) {
    return NULL;
  }

  MSG_WriteAngle(&self->msgBuf, n);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteAngle16(q3huff_WriterObject *self, PyObject *args)
{
  float n;

  if (!PyArg_ParseTuple(args, "f", &n)) {
    return NULL;
  }

  MSG_WriteAngle16(&self->msgBuf, n);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteDelta(q3huff_WriterObject *self, PyObject *args)
{
  int oldV, newV, bits;

  if (!PyArg_ParseTuple(args, "III", &oldV, &newV, &bits)) {
    return NULL;
  }

  if (bits < 1 || bits > 32) {
    PyErr_SetString(PyExc_OverflowError, "num_bits must be => 1 and <= 32");
    return NULL;
  }

  MSG_WriteDelta(&self->msgBuf, oldV, newV, bits);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteDeltaFloat(q3huff_WriterObject *self, PyObject *args)
{
  float oldV, newV;

  if (!PyArg_ParseTuple(args, "ff", &oldV, &newV)) {
    return NULL;
  }

  MSG_WriteDeltaFloat(&self->msgBuf, oldV, newV);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteDeltaKey(q3huff_WriterObject *self, PyObject *args)
{
  int key, oldV, newV, bits;

  if (!PyArg_ParseTuple(args, "IIII", &key, &oldV, &newV, &bits)) {
    return NULL;
  }

  if (bits < 1 || bits > 32) {
    PyErr_SetString(PyExc_OverflowError, "num_bits must be => 1 and <= 32");
    return NULL;
  }

  MSG_WriteDeltaKey(&self->msgBuf, key, oldV, newV, bits);
  Py_RETURN_NONE;
}

static PyObject *
Writer_WriteDeltaKeyFloat(q3huff_WriterObject *self, PyObject *args)
{
  int key;
  float oldV, newV;

  if (!PyArg_ParseTuple(args, "Iff", &key, &oldV, &newV)) {
    return NULL;
  }

  MSG_WriteDeltaKeyFloat(&self->msgBuf, key, oldV, newV);
  Py_RETURN_NONE;
}

static PyObject *
Writer_getattro(q3huff_WriterObject *self, PyObject *name)
{
  PyObject *result = NULL;

  Py_INCREF(name);
  const char *cname = PyUnicode_AsUTF8(name);

  if (strcmp(cname, "data") == 0) {
    result = PyBytes_FromStringAndSize((char *)self->msgBuf.data, self->msgBuf.cursize);
    Py_INCREF(result);
  }
  else if (strcmp(cname, "oob") == 0) {
    result = PyBool_FromLong(self->msgBuf.oob);
    Py_INCREF(result);
  }
  else if (strcmp(cname, "overflow") == 0) {
    result = PyBool_FromLong(self->msgBuf.overflowed);
    Py_INCREF(result);
  }
  else {
    result = PyObject_GenericGetAttr((PyObject *)self, name);
  }
  Py_DECREF(name);

  return result;
}

static int
Writer_setattro(q3huff_WriterObject *self, PyObject *name, PyObject *value)
{
  int result = 0;

  Py_INCREF(name);
  const char *cname = PyUnicode_AsUTF8(name);

  if (strcmp(cname, "oob") == 0) {
    if (PyObject_IsTrue(value)) {
      self->msgBuf.oob = qtrue;
    }
    else {
      self->msgBuf.oob = qfalse;
    }
  }
  else {
    result = PyObject_GenericSetAttr((PyObject *)self, name, value);
  }
  Py_DECREF(name);

  return result;
}

static PyMemberDef Writer_members[] = {
  {"data", -1, 0, READONLY|RESTRICTED, Writer_data__doc__},
  {"oob", -1, 0, RESTRICTED, Writer_oob__doc__},
  {"overflow", -1, 0, READONLY|RESTRICTED, Writer_overflow__doc__},
  {NULL}
};

static PyMethodDef Writer_methods[] = {
  {"reset", (PyCFunction)Writer_Reset, METH_NOARGS, Writer_reset__doc__},
  {"write_bits", (PyCFunction)Writer_WriteBits, METH_VARARGS, Writer_write_bits__doc__},
  {"write_char", (PyCFunction)Writer_WriteChar, METH_VARARGS, Writer_write_char__doc__},
  {"write_byte", (PyCFunction)Writer_WriteByte, METH_VARARGS, Writer_write_byte__doc__},
  {"write_data", (PyCFunction)Writer_WriteData, METH_VARARGS, Writer_write_data__doc__},
  {"write_short", (PyCFunction)Writer_WriteShort, METH_VARARGS, Writer_write_short__doc__},
  {"write_long", (PyCFunction)Writer_WriteLong, METH_VARARGS, Writer_write_long__doc__},
  {"write_float", (PyCFunction)Writer_WriteFloat, METH_VARARGS, Writer_write_float__doc__},
  {"write_string", (PyCFunction)Writer_WriteString, METH_VARARGS, Writer_write_string__doc__},
  {"write_bigstring", (PyCFunction)Writer_WriteBigString, METH_VARARGS, Writer_write_bigstring__doc__},
  {"write_angle", (PyCFunction)Writer_WriteAngle, METH_VARARGS, Writer_write_angle__doc__},
  {"write_angle16", (PyCFunction)Writer_WriteAngle16, METH_VARARGS, Writer_write_angle16__doc__},
  {"write_delta", (PyCFunction)Writer_WriteDelta, METH_VARARGS, Writer_write_delta__doc__},
  {"write_delta_float", (PyCFunction)Writer_WriteDeltaFloat, METH_VARARGS, Writer_write_delta_float__doc__},
  {"write_delta_key", (PyCFunction)Writer_WriteDeltaKey, METH_VARARGS, Writer_write_delta_key__doc__},
  {"write_delta_key_float", (PyCFunction)Writer_WriteDeltaKeyFloat, METH_VARARGS, Writer_write_delta_key_float__doc__},
  {NULL}
};

static PyTypeObject q3huff_WriterType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name      = "q3huff.Writer",
  .tp_basicsize = sizeof(q3huff_WriterObject),
  .tp_dealloc   = (destructor)Writer_dealloc,
  .tp_getattro  = (getattrofunc)Writer_getattro,
  .tp_setattro  = (setattrofunc)Writer_setattro,
  .tp_flags     = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
  .tp_doc       = Writer__doc__,
  .tp_methods   = Writer_methods,
  .tp_members   = Writer_members,
  .tp_init      = (initproc)Writer_init,
  .tp_new       = PyType_GenericNew,
};

/*
 * Reader Object
 */

PyDoc_STRVAR(Reader__doc__, "Reader(bytes)");
PyDoc_STRVAR(Reader_reset__doc__, "reset(bytes)");
PyDoc_STRVAR(Reader_read_bits__doc__, "read_bits(num_bits) -> integer");
PyDoc_STRVAR(Reader_read_char__doc__, "read_char() -> integer");
PyDoc_STRVAR(Reader_read_byte__doc__, "read_byte() -> integer");
PyDoc_STRVAR(Reader_lookahead_byte__doc__, "lookahead_byte() -> integer");
PyDoc_STRVAR(Reader_read_data__doc__, "read_data(num_bytes) -> bytes");
PyDoc_STRVAR(Reader_read_short__doc__, "read_short() -> integer");
PyDoc_STRVAR(Reader_read_long__doc__, "read_long() -> integer");
PyDoc_STRVAR(Reader_read_float__doc__, "read_float() -> float");
PyDoc_STRVAR(Reader_read_string__doc__, "read_string() -> string");
PyDoc_STRVAR(Reader_read_bigstring__doc__, "read_bigstring() -> string");
PyDoc_STRVAR(Reader_read_string_line__doc__, "read_string_line() -> string");
PyDoc_STRVAR(Reader_read_angle__doc__, "read_angle() -> float");
PyDoc_STRVAR(Reader_read_angle16__doc__, "read_angle16() -> float");
PyDoc_STRVAR(Reader_read_delta__doc__, "read_delta(old_value, num_bits) -> integer");
PyDoc_STRVAR(Reader_read_delta_float__doc__, "read_delta_float(old_value) -> float");
PyDoc_STRVAR(Reader_read_delta_key__doc__, "read_delta_key(key, old_value, num_bits) -> integer");
PyDoc_STRVAR(Reader_read_delta_key_float__doc__, "read_delta_key_float(key, old_value) -> float");
PyDoc_STRVAR(Reader_oob__doc__, "flag tells if data should be read as huffman compressed or not (oob)");

typedef struct {
  PyObject_HEAD
  msg_t msgBuf;
  byte buf[MAX_MSGLEN];
} q3huff_ReaderObject;

static int
Reader_init(q3huff_ReaderObject *self, PyObject *args, PyObject *kwds)
{
  Py_buffer data;
  int len;

  if (!PyArg_ParseTuple(args, "y*", &data)) {
    return -1;
  }

  memset(&self->msgBuf, 0, sizeof(self->msgBuf));
  len = data.len > sizeof(self->buf) ? sizeof(self->buf) : data.len;
  memcpy(self->buf, data.buf, len);
  MSG_Init(&self->msgBuf, self->buf, len);
  PyBuffer_Release(&data);

  self->msgBuf.cursize = len;
  return 0;
}

static void
Reader_dealloc(q3huff_ReaderObject *self)
{
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
Reader_Reset(q3huff_ReaderObject *self, PyObject *args)
{
  Py_buffer data;
  int len;

  if (!PyArg_ParseTuple(args, "y*", &data)) {
    return NULL;
  }

  memset(&self->msgBuf, 0, sizeof(self->msgBuf));
  len = data.len > sizeof(self->buf) ? sizeof(self->buf) : data.len;
  memcpy(self->buf, data.buf, len);
  MSG_Init(&self->msgBuf, self->buf, len);
  PyBuffer_Release(&data);

  self->msgBuf.cursize = len;
  Py_RETURN_NONE;
}

static PyObject *
Reader_ReadBits(q3huff_ReaderObject *self, PyObject *args)
{
  int bits, value;

  if (!PyArg_ParseTuple(args, "I", &bits)) {
    return NULL;
  }

  value = MSG_ReadBits(&self->msgBuf, bits);
  PyObject *result = PyLong_FromLong(value);
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadChar(q3huff_ReaderObject *self)
{
  PyObject *result;

  result = PyLong_FromLong(MSG_ReadChar(&self->msgBuf));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadByte(q3huff_ReaderObject *self)
{
  PyObject *result;

  result = PyLong_FromLong(MSG_ReadByte(&self->msgBuf));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_LookaheadByte(q3huff_ReaderObject *self)
{
  PyObject *result;

  result = PyLong_FromLong(MSG_LookaheadByte(&self->msgBuf));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadData(q3huff_ReaderObject *self, PyObject *args)
{
  PyObject *result;
  int len;
  char *buf;

  if (!PyArg_ParseTuple(args, "I", &len)) {
    return NULL;
  }

  buf = malloc(len);
  if (!buf) {
    PyErr_SetString(PyExc_RuntimeError, "unable to allocate buffer");
    return NULL;
  }

  MSG_ReadData(&self->msgBuf, buf, len);
  result = PyByteArray_FromStringAndSize(buf, len);
  free(buf);
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadShort(q3huff_ReaderObject *self)
{
  PyObject *result;

  result = PyLong_FromLong(MSG_ReadShort(&self->msgBuf));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadLong(q3huff_ReaderObject *self)
{
  PyObject *result;

  result = PyLong_FromLong(MSG_ReadLong(&self->msgBuf));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadFloat(q3huff_ReaderObject *self)
{
  PyObject *result;

  result = PyFloat_FromDouble(MSG_ReadFloat(&self->msgBuf));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadString(q3huff_ReaderObject *self)
{
  PyObject *result;

  result = PyUnicode_FromString(MSG_ReadString(&self->msgBuf));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadBigString(q3huff_ReaderObject *self)
{
  PyObject *result;

  result = PyUnicode_FromString(MSG_ReadBigString(&self->msgBuf));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadStringLine(q3huff_ReaderObject *self)
{
  PyObject *result;

  result = PyUnicode_FromString(MSG_ReadStringLine(&self->msgBuf));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadAngle(q3huff_ReaderObject *self)
{
  PyObject *result;

  result = PyFloat_FromDouble(MSG_ReadAngle(&self->msgBuf));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadAngle16(q3huff_ReaderObject *self)
{
  PyObject *result;

  result = PyFloat_FromDouble(MSG_ReadAngle16(&self->msgBuf));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadDelta(q3huff_ReaderObject *self, PyObject *args)
{
  int oldV, bits;
  PyObject *result;

  if(!PyArg_ParseTuple(args, "II", &oldV, &bits)) {
    return NULL;
  }

  if (bits < 1 || bits > 32) {
    PyErr_SetString(PyExc_OverflowError, "num_bits must be => 1 and <= 32");
    return NULL;
  }

  result = PyLong_FromLong(MSG_ReadDelta(&self->msgBuf, oldV, bits));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadDeltaFloat(q3huff_ReaderObject *self, PyObject *args)
{
  float oldV;
  PyObject *result;

  if (!PyArg_ParseTuple(args, "f", &oldV)) {
    return NULL;
  }

  result = PyFloat_FromDouble(MSG_ReadDeltaFloat(&self->msgBuf, oldV));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadDeltaKey(q3huff_ReaderObject *self, PyObject *args)
{
  int key, oldV, bits;
  PyObject *result;

  if (!PyArg_ParseTuple(args, "III", &key, &oldV, &bits)) {
    return NULL;
  }

  if (bits < 1 || bits > 32) {
    PyErr_SetString(PyExc_OverflowError, "num_bits must be => 1 and <= 32");
    return NULL;
  }

  result = PyLong_FromLong(MSG_ReadDeltaKey(&self->msgBuf, key, oldV, bits));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_ReadDeltaKeyFloat(q3huff_ReaderObject *self, PyObject *args)
{
  int key;
  float oldV;
  PyObject *result;

  if (!PyArg_ParseTuple(args, "if", &key, &oldV)) {
    return NULL;
  }

  result = PyLong_FromLong(MSG_ReadDeltaKeyFloat(&self->msgBuf, key, oldV));
  Py_INCREF(result);
  return result;
}

static PyObject *
Reader_getattro(q3huff_ReaderObject *self, PyObject *name)
{
  PyObject *result = NULL;

  Py_INCREF(name);
  const char *cname = PyUnicode_AsUTF8(name);

  if (strcmp(cname, "oob") == 0) {
    result = PyBool_FromLong(self->msgBuf.oob);
    Py_INCREF(result);
  }
  else {
    result = PyObject_GenericGetAttr((PyObject *)self, name);
  }
  Py_DECREF(name);

  return result;
}

static int
Reader_setattro(q3huff_WriterObject *self, PyObject *name, PyObject *value)
{
  int result = 0;

  Py_INCREF(name);
  const char *cname = PyUnicode_AsUTF8(name);

  if (strcmp(cname, "oob") == 0) {
    if (PyObject_IsTrue(value)) {
      self->msgBuf.oob = qtrue;
    }
    else {
      self->msgBuf.oob = qfalse;
    }
  }
  else {
    result = PyObject_GenericSetAttr((PyObject *)self, name, value);
  }
  Py_DECREF(name);

  return result;
}

static PyMemberDef Reader_members[] = {
  {"oob", -1, 0, RESTRICTED, Reader_oob__doc__},
  {NULL}
};

static PyMethodDef Reader_methods[] = {
  {"reset", (PyCFunction)Reader_Reset, METH_NOARGS, Reader_reset__doc__},
  {"read_bits", (PyCFunction)Reader_ReadBits, METH_VARARGS, Reader_read_bits__doc__},
  {"read_char", (PyCFunction)Reader_ReadChar, METH_NOARGS, Reader_read_char__doc__},
  {"read_byte", (PyCFunction)Reader_ReadByte, METH_NOARGS, Reader_read_byte__doc__},
  {"lookahead_byte", (PyCFunction)Reader_LookaheadByte, METH_NOARGS, Reader_lookahead_byte__doc__},
  {"read_data", (PyCFunction)Reader_ReadData, METH_VARARGS, Reader_read_data__doc__},
  {"read_short", (PyCFunction)Reader_ReadShort, METH_NOARGS, Reader_read_short__doc__},
  {"read_long", (PyCFunction)Reader_ReadLong, METH_NOARGS, Reader_read_long__doc__},
  {"read_float", (PyCFunction)Reader_ReadFloat, METH_NOARGS, Reader_read_float__doc__},
  {"read_string", (PyCFunction)Reader_ReadString, METH_NOARGS, Reader_read_string__doc__},
  {"read_bigstring", (PyCFunction)Reader_ReadBigString, METH_NOARGS, Reader_read_bigstring__doc__},
  {"read_string_line", (PyCFunction)Reader_ReadStringLine, METH_NOARGS, Reader_read_string_line__doc__},
  {"read_angle", (PyCFunction)Reader_ReadAngle, METH_NOARGS, Reader_read_angle__doc__},
  {"read_angle16", (PyCFunction)Reader_ReadAngle16, METH_NOARGS, Reader_read_angle16__doc__},
  {"read_delta", (PyCFunction)Reader_ReadDelta, METH_VARARGS, Reader_read_delta__doc__},
  {"read_delta_float", (PyCFunction)Reader_ReadDeltaFloat, METH_VARARGS, Reader_read_delta_float__doc__},
  {"read_delta_key", (PyCFunction)Reader_ReadDeltaKey, METH_VARARGS, Reader_read_delta_key__doc__},
  {"read_delta_key_float", (PyCFunction)Reader_ReadDeltaKeyFloat, METH_VARARGS, Reader_read_delta_key_float__doc__},
  {NULL}
};

static PyTypeObject q3huff_ReaderType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name      = "q3huff.Reader",
  .tp_basicsize = sizeof(q3huff_ReaderObject),
  .tp_dealloc   = (destructor)Reader_dealloc,
  .tp_getattro  = (getattrofunc)Reader_getattro,
  .tp_setattro  = (setattrofunc)Reader_setattro,
  .tp_flags     = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
  .tp_doc       = Reader__doc__,
  .tp_methods   = Reader_methods,
  .tp_members   = Reader_members,
  .tp_init      = (initproc)Reader_init,
  .tp_new       = PyType_GenericNew,
};

/*
 *  Free functions
 */

PyDoc_STRVAR(compress__doc__, "compress(bytes) -> bytes");
PyDoc_STRVAR(decompress__doc__, "decompress(bytes) -> bytes");

static PyObject *
q3huff_Compress(PyObject *self, PyObject *args)
{
  Py_buffer data;
  msg_t msgBuf = {0};
  byte buf[MAX_MSGLEN];
  PyObject *result;

  if (!PyArg_ParseTuple(args, "y*", &data)) {
    return NULL;
  }

  msgBuf.cursize = data.len;
  if (msgBuf.cursize > sizeof(buf)) {
    msgBuf.cursize = sizeof(buf);
  }
  memcpy(buf, data.buf, msgBuf.cursize);
  PyBuffer_Release(&data);
  msgBuf.data = buf;
  msgBuf.maxsize = sizeof(buf);
  Huff_Compress(&msgBuf, 0);
  result = PyBytes_FromStringAndSize((char *)msgBuf.data, msgBuf.cursize);
  Py_INCREF(result);
  return result;
}

static PyObject *
q3huff_Decompress(PyObject *self, PyObject *args)
{
  Py_buffer data;
  msg_t msgBuf = {0};
  byte buf[MAX_MSGLEN];
  PyObject *result;

  if (!PyArg_ParseTuple(args, "y*", &data)) {
    return NULL;
  }

  memset(&msgBuf, 0, sizeof(msgBuf));
  msgBuf.cursize = data.len;
  if (msgBuf.cursize > sizeof(buf)) {
    msgBuf.cursize = sizeof(buf);
  }
  memcpy(buf, data.buf, msgBuf.cursize);
  PyBuffer_Release(&data);
  msgBuf.maxsize = sizeof(buf);
  msgBuf.data = buf;
  Huff_Decompress(&msgBuf, 0);
  result = PyBytes_FromStringAndSize((char *)msgBuf.data, msgBuf.cursize);
  Py_INCREF(result);
  return result;
}

static PyMethodDef q3huff_methods[] = {
  {"compress", (PyCFunction)q3huff_Compress, METH_VARARGS, compress__doc__},
  {"decompress", (PyCFunction)q3huff_Decompress, METH_VARARGS, decompress__doc__},
  {NULL}
};

static PyModuleDef HuffmanModule = {
  PyModuleDef_HEAD_INIT,
  "huffman",
  "A python wrapper for huffman compression used in ioquake3.",
  -1,
  q3huff_methods,
  NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_q3huff(void)
{
  PyObject *m;

  MSG_initHuffman();

  if (PyType_Ready(&q3huff_WriterType) < 0)
    return NULL;

  if (PyType_Ready(&q3huff_ReaderType) < 0)
    return NULL;

  m = PyModule_Create(&HuffmanModule);
  if (m == NULL)
    return NULL;

  Py_INCREF(&q3huff_WriterType);
  PyModule_AddObject(m, "Writer", (PyObject *)&q3huff_WriterType);

  Py_INCREF(&q3huff_ReaderType);
  PyModule_AddObject(m, "Reader", (PyObject *)&q3huff_ReaderType);

  return m;
}
