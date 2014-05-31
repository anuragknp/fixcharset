#include <Python.h>
#include "fixcharset.h"

static char module_docstring[] =
    "This module provides an interface for fixing wrong charset";
static char fixcharset_docstring[] =
    "This module provides an interface for fixing wrong charset";

static PyObject *fixcharset_fixcharset(PyObject *self, PyObject *args);

static PyMethodDef module_methods[] = {
    {"fixcharset", fixcharset_fixcharset, METH_VARARGS, fixcharset_docstring},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_fixcharset(void)
{
    PyObject *m = Py_InitModule3("_fixcharset", module_methods, module_docstring);
    if (m == NULL)
        return;
}

static PyObject *fixcharset_fixcharset(PyObject *self, PyObject *args) {
	const char *input;
	if (!PyArg_ParseTuple(args, "s", &input))
		return NULL;
	wyString charset(input);
        yBool is_utf8 = FALSE;
	FixCharset(&charset, &is_utf8);
	return Py_BuildValue("s", charset.GetString());
}

