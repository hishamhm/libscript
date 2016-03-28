
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "libscript.h"
#include "libscript-perl.h"

SV* script_perl_get_sv(pTHX_ script_env* env, int i) {
   switch (script_get_type(env, i)) {
   case SCRIPT_DOUBLE: return newSVnv((double)script_get_double(env, i));
   case SCRIPT_STRING: return newSVpv(script_get_string(env, i), 0);
   case SCRIPT_BOOL: return newSViv(script_get_bool(env, i));
   default: /* pacify gcc warning */ return &PL_sv_undef;
   /* TODO: more types */
   }
}

SV** script_perl_buffer_to_stack(pTHX_ SV** sp, script_env* env) {
   int i;
   int len = script_buffer_len(env);
   for (i = 0; i < len; i++) {
      XPUSHs(sv_2mortal(script_perl_get_sv(aTHX_ env, i)));
   }
   return sp;
}

void script_perl_stack_to_buffer(pTHX_ int ax, script_env* env, int count, int offset) {
   int i;
   script_reset_buffer(env);
   for (i = 0; i < count; i++) {
      SV* o = ST(i+offset);
      if (SvIOK(o)) {
         script_put_int(env, i, SvIV(o));
      } else if (SvNOK(o)) {
         script_put_double(env, i, SvNV(o));
      } else if (SvPOK(o)) {
         script_put_string(env, i, SvPV_nolen(o));
      } else {
      	 script_put_int(env, i, 0);
      }
   }
}

MODULE = LibScript	PACKAGE = LibScript	PREFIX = script_perl_

SV *
script_perl_caller(env_i, name, ...)
	int env_i
	char* name
	CODE:
		int err;
		script_env* env = (script_env*) env_i;
		script_perl_stack_to_buffer(aTHX_ ax, env, items-2, 2);
		err = script_call(env, name);
		if (err != SCRIPT_OK) {
			croak(script_error_message(env));
		}
		switch (GIMME_V) {
		case G_SCALAR:
			RETVAL = script_perl_get_sv(aTHX_ env, 0);
			break;
		case G_ARRAY:
			{
				int len = script_buffer_len(env);
				int i;
				RETVAL = (SV*)newAV();
				sv_2mortal((SV*)RETVAL);
				for (i = 0; i < len; i++)
					Perl_av_push(aTHX_ (AV*)RETVAL, script_perl_get_sv(aTHX_ env, i));
				break;
			}
		case G_VOID:
			RETVAL = &PL_sv_undef;
			break;
		}
	OUTPUT:
		RETVAL

