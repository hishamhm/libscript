
#ifndef INTERNALS_H
#define INTERNALS_H

#ifdef __GNUC__
#define INLINE __inline
#else
#define INLINE
#endif

#include "ht.h"

#ifndef SCRIPT_EXTSEP
#define SCRIPT_EXTSEP '.'
#endif

#ifndef SCRIPT_DIRSEP
#define SCRIPT_DIRSEP '/'
#endif

#define SCRIPT_ERROR_MESSAGE_LEN 128

struct script_env_ {
   char* namespace;
   ht* plugins;
   ht* functions;
   script_data* params;
   int param_size;
   int param_ins;
   int param_outs;
   int error;
   char error_message[SCRIPT_ERROR_MESSAGE_LEN];
   const char* fn_name;
};

#define script_check_err(cond) do { if((cond)) { return env->error; } } while (0)

#define script_check_ret(cond, err, ret) do { if((cond)) { env->error = (err); return (ret); } } while (0)

#define script_check(cond, err) script_check_ret((cond), (err), (err))

#endif
