#ifndef SEQUENCE_H
#define SEQUENCE_H 1

#include <stdbool.h>
#include "sonicmaths/sequence.h"

struct smseq_parser {
	void *scanner;
	float root;
	float nbeats;
	float multiple;
	bool loop;
	struct smseq_beatlist *beats;
	void (*error)(const char *);
};

#endif
