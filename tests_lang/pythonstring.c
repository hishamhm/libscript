
#include <Python.h>

int main() {
   char *sentence, *newsentence;
   PyObject* sentence_py;
   Py_Initialize();
   sentence = strdup("Hello, world!\n");
   sentence_py = PyString_FromString(sentence);
   sentence[0] = 'h';
   newsentence = PyString_AsString(sentence_py);
   printf("%s", newsentence);
   newsentence[7] = 'W';
   free(sentence);
   sentence = PyString_AsString(sentence_py);
   printf("%s", sentence);
   Py_Finalize();
}
