#ifndef SEQUENCE_H
#define SEQUENCE_H 1

struct smseq_parser {
	void *scanner;
	float root;
	float nbeats;
	float multiple;
	struct smseq_beatlist *beats;
	void (*error)(const char *);
};

#endif
