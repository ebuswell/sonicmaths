/*
 * Copyright 2014 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 2.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.
 */

%pure-parser

%{
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <atomickit/malloc.h>
#include "sonicmaths/sequence.h"
#include "sequence.h"

int smseq_lex(void *, void *);

static void smseq_error(struct smseq_parser *seq_info, const char *s);

#define YYLEX_PARAM seq_info->scanner

%}

%parse-param { struct smseq_parser *seq_info }

%union {
	float num;
	bool boolean;
	struct smseq_event event;
	struct smseq_eventlist *eventlist;
	struct smseq_beat beat;
	struct smseq_beatlist *beatlist;
}

%token NUMBER NOTE BOOL
%token NOTEOFF TIMEHEADER ROOTHEADER LOOPHEADER

%%

sequence : emptylines headers emptylines beats emptylines
		{ seq_info->beats = $<beatlist>4; }
         ;

emptyline : '\n'
          ;
emptylines : /* Empty */
           | emptylines emptyline
           ;

headers : /* Empty */
        | headersnonl emptyline
        ;

headersnonl : /* Empty */
            | headersnonl header '\n'
            ;

header : timeheader
       | rootheader
       | loopheader
       ;

timeheader : TIMEHEADER NUMBER '/' NUMBER
		{ seq_info->nbeats = $<num>2;
		  seq_info->multiple = $<num>4 / 4.0f; }
           ;

rootheader : ROOTHEADER NOTE
		{ seq_info->root = $<num>2; }
           ;

loopheader : LOOPHEADER BOOL
	   	{ seq_info->loop = $<boolean>2; }
           ;

beats : /* Empty */
		{ $<beatlist>$ = amalloc(sizeof(struct smseq_beatlist));
		  if($<beatlist>$ == NULL) {
		  	YYERROR_CALL("Failed to allocate memory.");
		  }
		  $<beatlist>$->len = 0; }
      | beats emptylines beat '\n'
		{ struct smseq_beatlist *bl;
		  bl = arealloc($<beatlist>1,
		                sizeof(struct smseq_beatlist)
		                + sizeof(struct smseq_beat)
		                  * $<beatlist>1->len,
		                sizeof(struct smseq_beatlist)
		                + sizeof(struct smseq_beat)
		                  * ($<beatlist>1->len + 1));
		  if(bl == NULL) {
		  	YYERROR_CALL("Failed to allocate memory.");
		  }
		  memcpy(&bl->beats[bl->len], &$<beat>3,
		         sizeof(struct smseq_beat));
		  bl->len += 1; 
		  $<beatlist>$ = bl; }
      ;

beat : numberedsub
     | markedsub
     ;

event : NUMBER
		{ $<event>$.value = $<num>1;
		  $<event>$.ctl = 1; }
      | NOTE
		{ $<event>$.value = $<num>1 - seq_info->root;
		  $<event>$.ctl = 1; }
      | NOTEOFF
		{ $<event>$.ctl = -1; }
      | '.'
		{ $<event>$.ctl = 0; }
      ;

events : /* Empty */
		{ $<eventlist>$ = amalloc(sizeof(struct smseq_eventlist));
		  if($<eventlist>$ == NULL) {
		  	YYERROR_CALL("Failed to allocate memory.");
		  }
		  $<eventlist>$->len = 0; }
       | events event
		{ struct smseq_eventlist *el;
		  el = arealloc($<eventlist>1,
		                sizeof(struct smseq_eventlist)
		                + sizeof(struct smseq_event)
		                  * $<eventlist>1->len,
		                sizeof(struct smseq_eventlist)
		                + sizeof(struct smseq_event)
		                  * ($<eventlist>1->len + 1.0f));
		  if(el == NULL) {
		  	YYERROR_CALL("Failed to allocate memory.");
		  }
		  memcpy(&el->events[el->len], &$<event>2,
		         sizeof(struct smseq_event));
		  el->len += 1;
		  $<eventlist>$ = el; }
       ;

numberedsub : NUMBER events
		{ $<beat>$.sequence = $<num>1 - 1.0f;
		  if($<beat>$.sequence < 0
		     || $<beat>$.sequence > seq_info->nbeats) {
		  	YYERROR_CALL("Number out of specified beat range");
		  }
		  $<beat>$.events = $<eventlist>2; }
            ;

markedsub : "." events
		{ $<beat>$.sequence = -1.0f;
		  $<beat>$.events = $<eventlist>2; }
          ;

%%

int smseq_get_column(void *yyscanner);
int smseq_get_lineno(void *yyscanner);

static void smseq_error(struct smseq_parser *seq_info, const char *s) {
	char buffer[4096];
	snprintf(buffer, 4096, "%d:%d: %s\n",
	         smseq_get_lineno(seq_info->scanner),
	         smseq_get_column(seq_info->scanner),
	         s);
	seq_info->error(buffer);
}
