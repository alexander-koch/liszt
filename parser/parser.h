#ifndef parser_h
#define parser_h

#include <stdlib.h>
#include <lexer/lexer.h>
#include <adt/vector.h>

// <number> = -?[0-9]+;
// <symbol> = "+" | "-" | "*" | "/";
// <expr> = <number> | '(', <operator>, <expr>+, ')';
// <program> = {<expr>}

typedef enum {
	kASTClassNumber,
	kASTClassSymbol,
	kASTClassSExpr,
	kASTClassBlock
} ast_class_t;

typedef struct {
	ast_class_t class;
	location_t location;

	union {
		double number;
		char* symbol;
		vector_t* cells;
	};
} ast_t;

ast_t* ast_new(ast_class_t class, location_t location);
void ast_print(ast_t* ast);
void ast_free(ast_t* ast);

ast_t* parse_buffer(token_t* tokens, size_t numTokens);

#endif
