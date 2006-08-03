
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "libscript.h"
#include "libscript-perl.h"

static SV * script_perl_param_to_sv(script_env* env, int i) {
	switch (script_get_type(env, i)) {
	case SCRIPT_DOUBLE: return newSVnv(script_get_double(env, i));
	case SCRIPT_STRING: return newSVpv(script_get_string(env, i), 0);
	case SCRIPT_BOOL: return newSViv(script_get_bool(env, i));
	default: /* pacify gcc warning */ return &PL_sv_undef;
	/* TODO: more types */
	}
}

MODULE = LibScript	PACKAGE = LibScript	PREFIX = script_perl_

SV *
script_perl_call(state_i, name, ...)
	int state_i
	char* name
	CODE:
		int i, err;
		script_perl_state* state = (script_perl_state*) state_i;
		script_env* env = state->env;
		for (i = 2; i <= items; i++) {
			if (SvIOK(ST(i))) {
				script_put_int(env, i-2, SvIV(ST(i)));
			} else if (SvNOK(ST(i))) {
				script_put_double(env, i-2, SvNV(ST(i)));
			} else if (SvPOK(ST(i))) {
				script_put_string(env, i-2, SvPV_nolen(ST(i))); /* TODO: zero-term */
			} /* else: other types */
		}
		err = script_call(env, name);
		if (err != SCRIPT_OK) {
			croak(script_error_message(state->env));
		}
		switch (GIMME_V) {
		case G_SCALAR:
			RETVAL = script_perl_param_to_sv(env, 0);
			break;
		case G_ARRAY:
			{
				int len = script_param_count(env);
				int i;
				RETVAL = (SV*)newAV();
				sv_2mortal((SV*)RETVAL);
				for (i = 0; i < len; i++)
					Perl_av_push(aTHX_ (AV*)RETVAL, script_perl_param_to_sv(env, i));
			}
		case G_VOID:
			RETVAL = &PL_sv_undef;
			break;
		}
	OUTPUT:
		RETVAL

