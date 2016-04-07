// Copyright (c) Alexander Koch 2016

#include <stdio.h>
#include <stdint.h>
#include <core/util.h>
#include <lexer/lexer.h>
#include <parser/parser.h>
#include <core/val.h>

#define ERR(v, msg) \
	val_free(v); \
	printf(msg); \
	env->error = 1

val_t* eval(env_t* env, val_t* v);
val_t* builtin_op(env_t* env, val_t* v, char* op) {
	for(unsigned i = 0; i < v->count; i++) {
		if(v->cell[i]->type != VNUM) {
			ERR(v, "Cannot operate on non-number!\n");
			return NULL;
		}
	}

	val_t* x = val_pop(v, 0);
	if(!strcmp(op, "-") && v->count == 0) {
		x->num = -x->num;
	}

	while(v->count > 0) {
		val_t* y = val_pop(v, 0);

		if(!strcmp(op, "+")) { x->num += y->num; }
		if(!strcmp(op, "-")) { x->num -= y->num; }
		if(!strcmp(op, "*")) { x->num *= y->num; }
		if(!strcmp(op, "/")) { x->num /= y->num; }

		val_free(y);
	}

	val_free(v);
	return x;
}

val_t* builtin_add(env_t* env, val_t* v) {
	return builtin_op(env, v, "+");
}

val_t* builtin_sub(env_t* env, val_t* v) {
	return builtin_op(env, v, "-");
}

val_t* builtin_mul(env_t* env, val_t* v) {
	return builtin_op(env, v, "*");
}

val_t* builtin_div(env_t* env, val_t* v) {
	return builtin_op(env, v, "/");
}

val_t* builtin_list(env_t* env, val_t* v) {
	v->type = VQEXPR;
	return v;
}

val_t* builtin_head(env_t* env, val_t* v) {
	if(v->count != 1) {
		ERR(v, "Too many arguments for function `head`.");
		return NULL;
	}

	if(v->cell[0]->type != VQEXPR) {
		ERR(v, "Expected Q-Expression for function `head`.");
		return NULL;
	}

	if(v->cell[0]->count == 0) {
		ERR(v, "Function `head` passed {}.");
		return NULL;
	}

	val_t* x = val_take(v, 0);
	while(x->count > 1) {val_free(val_pop(x, 1));}
	return x;
}

val_t* builtin_tail(env_t* env, val_t* v) {
	if(v->count != 1) {
		ERR(v, "Too many arguments for function `tail`.");
		return NULL;
	}

	if(v->cell[0]->type != VQEXPR) {
		ERR(v, "Expected Q-Expression for function `tail`.");
		return NULL;
	}

	if(v->cell[0]->count == 0) {
		ERR(v, "Function `tail` passed {}.");
		return NULL;
	}
	val_t* x = val_take(v, 0);
	val_free(val_pop(x, 0));
	return x;
}


val_t* builtin_def(env_t* env, val_t* v) {
	if(v->cell[0]->type != VQEXPR) {
		ERR(v, "Incorrect type for function 'def'\n");
		return NULL;
	}

	val_t* syms = v->cell[0];
	for(unsigned i = 0; i < syms->count; i++) {
		if(syms->cell[i]->type != VSYM) {
			ERR(v, "'def' cannot define non-symbol\n");
			return NULL;
		}
	}

	if(syms->count != v->count-1) {
		ERR(v, "Incorrect number of symbols\n");
		return NULL;
	}

	for(unsigned i = 0; i < syms->count; i++) {
		env_put(env, syms->cell[i], v->cell[i+1]);
	}

	val_free(v);
	return val_sexpr();
}

val_t* eval_sexpr(env_t* env, val_t* v) {
	if(v == NULL) return NULL;

	for(unsigned i = 0; i < v->count; i++) {
		v->cell[i] = eval(env, v->cell[i]);
	}

	if(env_error(env)) return NULL;

	if(v->count == 0) return v;
	if(v->count == 1) return val_take(v, 0);

	val_t* f = val_pop(v, 0);
	if(f->type != VFUN) {
		val_free(f);
		ERR(v, "First element must be a function\n");
		return NULL;
	}

	val_t* result = f->fun(env, v);
	val_free(f);
	return result;
}

val_t* eval(env_t* env, val_t* v) {
	switch(v->type) {
		case VNUM: return v;
		case VSYM: {
			val_t* x = env_get(env, v);
			val_free(v);
			return x;
		}
		case VSEXPR: return eval_sexpr(env, v);
		case VQEXPR: return v;
		default: break;
	}
	return NULL;
}

void eval_root(env_t* env, val_t* root) {
	for(unsigned i = 0; i < root->count; i++) {
		val_t* x = eval(env, root->cell[i]);
		val_println(x);
		val_free(x);
	}
}

void env_add_builtins(env_t* env) {
	env_add_builtin(env, "list", builtin_list);
	env_add_builtin(env, "head", builtin_head);
	env_add_builtin(env, "tail", builtin_tail);
	env_add_builtin(env, "def", builtin_def);

	env_add_builtin(env, "+", builtin_add);
	env_add_builtin(env, "-", builtin_sub);
	env_add_builtin(env, "*", builtin_mul);
	env_add_builtin(env, "/", builtin_div);
}

// Run arbitrary data buffer
void run(env_t* env, const char* name, char* input) {
	size_t numTokens;
	token_t* tokens = lexer_scan("<stdin>", input, &numTokens);
	if(tokens) {
		//lexer_print_tokens(tokens, numTokens);
		val_t* root = parse_buffer(tokens, numTokens);
		eval_root(env, root);

		free(root->cell);
		free(root);
		lexer_free_buffer(tokens, numTokens);
	}
}

static char buffer[2048];
void repl(env_t* env) {
	puts("Version 0.1a-dev");
	puts("Type 'exit' to exit\n");
	while(1) {
		fputs("lisp> ", stdout);
		fgets(buffer, 2048, stdin);
		if(!strncmp(buffer, "exit", 4)) break;
		run(env, "<stdin>", buffer);
		env->error = 0;
	}
}

void run_file(env_t* env, char* file) {
	char* data = readFile(file);
	if(!data) return;
	run(env, file, data);
}

int main(int argc, char** argv) {
	env_t* env = env_new();
	env_add_builtins(env);

	if(argc == 2) {
		run_file(env, argv[1]);
	} else {
		repl(env);
	}

	env_free(env);
	return 0;
}
