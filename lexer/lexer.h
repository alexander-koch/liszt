#ifndef lexer_h
#define lexer_h

// Lexer - reads a string and converts its content
// into text fragments: Tokens.
// Each tokens has a certain type with a string value.
// The string value may be NULL.

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <core/util.h>
#include <core/bytebuffer.h>

// Location in file / code location
typedef struct {
    unsigned int line;
    unsigned int column;
} location_t;

#define location_new(x, y) (location_t) {x, y}

// Lexer structure
typedef struct lexer_t {
    location_t location;
    const char* name;
    const char* source;
    const char* cursor;
    const char* lastline;
    int error;
} lexer_t;

typedef enum {
    TOKEN_NEWLINE,
    TOKEN_SPACE,
    TOKEN_WORD,
    TOKEN_STRING,
    TOKEN_NUM,
    TOKEN_LPAREN,    // '('
    TOKEN_RPAREN,    // ')'
    TOKEN_LBRACE,    // '{'
    TOKEN_RBRACE,    // '}'
    TOKEN_LBRACKET,  // '['
    TOKEN_RBRACKET,  // ']'
    TOKEN_COMMA,     // ','
    TOKEN_ADD,       // '+' OP
    TOKEN_SUB,       // '-' OP
    TOKEN_MUL,       // '*' OP
    TOKEN_DIV,       // '/' OP
    TOKEN_MOD,       // '%' OP
    TOKEN_EQUAL,     // '=' OP
    TOKEN_NEQUAL,    // '!=' OP
    TOKEN_NOT,       // '!' OP
    TOKEN_DOT,       // '.'
    TOKEN_BITLSHIFT, // '<<' OP
    TOKEN_BITRSHIFT, // '>>' OP
    TOKEN_LEQUAL,    // '<=' OP
    TOKEN_GEQUAL,    // '>=' OP
    TOKEN_LESS,      // '<' OP
    TOKEN_GREATER,   // '>' OP
    TOKEN_AND,       // '&&' OP
    TOKEN_OR,        // '||' OP
    TOKEN_BITAND,    // '&' OP
    TOKEN_BITOR,     // '|' OP
    TOKEN_BITXOR,    // '^' OP
    TOKEN_BITNOT,    // '~' OP
    TOKEN_DOUBLECOLON, // '::'
    TOKEN_COLON,     // ':'
    TOKEN_AT,        // '@'
    TOKEN_QUOTE,     // '\''
    TOKEN_SIGIL,     // '$'
    TOKEN_NUMBERSIGN // '#'
} token_type_t;

typedef struct {
    location_t location;
    token_type_t type;
    char* value;
} token_t;

const char* tok2str(token_type_t type);

int is_operator(token_t* token);
token_t* lexer_scan(const char* name, const char* src, size_t* numTokens);
void lexer_print_tokens(token_t* tokens, size_t n);
void lexer_free_buffer(token_t* buffer, size_t n);

#endif
