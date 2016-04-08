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

val_t* builtin_eval(env_t* env, val_t* v) {
	if(v->count != 1) {
		ERR(v, "Too many arguments\n");
		return NULL;
	}

	if(v->cell[0]->type != VQEXPR) {
		ERR(v, "Eval expects a Q-Expression\n");
		return NULL;
	}

	val_t* x = val_take(v, 0);
	x->type = VSEXPR;
	return eval(env, x);
}

val_t* builtin_vardecl(env_t* env, val_t* v, int global) {
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
		if(global) {
			env_put_global(env, syms->cell[i], v->cell[i+1]);
		} else {
			env_put(env, syms->cell[i], v->cell[i+1]);
		}
	}

	val_free(v);
	return val_sexpr();
}

val_t* builtin_var(env_t* env, val_t* v) {
	return builtin_vardecl(env, v, 1);
}

val_t* builtin_local(env_t* env, val_t* v) {
	return builtin_vardecl(env, v, 0);
}

val_t* builtin_lambda(env_t* env, val_t* v) {
	if(v->count != 2) {
		ERR(v, "Expected two arguments in lambda\n");
		return NULL;
	}

	if(v->cell[0]->type != VQEXPR ||
		v->cell[1]->type != VQEXPR) {
		ERR(v, "Expected Q-Expressions as parameters (lambda)\n");
		return NULL;
	}

	for(int i = 0; i < v->cell[0]->count; i++) {
		if(v->cell[0]->cell[i]->type != VSYM) {
			ERR(v, "Cannot define non symbol\n");
			return NULL;
		}
	}

	val_t* formals = val_pop(v, 0);
	val_t* body = val_pop(v, 0);
	val_free(v);
	return val_lambda(formals, body);
}

val_t* val_call(env_t* env, val_t* f, val_t* v) {
	if(f->builtin) { return f->builtin(env, v); }

	int given = v->count;
	int total = f->formals->count;
	while(v->count) {
		if(f->formals->count == 0) {
			val_free(v);
			printf("Function passed too many arguments: (got: %d, expected: %d)\n", given, total);
			return 0;
		}

		val_t* sym = val_pop(f->formals, 0);
		val_t* val = val_pop(v, 0);
		env_put(env, sym, val);
		val_free(sym);
		val_free(val);
	}

	val_free(v);

	if(f->formals->count == 0) {
		f->env->par = env;
		return builtin_eval(f->env, val_add(val_sexpr(), val_copy(f->body)));
	}
	return val_copy(f);
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

	val_t* result = val_call(env, f, v);
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

void run_file(env_t* env, char* file);
val_t* builtin_import(env_t* env, val_t* v) {
	if(v->count != 1) {
		ERR(v, "One argument expected\n");
		return NULL;
	}

	if(v->cell[0]->type != VQEXPR ||
		v->cell[0]->count != 1) {
		ERR(v, "One Q-Expression expected\n");
		return NULL;
	}

	char* name = v->cell[0]->cell[0]->sym;
	char* module = concat(name, ".lisp");
	val_free(v);
	run_file(env, module);
	free(module);
	return val_sexpr();
}

void env_add_builtins(env_t* env) {
	env_add_builtin(env, "list", builtin_list);
	env_add_builtin(env, "head", builtin_head);
	env_add_builtin(env, "tail", builtin_tail);
	env_add_builtin(env, "eval", builtin_eval);
	env_add_builtin(env, "local", builtin_local);
	env_add_builtin(env, "var", builtin_var);
	env_add_builtin(env, "lambda", builtin_lambda);
	env_add_builtin(env, "import", builtin_import);

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
		// lexer_print_tokens(tokens, numTokens);
		val_t* root = parse_buffer(tokens, numTokens);
		eval_root(env, root);

		free(root->cell);
		free(root);
		lexer_free_buffer(tokens, numTokens);
	}
}

static char buffer[2048];
void repl(env_t* env) {
	puts("Liszt v0.1-dev");
	puts("Type 'exit' to exit\n");
	while(1) {
		fputs("liszt> ", stdout);
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
