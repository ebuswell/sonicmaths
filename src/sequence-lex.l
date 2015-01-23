/*
 * Copyright 2015 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 2.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.
 */

%option					reentrant
%option					warn nodefault
%option					stack
%option					noyy_top_state noinput nounput
%option					noyywrap
%option					extra-type="bool"

%x COMMENT QUOTE

%{
#include <stdbool.h>
#include "sonicmaths/sequence.h"
#include "sequence.h"
#include "sequence-gram.h"

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
<INITIAL><<EOF>>			{ if(yyextra) {
					  	yyextra = false;
					  	return (int) '\n';
					  } else {
					  	return 0;
					  } }
[A-Ga-g][#b]?([+-]?[0-9]+)?		{ yylval->num = notetof(yytext);
					  return NOTE; }
[+-]?[0-9]+(\.[0-9]+)?([eE][0-9]+)?	{ yylval->num = atof(yytext);
					  return NUMBER; }
[Xx]					{ return NOTEOFF; }
[Tt][Ii][Mm][Ee]:			{ return TIMEHEADER; }
[Rr][Oo][Oo][Tt]:			{ return ROOTHEADER; }
[Ll][Oo][Oo][Pp]:			{ return LOOPHEADER; }
[Tt][Rr][Uu][Ee]			{ yylval->boolean = true; return BOOL; }
[Ff][Aa][Ll][Ss][Ee]			{ yylval->boolean = false; return BOOL; }
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
		default:
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
