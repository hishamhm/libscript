
#ifndef DATA_H
#define DATA_H

#include "libscript.h"

struct script_data_ {
   script_type type;
   union {
      const char* string_value;
      double double_value;
   } u;
};

#endif
