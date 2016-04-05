#include "val.h"

env_t* env_new() {
	env_t* env = malloc(sizeof(env_t));
	env->count = 0;
	env->syms = NULL;
	env->vals = NULL;
	env->error = 0;
	return env;
}

void env_free(env_t* env) {
	for(unsigned i = 0; i < env->count; i++) {
		free(env->syms[i]);
		val_free(env->vals[i]);
	}
	free(env->syms);
	free(env->vals);
	free(env);
}

val_t* env_get(env_t* env, val_t* k) {
	for(unsigned i = 0; i < env->count; i++) {
		if(!strcmp(env->syms[i], k->sym))
			return val_copy(env->vals[i]);
	}

	printf("Unbound symbol `%s`.\n", k->sym);
	env->error = 1;
	return NULL;
}

void env_put(env_t* env, val_t* k, val_t* v) {
	for(unsigned i = 0; i < env->count; i++) {
		if(!strcmp(env->syms[i], k->sym)) {
			val_free(env->vals[i]);
			env->vals[i] = val_copy(v);
			return;
		}
	}

	env->count++;
	env->vals = realloc(env->vals, sizeof(val_t*) * env->count);
	env->syms = realloc(env->syms, sizeof(char*) * env->count);

	env->vals[env->count-1] = val_copy(v);
	env->syms[env->count-1] = malloc(strlen(k->sym)+1);
	strcpy(env->syms[env->count-1], k->sym);
}

void env_add_builtin(env_t* env, char* name, vbuiltin func) {
	val_t* k = val_sym(name);
	val_t* v = val_fun(func);
	env_put(env, k, v);
	val_free(k);
	val_free(v);
}

int env_error(env_t* env) {return env->error == 1;}

val_t* val_num(double num) {
	val_t* v = malloc(sizeof(val_t));
	v->type = VNUM;
	v->num = num;
	return v;
}

val_t* val_sym(char* sym) {
	val_t* v = malloc(sizeof(val_t));
	v->type = VSYM;
	v->sym = malloc(strlen(sym) + 1);
	strcpy(v->sym, sym);
	return v;
}

val_t* val_fun(vbuiltin func) {
	val_t* v = malloc(sizeof(val_t));
	v->type = VFUN;
	v->fun = func;
	return v;
}

val_t* val_sexpr() {
	val_t* v = malloc(sizeof(val_t));
	v->type = VSEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

val_t* val_add(val_t* v, val_t* x) {
	v->count++;
	v->cell = realloc(v->cell, sizeof(val_t) * v->count);
	v->cell[v->count-1] = x;
	return v;
}

val_t* val_pop(val_t* v, int i) {
	val_t* x = v->cell[i];
	memmove(&v->cell[i], &v->cell[i+1], sizeof(val_t) * (v->count-i-1));
	v->count--;
	v->cell = realloc(v->cell, sizeof(val_t) * v->count);
	return x;
}

val_t* val_take(val_t* v, int i) {
	val_t* x = val_pop(v, i);
	val_free(v);
	return x;
}

val_t* val_copy(val_t* v) {
	val_t* x = malloc(sizeof(val_t));
	x->type = v->type;

	switch(v->type) {
		case VFUN: x->fun = v->fun; break;
		case VNUM: x->num = v->num; break;
		case VSYM: {
			x->sym = malloc(strlen(v->sym)+1);
			strcpy(x->sym, v->sym);
			break;
		}
		case VQEXPR:
		case VSEXPR: {
			x->count = v->count;
			x->cell = malloc(sizeof(val_t) * x->count);
			for(unsigned i = 0; i < v->count; i++) {
				x->cell[i] = val_copy(v->cell[i]);
			}
			break;
		}
		default: break;
	}

	return x;
}

void val_print(val_t* v) {
	if(v == NULL) return;

	switch(v->type) {
		case VNUM:
			printf("%f", v->num);
			break;
		case VSYM:
			printf("%s", v->sym);
			break;
		case VFUN:
			printf("<function>");
			break;
		case VQEXPR: {
			putchar('{');
			for(unsigned i = 0; i < v->count; i++) {
				val_print(v->cell[i]);
				if(i < v->count-1) putchar(' ');
			}
			putchar('}');
			break;
		}
		case VSEXPR: {
			putchar('(');
			for(unsigned i = 0; i < v->count; i++) {
				val_print(v->cell[i]);
				if(i < v->count-1) putchar(' ');
			}
			putchar(')');
			break;
		}
		default: break;
	}
}

void val_println(val_t* v) {
	val_print(v);
	putchar('\n');
}

void val_free(val_t* v) {
	if(v == NULL) return;
	switch(v->type) {
		case VSYM:
			free(v->sym);
			break;
		case VQEXPR:
		case VSEXPR: {
			for(unsigned i = 0; i < v->count; i++) {
				val_free(v->cell[i]);
			}
			free(v->cell);
			break;
		}
		default: break;
	}
	free(v);
}
