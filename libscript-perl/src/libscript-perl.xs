
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "libscript.h"
#include "libscript-perl.h"

static SV * script_perl_param_to_sv(script_env* env) {
	switch (script_in_type(env)) {
	case SCRIPT_DOUBLE: return newSVnv(script_in_double(env));
	case SCRIPT_STRING: return newSVpv(script_in_string(env), 0);
	case SCRIPT_BOOL: return newSViv(script_in_bool(env));
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
		script_params(env);
		for (i = 2; i <= items; i++) {
			if (SvIOK(ST(i))) {
				script_out_int(env, SvIV(ST(i)));
			} else if (SvNOK(ST(i))) {
				script_out_double(env, SvNV(ST(i)));
			} else if (SvPOK(ST(i))) {
				script_out_string(env, SvPV_nolen(ST(i))); /* TODO: zero-term */
			} /* else: other types */
		}
		err = script_call(env, name);
		if (err != SCRIPT_OK) {
			croak(script_error_message(state->env));
		}
		switch (GIMME_V) {
		case G_SCALAR:
			RETVAL = script_perl_param_to_sv(env);
			break;
		case G_ARRAY:
			RETVAL = (SV*)newAV();
			sv_2mortal((SV*)RETVAL);
			while ( script_in_type(env) != SCRIPT_NONE ) {
				Perl_av_push(aTHX_ (AV*)RETVAL, script_perl_param_to_sv(env));
			}
		case G_VOID:
			RETVAL = &PL_sv_undef;
			break;
		}
	OUTPUT:
		RETVAL

