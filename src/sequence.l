%option					reentrant
%option					warn nodefault
%option					stack
%option					noyyalloc noyyrealloc noyyfree
%option					noyy_top_state noinput nounput

%x COMMENT QUOTE

%{
#include "sonicmaths/sequence.h"
#include "sequence.h"
#include "sequence-gram.h"
#include <atomickit/malloc.h>

static float notetof(char *note);

int smseq_lex(YYSTYPE *yylval, void *yyscanner);

#define YY_DECL int smseq_lex(YYSTYPE *yylval, void *yyscanner)

/* Avoid unused parameter warning in yy_fatal_error */
#define YY_EXIT_FAILURE ((void) yyscanner, 2)
%}

%%

[ \t\v\f]+				/* eat whitespace */ ;
"(*"					{ yy_push_state(COMMENT, yyscanner); }
<COMMENT>\\.				/* eat escaped character */ ;
<COMMENT>"(*"				{ yy_push_state(COMMENT, yyscanner); }
<COMMENT>"*)"				{ yy_pop_state(yyscanner); }
<COMMENT>.|[\r\n]			/* eat comment content */ ;
\r\n|\n|\r				{ return (int) '\n'; }
<INITIAL><<EOF>>			{ return (int) '\n'; }
[A-Ga-g][#b]?([+-]?[0-9]+)?		{ yylval->num = notetof(yytext);
					  return NOTE; }
[+-]?[0-9]+(\.[0-9]+)?([eE][0-9]+)?	{ yylval->num = atof(yytext);
					  return NUMBER; }
[Xx]					{ return NOTEOFF; }
[Tt][Ii][Mm][Ee]:			{ return TIMEHEADER; }
[Rr][Oo][Oo][Tt]:			{ return ROOTHEADER; }
.					{ return (int) *yytext; }

%%

static inline float notetof(char *note) {
	float tone, octave;
	switch(*note++) {
		case 'a':
		case 'A':
			tone = -3;
			break;
		case 'b':
		case 'B':
			tone = -1;
			break;
		case 'c':
		case 'C':
			tone = 0;
			break;
		case 'd':
		case 'D':
			tone = 2;
			break;
		case 'e':
		case 'E':
			tone = 4;
			break;
		case 'f':
		case 'F':
			tone = 5;
			break;
		case 'g':
		case 'G':
			tone = 7;
			break;
	}
	if(*note == '#') {
		note++;
		tone += 1;
	} else if(*note == 'b') {
		note++;
		tone -= 1;
	}
	if(*note == '\0') {
		octave = 0;
	} else {
		octave = atof(note);
		octave -= 4;
	}
	return tone + 12 * octave;
}

void *yyalloc(size_t bytes,
                     void *yyscanner __attribute__((unused))) {
	void *ret;
	bytes += sizeof(void *);
	ret = amalloc(bytes);
	if(ret == NULL) {
		return NULL;
	}
	*((size_t *) ret) = bytes;
	return ret + sizeof(void *);
}

void *yyrealloc(void *ptr, size_t bytes,
                       void *yyscanner __attribute__((unused))) {
	void *ret;
	bytes += sizeof(void *);
	ptr -= sizeof(void *);
	ret = arealloc(ptr, *((size_t *) ptr), bytes);
	if(ret == NULL) {
		return NULL;
	}
	*((size_t *) ret) = bytes;
	return ret + sizeof(void *);
}

void yyfree(void *ptr,
                   void *yyscanner __attribute__((unused))) {
	ptr -= sizeof(void *);
	afree(ptr, *((size_t *) ptr));
}
