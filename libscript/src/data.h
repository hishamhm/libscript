
#ifndef DATA_H
#define DATA_H

#include "libscript.h"

struct script_data_ {
   script_type type;
   union {
      char* string_value;
      double128 double_value;
      int bool_value;
   } u;
};

#endif
