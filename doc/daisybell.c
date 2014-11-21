#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <livec.h>
#include <atomickit/atomic.h>
#include <atomickit/malloc.h>
#include <jack/jack.h>
#include <sonicmaths/synth.h>
#include <sonicmaths/impulse-train.h>
#include <sonicmaths/sine.h>
#include <sonicmaths/integrator.h>
#include <sonicmaths/math.h>
#include <sonicmaths/clock.h>
#include <sonicmaths/key.h>
#include <sonicmaths/sequence.h>
#include <sonicmaths/envelope-generator.h>
#include <sonicmaths/second-order.h>
#include <sonicmaths/bandpass.h>
#include <sonicmaths/lowpass.h>
#include <sonicmaths/highpass.h>
#include <sonicmaths/notch.h>

struct state {
	struct arcp_region;
	jack_client_t *client;
	jack_port_t *out_port;
	struct smclock *clock;
	struct smsynth *synth;
	struct smsynth *lfo;
	struct smintg *intg[2];
	arcp_t seq;
	struct smenvg *aenvg;
	struct smenvg *fenvg;
	struct sm2order *filter;
};

int process(jack_nframes_t nframes, void *arg __attribute__((unused))) {
	float *out;
	unsigned int i;
	int j, nchannels;
	float y, note, ctl, clock;
	struct state *cstate;
	struct smseq *seq;
	float sample_freq;

	cstate = (struct state *) arcp_load(&state);
	seq = (struct smseq *) arcp_load(&cstate->seq);

	sample_freq = jack_get_sample_rate(cstate->client);

	out = jack_port_get_buffer(cstate->out_port, nframes);

	nchannels = seq->nchannels;
	smsynth_redim(cstate->synth, nchannels);
	smsynth_redim(cstate->lfo, nchannels);
	smintg_redim(cstate->intg[0], nchannels);
	smintg_redim(cstate->intg[1], nchannels);
	smenvg_redim(cstate->aenvg, nchannels);
	smenvg_redim(cstate->fenvg, nchannels);
	sm2order_redim(cstate->filter, nchannels);

	for(i = 0; i < nframes; i++) {
		out[i] = 0.0f;
		clock = smclock(cstate->clock,
		                smaths_normfreq(sample_freq,
				                100.0f/60.0f));
		for(j = 0; j < nchannels; j++) {
			note = smseq(seq,
			     	     j,
				     clock,
			     	     &ctl);
			note = smkey(SMKEY_EQUAL,
		             	     smaths_normfreq(sample_freq,
					     	     SMKEY_C),
			     	     note);
			y = smitrain(cstate->synth, j, note, 0.0f);
			y = note * smintg(cstate->intg[0], j, y);
			y = smlowpass(cstate->filter, j,
			              y,
			              smenvg(cstate->fenvg, j, true, ctl,
			                     smaths_normtime(sample_freq, 0.25),
			                     note * 16,
			                     smaths_normtime(sample_freq, 1.0),
			                     note * 3,
			                     smaths_normtime(sample_freq, 1.0),
			                     note * 3),
			              1.0f);
			y = y * smenvg(cstate->aenvg, j, false, ctl,
			               smaths_normtime(sample_freq, 0.25),
			               1.0f,
			               smaths_normtime(sample_freq, 0.5),
			               0.7,
			               smaths_normtime(sample_freq, 0.15),
			               0.0f);
			y *= 0.5 * powf(1.3f, smsine(cstate->lfo, j, smaths_normfreq(sample_freq, 4.6f), 0.0f));
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
	size_t i, j;
	struct state *cstate = (struct state *) arcp_load(&state);
	struct smseq *seq;
	cstate->synth = smsynth_create();
	cstate->clock = smclock_create();
	smclock_set_time(cstate->clock,  -6.0f);
	for(i = 0; i < 2; i++) {
		cstate->intg[i] = smintg_create();
	}
	arcp_init(&cstate->seq, NULL);
	smseq_start_watch("daisybell.seq", &cstate->seq, sequence_file_err);
	cstate->aenvg = smenvg_create();
	cstate->fenvg = smenvg_create();
	cstate->filter = sm2order_create();
	cstate->lfo = smsynth_create();
}

int main(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
	init_state();
	init_synths();
	init_jack();
	return 0;
}
