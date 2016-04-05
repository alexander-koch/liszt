#ifndef parser_h
#define parser_h

#include <stdlib.h>
#include <lexer/lexer.h>
#include <core/val.h>

val_t* parse_buffer(token_t* tokens, size_t numTokens);

#endif
