#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <livec.h>
#include <atomickit/atomic.h>
#include <atomickit/malloc.h>
#include <jack/jack.h>
#include <sonicmaths/predefs.h>
#include <sonicmaths/math.h>
#include <sonicmaths/clock.h>
#include <sonicmaths/key.h>
#include <sonicmaths/sequence.h>

struct state {
	struct arcp_region;
	jack_client_t *client;
	jack_port_t *out_port;
	struct smclock *clock;
	arcp_t seq;
	struct smasynth *asynth;
};

int process(jack_nframes_t nframes, void *arg __attribute__((unused))) {
	float *out;
	unsigned int i;
	int j, nchannels;
	float y, note, ctl, clock;
	enum smasynth_type osc1, osc2;
	float interphase1, interphase2, amp1, amp2, detune1, detune2,
	      attack, decay, sustain, release,
	      filtermin, filtermax, resonance,
	      lforate, lfopitch, lfofilter;
	struct state *cstate;
	struct smseq *seq;
	float sample_rate;

	cstate = (struct state *) arcp_load(&state);
	seq = (struct smseq *) arcp_load(&cstate->seq);
	sample_rate = jack_get_sample_rate(cstate->client);
	out = jack_port_get_buffer(cstate->out_port, nframes);

	nchannels = seq->nchannels;
	smasynth_redim(cstate->asynth, nchannels);

	osc1 = SMSAW;
	interphase1 = 0;
	amp1 = 1;
	detune1 = 0;

	osc2 = SMSQUARE;
	interphase2 = 0.25;
	amp2 = 0.25;
	detune2 = 0.1;

	attack = smaths_normtime(sample_rate, 0.15);
	decay = smaths_normtime(sample_rate, 0.5);
	sustain = powf(2, -0.5);
	release = smaths_normtime(sample_rate, 0.15);

	filtermin = 1;
	filtermax = 7;
	resonance = 0.5;

	lforate = smaths_normfreq(sample_rate, 5);

	lfopitch = 0.1;
	lfofilter = 0;

	for(i = 0; i < nframes; i++) {
		out[i] = 0.0f;
		clock = smclock(cstate->clock,
		                smaths_normfreq(sample_rate,
				                100.0f/60.0f));
		for(j = 0; j < nchannels; j++) {
			note = smseq(seq, j, clock, &ctl);
			note = smkey(SMKEY_EQUAL,
		             	     smaths_normfreq(sample_rate, SMKEY_C),
			     	     note);
			y = smasynth(cstate->asynth, j, note, ctl,
			             osc1, interphase1, amp1, detune1,
			             osc2, interphase2, amp2, detune2,
			             attack, decay, sustain, release,
			             filtermin, filtermax, resonance,
			             lforate, lfopitch, lfofilter);
			out[i] += 0.5f * y;
		}
	}
	arcp_release(seq);
	arcp_release(cstate);
	return 0;
}

void init_state() {
	struct state *cstate = amalloc(sizeof(struct state));
	arcp_region_init(cstate, NULL);
	arcp_store(&state, cstate);
	arcp_release(cstate);
}

void init_jack() {
	struct state *cstate;

	cstate = (struct state *) arcp_load(&state);

	cstate->client = jack_client_open("daisybell_c", JackNullOption, NULL);
	cstate->out_port
		= jack_port_register(cstate->client, "out",
		                     JACK_DEFAULT_AUDIO_TYPE,
		                     JackPortIsOutput|JackPortIsTerminal, 0);

	jack_set_process_callback(cstate->client,
	                          (JackProcessCallback)
	                          AUTOLINK_CREATE(process,
	                                          "(%u, %p) -> %i"),
	                          NULL);
	jack_activate(cstate->client);

	sleep(1);
	jack_connect(cstate->client,
	             jack_port_name(cstate->out_port), "system:playback_1");
	jack_connect(cstate->client,
	             jack_port_name(cstate->out_port), "system:playback_2");
}

void sequence_file_err(const char *error) {
	printf("daisybell.seq:%s", error);
}

void init_synths() {
	struct state *cstate = (struct state *) arcp_load(&state);
	cstate->asynth = smasynth_create();
	cstate->clock = smclock_create();
	smclock_set_time(cstate->clock,  -5.0f);
	arcp_init(&cstate->seq, NULL);
	smseq_start_watch("daisybell.seq", &cstate->seq, sequence_file_err);
	sleep(1);
}

int main(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
	// init_state();
	// init_synths();
	// init_jack();
	return 0;
}
