/**
 * val.h
 * @author Alexander Koch 2016
 * @desc Generic value representation
 */

#ifndef val_h
#define val_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct val_t;
struct env_t;
typedef struct val_t val_t;
typedef struct env_t env_t;

typedef val_t* (*vbuiltin)(env_t*, val_t*);

enum val_type_t {
	VNUM,
	VSYM,
	VSTR,
	VFUN,
	VSEXPR,
	VQEXPR
};

struct val_t {
	int type;

	union {
		double num;
		char* sym;
		char* str;
		vbuiltin builtin;
		struct val_t** cell;
	};

	// S/Q-Expression cell size
	unsigned count;

	// Function related
	env_t* env;
	val_t* formals;
	val_t* body;
};

struct env_t {
	int error;
	env_t* par;
	unsigned count;
	char** syms;
	val_t** vals;
};

env_t* env_new();
void env_free(env_t* env);
val_t* env_get(env_t* env, val_t* v);
void env_put(env_t* env, val_t* k, val_t* v);
void env_put_global(env_t* env, val_t* k, val_t* v);
void env_add_builtin(env_t* env, char* name, vbuiltin func);
int env_error(env_t* env);
env_t* env_copy(env_t* env);

val_t* val_num(double num);
val_t* val_sym(char* sym);
val_t* val_str(char* str);
val_t* val_fun(vbuiltin func);
val_t* val_sexpr();
val_t* val_lambda(val_t* formals, val_t* body);

val_t* val_add(val_t* v, val_t* x);
val_t* val_pop(val_t* v, int i);
val_t* val_take(val_t* v, int i);
val_t* val_copy(val_t* v);
int val_eq(val_t* x, val_t* y);

void val_print(val_t* v);
void val_println(val_t* v);
void val_free(val_t* v);

#endif
