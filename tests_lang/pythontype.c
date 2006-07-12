
#include <Python.h>

typedef struct {
   PyObject_HEAD
   int x, y;
} point;

PyObject* point_distance(point* p) {
   PyObject* d = PyFloat_FromDouble( sqrt(pow(p->x,2)+pow(p->y,2)) );
   return d;
}

static PyMethodDef point_methods[] = {
   { "distance", (PyCFunction) point_distance, METH_NOARGS }, { 0 }
};

PyObject* point_getattr(PyObject* self, char* name) {
   if (strcmp(name, "x") == 0)
      return PyInt_FromLong(((point*)self)->x);
   else if (strcmp(name, "y") == 0)
      return PyInt_FromLong(((point*)self)->y);
   else
      return Py_FindMethod(point_methods, self, name);
}

static PyTypeObject point_type = {
   PyObject_HEAD_INIT(NULL)
   .tp_name = "point",
   .tp_basicsize = sizeof(point),
   .tp_getattr = point_getattr,
   .tp_flags = Py_TPFLAGS_DEFAULT
};

void test_point() {
   /* Initialize the type */
   point_type.tp_new = PyType_GenericNew;
   if (PyType_Ready(&point_type) < 0) return;
   /* Make an instance of it */
   point* a_point = PyObject_New(point, &point_type);
   a_point->x = 100; a_point->y = 200;
   /* Set the instance as the Python global 'P' */
   PyObject* globals = PyModule_GetDict(PyImport_AddModule("__main__"));
   PyDict_SetItemString(globals, "P", (PyObject*) a_point);
   /* Some Python code using it */
   PyRun_SimpleString(
      "print 'P.x = ' + str(P.x) \n"
      "print 'P.y = ' + str(P.y) \n"
      "print 'distance  = ' + str(P.distance()) \n"
   );
}

int main() {
   Py_Initialize();
   test_point();
   Py_Finalize();
}
