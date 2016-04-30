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
val_t* builtin_op(env_t* env, val_t* v, token_type_t op) {
	for(unsigned i = 0; i < v->count; i++) {
		if(v->cell[i]->type != VNUM) {
			ERR(v, "Cannot operate on non-number!\n");
			return NULL;
		}
	}

	val_t* x = val_pop(v, 0);
	if(op == TOKEN_SUB && v->count == 0) {
		x->num = -x->num;
	}

	while(v->count > 0) {
		val_t* y = val_pop(v, 0);

		switch(op) {
			case TOKEN_ADD: x->num += y->num; break;
			case TOKEN_SUB: x->num -= y->num; break;
			case TOKEN_MUL: x->num *= y->num; break;
			case TOKEN_DIV: x->num /= y->num; break;
			case TOKEN_MOD: x->num = ((int)x->num) % (int)y->num; break;
			default: break;
		}

		val_free(y);
	}

	val_free(v);
	return x;
}

val_t* builtin_cmp(env_t* env, val_t* v, token_type_t op, int num) {
	if(v->count != 2) {
		ERR(v, "Expected two arguments for comparison operation\n");
		return NULL;
	}

	if((v->cell[0]->type != VNUM ||
		v->cell[1]->type != VNUM) && num) {
		ERR(v, "Expected two numbers for comparison operation\n");
		return NULL;
	}

	int r = 0;
	switch(op) {
		case TOKEN_GT:
			r = (v->cell[0]->num > v->cell[1]->num);
			break;
		case TOKEN_LT:
			r = (v->cell[0]->num < v->cell[1]->num);
			break;
		case TOKEN_GE:
			r = (v->cell[0]->num >= v->cell[1]->num);
			break;
		case TOKEN_LE:
			r = (v->cell[0]->num <= v->cell[1]->num);
			break;
		case TOKEN_EQUAL:
			r = val_eq(v->cell[0], v->cell[1]);
			break;
		case TOKEN_NEQUAL:
			r = !val_eq(v->cell[0], v->cell[1]);
			break;
		default: break;
	}

	val_free(v);
	return val_num(r);
}

// Builtin number operators
val_t* builtin_add(env_t* env, val_t* v)
	{return builtin_op(env, v, TOKEN_ADD);}
val_t* builtin_sub(env_t* env, val_t* v)
	{return builtin_op(env, v, TOKEN_SUB);}
val_t* builtin_mul(env_t* env, val_t* v)
	{return builtin_op(env, v, TOKEN_MUL);}
val_t* builtin_div(env_t* env, val_t* v)
	{return builtin_op(env, v, TOKEN_DIV);}
val_t* builtin_mod(env_t* env, val_t* v)
	{return builtin_op(env, v, TOKEN_MOD);}
val_t* builtin_gt(env_t* env, val_t* v)
	{return builtin_cmp(env, v, TOKEN_GT, 1);}
val_t* builtin_lt(env_t* env, val_t* v)
	{return builtin_cmp(env, v, TOKEN_LT, 1);}
val_t* builtin_ge(env_t* env, val_t* v)
	{return builtin_cmp(env, v, TOKEN_GE, 1);}
val_t* builtin_le(env_t* env, val_t* v)
	{return builtin_cmp(env, v, TOKEN_LE, 1);}
val_t* builtin_eq(env_t* env, val_t* v)
	{return builtin_cmp(env, v, TOKEN_EQUAL, 0);}
val_t* builtin_ne(env_t* env, val_t* v)
	{return builtin_cmp(env, v, TOKEN_NEQUAL, 0);}
val_t* builtin_list(env_t* env, val_t* v)
	{v->type = VQEXPR; return v;}

val_t* builtin_head(env_t* env, val_t* v) {
	if(v->count != 1) {
		ERR(v, "Too many arguments for function `head`.\n");
		return NULL;
	}

	if(v->cell[0]->type != VQEXPR) {
		ERR(v, "Expected Q-Expression for function `head`.\n");
		return NULL;
	}

	if(v->cell[0]->count == 0) {
		ERR(v, "Function `head` passed {}.\n");
		return NULL;
	}

	val_t* x = val_take(v, 0);
	while(x->count > 1) {val_free(val_pop(x, 1));}
	return x;
}

val_t* builtin_tail(env_t* env, val_t* v) {
	if(v->count != 1) {
		ERR(v, "Too many arguments for function `tail`.\n");
		return NULL;
	}

	if(v->cell[0]->type != VQEXPR) {
		ERR(v, "Expected Q-Expression for function `tail`.\n");
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

val_t* builtin_join(env_t* env, val_t* v) {
	for(unsigned i = 0; i < v->count; i++) {
		if(v->cell[i]->type != VQEXPR) {
			ERR(v, "Passed incorrect type to function `join`.\n");
			return NULL;
		}
	}

	val_t* x = val_pop(v, 0);
	while(v->count) {
		val_t* y = val_pop(v, 0);
		while(y->count) {
			x = val_add(x, val_pop(y, 0));
		}
		val_free(y);
	}
	val_free(v);
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

val_t* builtin_var(env_t* env, val_t* v) {return builtin_vardecl(env, v, 1);}
val_t* builtin_local(env_t* env, val_t* v) {return builtin_vardecl(env, v, 0);}

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

val_t* builtin_call(env_t* env, val_t* f, val_t* v) {
	if(f->builtin) { return f->builtin(env, v); }

	int given = v->count;
	int total = f->formals->count;
	while(v->count) {
		if(f->formals->count == 0) {
			val_free(v);
			printf("Function passed too many arguments: (got: %d, expected: %d)\n", given, total);
			return NULL;
		}

		val_t* sym = val_pop(f->formals, 0);

		if(!strcmp(sym->sym, "&")) {
			if(f->formals->count != 1) {
				val_free(sym);
				ERR(v, "Function format invalid\n");
				return NULL;
			}

			val_t* nsym = val_pop(f->formals, 0);
			env_put(f->env, nsym, builtin_list(env, v));
			val_free(sym);
			val_free(nsym);
			break;
		}

		val_t* val = val_pop(v, 0);
		env_put(env, sym, val);
		val_free(sym);
		val_free(val);
	}

	val_free(v);

	if(f->formals->count > 0 && !strcmp(f->formals->cell[0]->sym, "&")) {
		if(f->formals->count != 2) {
			printf("Function format invalid}\n");
			return NULL;
		}

		val_free(val_pop(f->formals, 0));
		val_t* sym = val_pop(f->formals, 0);
		val_t* val = val_sexpr();
		val->type = VQEXPR;
		env_put(f->env, sym, val);
		val_free(sym);
		val_free(val);
	}

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

	val_t* result = builtin_call(env, f, v);
	val_free(f);
	return result;
}

val_t* eval(env_t* env, val_t* v) {
	switch(v->type) {
		case VNUM: return v;
		case VSTR: return v;
		case VSYM: {
			val_t* x = env_get(env, v);
			val_free(v);
			return x;
		}
		case VSEXPR: return eval_sexpr(env, v);
		case VQEXPR: return v;
		default: break;
	}
	return v;
}

int prompt = 0;
void eval_root(env_t* env, val_t* root) {
	for(unsigned i = 0; i < root->count; i++) {
		val_t* x = eval(env, root->cell[i]);
		if(prompt) val_println(x);
		val_free(x);
	}
}

void run_file(env_t* env, char* file);
val_t* builtin_import(env_t* env, val_t* v) {
	if(v->count != 1) {
		ERR(v, "One argument expected\n");
		return NULL;
	}

	if(v->cell[0]->type != VQEXPR
		|| v->cell[0]->count != 1) {
		ERR(v, "One Q-Expression expected\n");
		return NULL;
	}

	int tmp = prompt;
	prompt = 0;
	char* name = v->cell[0]->cell[0]->sym;
	char* module = concat(name, ".lisp");
	val_free(v);
	run_file(env, module);
	free(module);
	prompt = tmp;
	return val_sexpr();
}

val_t* builtin_if(env_t* env, val_t* v) {
	if(v->count != 3) {
		ERR(v, "If expects three arguments\n");
		return NULL;
	}
	if(v->cell[0]->type != VNUM
		|| v->cell[1]->type != VQEXPR
		|| v->cell[2]->type != VQEXPR) {
		ERR(v, "If needs a boolean, followed by two Q-Expressions\n");
		return NULL;
	}

	v->cell[1]->type = VSEXPR;
	v->cell[2]->type = VSEXPR;
	val_t* x = eval(env, val_pop(v, ((v->cell[0]->num) ? 1 : 2)));
	val_free(v);
	return x;
}

val_t* builtin_print(env_t* env, val_t* v) {
	val_t* x = eval(env, v);
	val_print(x);
	return x;
}

val_t* builtin_sqrt(env_t* env, val_t* v) {
	val_t* x = val_pop(v, 0);
	val_free(v);
	x->num = sqrt(x->num);
	return x;
}

val_t* builtin_pow(env_t* env, val_t* v) {
	val_t* x = val_pop(v, 0);
	val_t* y = val_pop(v, 0);
	val_free(v);
	x->num = pow(x->num, y->num);
	val_free(y);
	return x;
}

void env_add_builtins(env_t* env) {
	env_add_builtin(env, "list", builtin_list);
	env_add_builtin(env, "head", builtin_head);
	env_add_builtin(env, "tail", builtin_tail);
	env_add_builtin(env, "join", builtin_join);
	env_add_builtin(env, "eval", builtin_eval);
	env_add_builtin(env, "local", builtin_local);
	env_add_builtin(env, "var", builtin_var);
	env_add_builtin(env, "lambda", builtin_lambda);
	env_add_builtin(env, "import", builtin_import);
	env_add_builtin(env, "if", builtin_if);
	env_add_builtin(env, "print", builtin_print);

	env_add_builtin(env, "+", builtin_add);
	env_add_builtin(env, "-", builtin_sub);
	env_add_builtin(env, "*", builtin_mul);
	env_add_builtin(env, "/", builtin_div);
	env_add_builtin(env, "%", builtin_mod);

	env_add_builtin(env, "<", builtin_lt);
	env_add_builtin(env, ">", builtin_gt);
	env_add_builtin(env, "<=", builtin_le);
	env_add_builtin(env, ">=", builtin_ge);
	env_add_builtin(env, "==", builtin_eq);
	env_add_builtin(env, "!=", builtin_ne);
	env_add_builtin(env, "sqrt", builtin_sqrt);
	env_add_builtin(env, "pow", builtin_pow);
}

// Run arbitrary data buffer
void run(env_t* env, const char* name, char* input) {
	size_t numTokens;
	token_t* tokens = lexer_scan(name, input, &numTokens);
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
	prompt = 1;
	char font[] = {
	"    ___            __ \n"
	"   / (_)________  / /_\n"
	"  / / / ___/_  / / __/\n"
	" / / (__  ) / /_/ /_  \n"
	"/_/_/____/ /___/\\__/ \n"
	"   Liszt v0.2-dev     \n"
	};
	puts(font);
	puts("Type 'exit' to exit\n");
	while(1) {
		fputs("liszt> ", stdout);
		fgets(buffer, 2048, stdin);
		if(!strncmp(buffer, "exit", 4)) break;
		run(env, "stdin", buffer);
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
