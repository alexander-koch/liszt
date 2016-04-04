#include "val.h"

val_t* val_num(double num) {
	val_t* v = malloc(sizeof(val_t));
	v->type = VNUM;
	v->num = num;
	return v;
}

val_t* val_sym(char* sym) {
	val_t* v = malloc(sizeof(val_t));
	v->type = VSYM;
	v->sym = malloc(strlen(sym) + 1);
	strcpy(v->sym, sym);
	return v;
}

val_t* val_sexpr() {
	val_t* v = malloc(sizeof(val_t));
	v->type = VSEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

val_t* val_add(val_t* v, val_t* x) {
	v->count++;
	v->cell = realloc(v->cell, sizeof(val_t) * v->count);
	v->cell[v->count-1] = x;
	return v;
}

val_t* val_pop(val_t* v, int i) {
	val_t* x = v->cell[i];
	memmove(&v->cell[i], &v->cell[i+1], sizeof(val_t) * (v->count-i-1));
	v->count--;
	v->cell = realloc(v->cell, sizeof(val_t) * v->count);
	return x;

}

val_t* val_take(val_t* v, int i) {
	val_t* x = val_pop(v, i);
	val_free(v);
	return x;
}

void val_print(val_t* v) {
	if(v == NULL) return;

	switch(v->type) {
		case VNUM: {
			printf("%f", v->num);
			break;
		}
		case VSYM: {
			printf("%s", v->sym);
			break;
		}
		case VQEXPR:
		case VSEXPR: {
			putchar('(');
			for(unsigned i = 0; i < v->count; i++) {
				val_print(v->cell[i]);
				if(i < v->count-1) putchar(' ');
			}
			putchar(')');
			break;
		}
		default: break;
	}
}

void val_println(val_t* v) {
	val_print(v);
	putchar('\n');
}

void val_free(val_t* v) {
	switch(v->type) {
		case VSYM: {
			free(v->sym);
			break;
		}
		case VQEXPR:
		case VSEXPR: {
			for(unsigned i = 0; i < v->count; i++) {
				val_free(v->cell[i]);
			}
			free(v->cell);
			break;
		}
		default: break;
	}
	free(v);
}
