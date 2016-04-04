#ifndef val_h
#define val_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum val_type_t {
	VNUM,
	VSYM,
	VSEXPR,
	VQEXPR
};

typedef struct val_t {
	int type;
	double num;
	char* sym;

	unsigned count;
	struct val_t** cell;
} val_t;

val_t* val_num(double num);
val_t* val_sym(char* sym);
val_t* val_sexpr();

val_t* val_add(val_t* v, val_t* x);
val_t* val_pop(val_t* v, int i);
val_t* val_take(val_t* v, int i);

void val_print(val_t* v);
void val_println(val_t* v);
void val_free(val_t* v);

#endif
