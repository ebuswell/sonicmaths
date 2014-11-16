%pure-parser

%{
#include <stddef.h>
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
	struct smseq_event event;
	struct smseq_eventlist *eventlist;
	struct smseq_beat beat;
	struct smseq_beatlist *beatlist;
}

%token NUMBER NOTE
%token NOTEOFF TIMEHEADER ROOTHEADER

%%

sequence : headers beats
		{ seq_info->beats = $<beatlist>2; }
         ;

headers : /* Empty */
        | headersnonl '\n'
        ;

headersnonl : /* Empty */
            | headersnonl header '\n'
            ;

header : timeheader
       | rootheader
       ;

timeheader : TIMEHEADER NUMBER '/' NUMBER
		{ seq_info->nbeats = $<num>2;
		  seq_info->multiple = $<num>4 / 4.0f; }
           ;

rootheader : ROOTHEADER NOTE
		{ seq_info->root = $<num>2; }
           ;

beats : /* Empty */
		{ $<beatlist>$ = amalloc(sizeof(struct smseq_beatlist));
		  if($<beatlist>$ == NULL) {
		  	YYERROR_CALL("Failed to allocate memory.");
		  }
		  $<beatlist>$->len = 0; }
      | beats beat '\n'
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
		  memcpy(&bl->beats[bl->len], &$<beat>2,
		         sizeof(struct smseq_beat));
		  bl->len += 1.0f; }
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
		  el->len += 1.0f; }
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

static void smseq_error(struct smseq_parser *seq_info, const char *s) {
	seq_info->error(s);
}
