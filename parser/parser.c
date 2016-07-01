#include "parser.h"

typedef struct {
	int position;
	token_t* tokens;
	size_t numTokens;
} parser_t;

token_t eof_tok = {
	.location = {0, 0},
	.type = TOKEN_EOF,
	.value = NULL
};

// Helper functions
token_t* parser_peek(parser_t* parser, int offset) {
	if(offset < 0) {
		printf("Tried to look back at the buffer\n");
		return NULL;
	}

	if(parser->position+offset >= parser->numTokens) {
		return &eof_tok;
	}

	return &parser->tokens[parser->position + offset];
}

void skip_newlines(parser_t* parser) {
	while(parser_peek(parser, 0)->type == TOKEN_NEWLINE) {
		parser->position++;
	}
}

// Parsing functions
val_t* parse_number(parser_t* parser) {
	token_t* num = parser_peek(parser, 0);
	parser->position++;
	return val_num(atof(num->value));
}

val_t* parse_expr(parser_t* parser);
void parse_block(parser_t* parser, val_t* root) {
	val_t* expr = NULL;
	while((expr = parse_expr(parser)) != NULL) {
		root = val_add(root, expr);
	}
}

val_t* parse_sexpr(parser_t* parser) {
	parser->position++;

	val_t* nodes = val_sexpr();
	parse_block(parser, nodes);

	token_t* end = parser_peek(parser, 0);
	if(end == NULL || end->type != TOKEN_RPAREN) {
		printf("Closing parenthesis expected\n");
		return NULL;
	}
	parser->position++;
	return nodes;
}

val_t* parse_symbol(parser_t* parser) {
	token_t* word = parser_peek(parser, 0);
	parser->position++;
	return val_sym(word->value);
}

val_t* parse_string(parser_t* parser) {
	token_t* str = parser_peek(parser, 0);
	parser->position++;
	return val_str(str->value);
}

// parse main expression
val_t* parse_expr(parser_t* parser) {
	token_t* token = parser_peek(parser, 0);
	if(token->type == TOKEN_EOF) return NULL;
	skip_newlines(parser);

	if(token->type == TOKEN_NUM)
		return parse_number(parser);
	if(token->type == TOKEN_WORD || is_operator(token))
		return parse_symbol(parser);
	if(token->type == TOKEN_STRING)
		return parse_string(parser);

	if(token->type == TOKEN_NUMBERSIGN) {
		parser->position++;
		val_t* expr = parse_expr(parser);
		if(expr->type == VSEXPR) {
			expr->type = VQEXPR;
		}
		else {
			val_t* x = val_sexpr();
			x = val_add(x, expr);
			x->type = VQEXPR;
			expr = x;
		}
		return expr;
	}

	if(token->type == TOKEN_LPAREN)
		return parse_sexpr(parser);
	if(token->type == TOKEN_RPAREN)
		return NULL;

	printf("Error: Cannot parse: %s\n", token->value);
	return NULL;
}

// main function
val_t* parse_buffer(token_t* tokens, size_t numTokens) {
	parser_t parser = {0, tokens, numTokens};
	val_t* root = val_sexpr();
	parse_block(&parser, root);
	return root;
}
