#ifndef val_h
#define val_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct val_t;
struct env_t;
typedef struct val_t val_t;
typedef struct env_t env_t;

typedef val_t* (*vbuiltin)(env_t*, val_t*);

enum val_type_t {
	VNUM,
	VSYM,
	VFUN,
	VSEXPR,
	VQEXPR
};

struct val_t {
	int type;

	unsigned count;
	union {
		double num;
		char* sym;
		vbuiltin fun;
		struct val_t** cell;
	};
};

struct env_t {
	int error;
	unsigned count;
	char** syms;
	val_t** vals;
};

env_t* env_new();
void env_free(env_t* env);
val_t* env_get(env_t* env, val_t* v);
void env_put(env_t* env, val_t* k, val_t* v);
void env_add_builtin(env_t* env, char* name, vbuiltin func);
int env_error(env_t* env);

val_t* val_num(double num);
val_t* val_sym(char* sym);
val_t* val_fun(vbuiltin func);
val_t* val_sexpr();

val_t* val_add(val_t* v, val_t* x);
val_t* val_pop(val_t* v, int i);
val_t* val_take(val_t* v, int i);
val_t* val_copy(val_t* v);

void val_print(val_t* v);
void val_println(val_t* v);
void val_free(val_t* v);

#endif
