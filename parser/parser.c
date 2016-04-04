#include "parser.h"

typedef struct {
	int position;
	token_t* tokens;
	size_t numTokens;
} parser_t;

ast_t* ast_new(ast_class_t class, location_t location) {
	ast_t* ast = malloc(sizeof(ast_t));
	ast->class = class;
	ast->location = location;
	return ast;
}

void ast_print(ast_t* ast) {
	switch(ast->class) {
		case kASTClassBlock: {
			for(int i = 0; i < vector_size(ast->cells); i++) {
				ast_print(vector_get(ast->cells, i));
				printf("\n");
			}
			break;
		}
		case kASTClassNumber: {
			printf("%f", ast->number);
			break;
		}
		case kASTClassSymbol: {
			printf("%s", ast->symbol);
			break;
		}
		case kASTClassSExpr: {
			printf("<S-Expr: ");
			for(int i = 0; i < vector_size(ast->cells); i++) {
				ast_print(vector_get(ast->cells, i));
				printf(" ");
			}
			printf(">");
		}
		default: break;
	}
}

void ast_free(ast_t* ast) {
	switch(ast->class) {
		case kASTClassBlock:
		case kASTClassSExpr: {
			for(int i = 0; i < vector_size(ast->cells); i++) {
				ast_free(vector_get(ast->cells, i));
			}
			vector_free(ast->cells);
			break;
		}
		default: break;
	}

	free(ast);
}

// Helper functions
token_t* parser_peek(parser_t* parser, int offset) {
	if(offset < 0) {
		printf("Tried to look back at the buffer\n");
	}

	if(parser->position+offset >= parser->numTokens) {
		return NULL;
	}

	return &parser->tokens[parser->position + offset];
}

// Parsing functions

ast_t* parse_number(parser_t* parser) {
	token_t* num = parser_peek(parser, 0);
	parser->position++;

	ast_t* node = ast_new(kASTClassNumber, num->location);
	node->number = atof(num->value);
	return node;
}


ast_t* parse_expr(parser_t* parser);

vector_t* parse_block(parser_t* parser) {
	vector_t* nodes = vector_new();

	ast_t* expr = NULL;
	while((expr = parse_expr(parser)) != NULL) {
		vector_push(nodes, expr);
	}

	return nodes;
}

ast_t* parse_sexpr(parser_t* parser) {
	token_t* begin = parser_peek(parser, 0);
	parser->position++;

	vector_t* nodes = parse_block(parser);
	if(nodes == NULL) {
		printf("Failed at creating S-Expr\n");
		return NULL;
	}

	token_t* end = parser_peek(parser, 0);
	if(end == NULL) return NULL;
	if(end->type != TOKEN_RPAREN) {
		printf("Closing parenthesis expected\n");
		return NULL;
	}
	parser->position++;

	ast_t* node = ast_new(kASTClassSExpr, begin->location);
	node->cells = nodes;
	return node;
}

ast_t* parse_symbol(parser_t* parser) {
	token_t* word = parser_peek(parser, 0);
	parser->position++;

	ast_t* node = ast_new(kASTClassSymbol, word->location);
	node->symbol = word->value;
	return node;
}

// parse main expression
ast_t* parse_expr(parser_t* parser) {
	token_t* token = parser_peek(parser, 0);
	//printf("%d: ", parser->position);
	if(token == NULL) return NULL;

	if(token->type == TOKEN_INT || token->type == TOKEN_FLOAT) {
		// printf("Number found\n");
		return parse_number(parser);
	}

	if(token->type == TOKEN_WORD || is_operator(token)) {
		// printf("Symbol %s found\n", token->value);
		return parse_symbol(parser);
	}

	if(token->type == TOKEN_LPAREN) {
		// printf("++ Sexpr begin\n");
		return parse_sexpr(parser);
	}

	if(token->type == TOKEN_RPAREN) {
		// printf("-- Sexpr end\n");
		return NULL;
	}

	printf("Error: Cannot parse: %s\n", token->value);
	return NULL;
}

// main function

ast_t* parse_buffer(token_t* tokens, size_t numTokens) {
	parser_t parser = {0, tokens, numTokens};

	ast_t* root = ast_new(kASTClassBlock, location_new(0, 0));
	root->cells = parse_block(&parser);

	for(int i = 0; i < vector_size(root->cells); i++) {
		ast_t* node = vector_get(root->cells, i);
		if(node->class != kASTClassSExpr) {
			printf("Only S-Expressions are allowed!\n");
			break;
		}
	}

	return root;
}
