// Slothlisp - the lisp that keeps it easy and slow
// Copyright (c) Alexander Koch 2016

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <core/util.h>
#include <lexer/lexer.h>
#include <parser/parser.h>
#include <val.h>

val_t* builtin_op(val_t* v, char* op) {
	for(unsigned i = 0; i < v->count; i++) {
		if(v->cell[i]->type != VNUM) {
			printf("Cannot operate on non-number!\n");
			return v;
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

val_t* builtin_quote(val_t* v) {
	v->type = VQEXPR;
	return v;
}

val_t* builtin_head(val_t* v) {
	/*if(v->count == 1
		|| v->cell[0]->type == VQEXPR
		|| v->cell[0]->count != 0) {
		val_free(v);
		printf("Error in head function\n");
		return NULL;
	}*/

	val_t* x = val_take(v, 0);
	while(x->count > 1) {val_free(val_pop(x, 1));}
	return x;
}

val_t* builtin_tail(val_t* v) {
	/*if(v->count == 1
		|| v->cell[0]->type == VQEXPR
		|| v->cell[0]->count != 0) {
		val_free(v);
		printf("Error in tail function\n");
		return NULL;
	}*/

	val_t* x = val_take(v, 0);
	val_free(val_pop(x, 0));
	return x;
}

val_t* builtin(val_t* v, char* func) {
	if(!strcmp("quote", func)) return builtin_quote(v);
	if(!strcmp("head", func)) return builtin_head(v);
	if(!strcmp("tail", func)) return builtin_tail(v);
	if(strstr("+-*/", func)) return builtin_op(v, func);
	val_free(v);
	printf("Unknown function!\n");
	return NULL;
}

val_t* eval_sexpr(val_t* v) {
	if(v->count == 0) return v;
	if(v->count == 1) return val_take(v, 0);

	val_t* f = val_pop(v, 0);
	if(f->type != VSYM) {
		val_free(f);
		val_free(v);
		printf("First element must be a symbol\n");
		return NULL;
	}

	val_t* result = builtin(v, f->sym);
	val_free(f);
	return result;
}

val_t* eval(ast_t* node) {
	switch(node->class) {
		case kASTClassNumber: return val_num(node->number);
		case kASTClassSymbol: return val_sym(node->symbol);
		case kASTClassSExpr: {
			val_t* res = val_sexpr();
			for(int i = 0; i < vector_size(node->cells); i++) {
				val_t* sub = eval(vector_get(node->cells, i));
				res = val_add(res, sub);
			}

			return eval_sexpr(res);
		}
		case kASTClassBlock: {
			for(int i = 0; i < vector_size(node->cells); i++) {
				val_t* res = eval(vector_get(node->cells, i));
				val_println(res);
				val_free(res);
			}
			break;
		}
		default: break;
	}

	return NULL;
}

static char input[2048];
int main(int argc, char** argv) {
	puts("Slothlisp version 0.1a");
	puts("Press Ctrl+C to exit\n");

	while(true) {
		fputs("lisp> ", stdout);
		fgets(input, 2048, stdin);

		size_t numTokens;
		token_t* tokens = lexer_scan("<stdin>", input, &numTokens);
		if(tokens) {
			//lexer_print_tokens(tokens, numTokens);

			ast_t* root = parse_buffer(tokens, numTokens);
			//ast_print(root);

			eval(root);

			// Free the ast
			ast_free(root);
			lexer_free_buffer(tokens, numTokens);
		}
	}
	return 0;
}
