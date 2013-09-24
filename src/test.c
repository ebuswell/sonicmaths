/*
 * test.c
 * 
 * Copyright 2013 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, version 2.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Sonic Maths.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <atomickit/atomic.h>
#include <graphline.h>
#include "sonicmaths/jbridge.h"
#include "sonicmaths/sine.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/instrument.h"
#include "sonicmaths/envelope-generator.h"
/* #include "sonicmaths/clock.h" */
/* #include "sonicmaths/scheduler.h" */
#include "sonicmaths/key.h"
/* #include "sonicmaths/jmidi.h" */
#include "sonicmaths/mixer.h"
#include "sonicmaths/noise.h"
#include "sonicmaths/distortion.h"
#include "sonicmaths/impulse-train.h"
#include "sonicmaths/integrator.h"
#include "sonicmaths/portamento.h"
#include "sonicmaths/lowpass.h"
#include "sonicmaths/bandpass.h"
#include "sonicmaths/highpass.h"
#include "sonicmaths/notch.h"
#include "sonicmaths/modulator.h"
/* #include "sonicmaths/dsf.h" */
#include "sonicmaths/sawtooth.h"
#include "sonicmaths/parabola.h"
#include "sonicmaths/square.h"
#include "sonicmaths/triangle.h"
#include "sonicmaths/midi.h"

#define CHECKING(function)			\
    printf("Checking " #function "...")

#define CHECKING_S(string)			\
    printf("Checking " string "...")

#define OK()					\
    printf("OK\n")

#define CHECK_NULL(obj)				\
    do {					\
	if(obj == NULL) {			\
	    error(1, errno, "Error");		\
	}					\
    } while(0)

#define CHECK_R()				\
    do {					\
	if(r != 0) {				\
	    error(1, errno, "Error");		\
	}					\
    } while(0)

struct nullnode {
    struct gln_node;
    struct gln_socket *in;
    struct gln_socket *out;
};

static int nullnode_f(struct nullnode *nn) {
    struct smaths_buffer *in_buffer;
    int r, i, j;
    r = gln_get_buffers(1, nn->in, &in_buffer);
    if(r != 0) {
	return r;
    }
    gln_set_buffer(nn->out, in_buffer);

    if(in_buffer == NULL) {
	printf("NULL\n");
	return 0;
    }

    for(j = 0; j < in_buffer->nchannels; j++) {
	for(i = 0; i < in_buffer->nframes; i++) {
	    printf("[%d, %d] = %f\n", i, j, (double) in_buffer->data[i*in_buffer->nchannels + j]);
	}
    }

    return 0;
}

static int nullnode_init(struct nullnode *nn, struct smaths_graph *graph, void (*destroy)(struct nullnode *)) {
    int r;
    r = gln_node_init(nn, graph, (gln_process_fp_t) nullnode_f, (void (*)(struct gln_node *)) destroy);
    if(r != 0) {
	goto undo0;
    }
    nn->in = gln_socket_create(nn, GLNS_INPUT);
    if(nn->in == NULL) {
	r = -1;
	goto undo1;
    }
    nn->out = gln_socket_create(nn, GLNS_OUTPUT);
    if(nn->in == NULL) {
	r = -1;
	goto undo2;
    }

    return 0;

undo2:
    arcp_release(nn->in);
undo1:
    gln_node_destroy(nn);
undo0:
    return r;
}

static void nullnode_destroy(struct nullnode *nn) {
    arcp_release(nn->out);
    arcp_release(nn->in);
    gln_node_destroy(nn);
}

static void __nullnode_destroy(struct nullnode *nn) {
    nullnode_destroy(nn);
    afree(nn, sizeof(struct nullnode));
}

static struct nullnode *nullnode_create(struct smaths_graph *graph) {
    int r;
    struct nullnode *ret;

    ret = amalloc(sizeof(struct nullnode));
    if(ret == NULL) {
	return NULL;
    }

    r = nullnode_init(ret, graph, __nullnode_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct nullnode));
	return NULL;
    }

    return ret;
}

/* int play_first(struct smaths_inst *inst) { */
/*     smaths_inst_play(inst, 0.0f); */
/*     return 0; */
/* } */

/* int play_second(struct smaths_inst *inst) { */
/*     smaths_inst_play(inst, 2.0f); */
/*     return 0; */
/* } */

/* int play_third(struct smaths_inst *inst) { */
/*     smaths_inst_play(inst, 4.0f); */
/*     return 0; */
/* } */

/* int stop_it(struct smaths_inst *inst) { */
/*     smaths_inst_stop(inst); */
/*     return 0; */
/* } */

int main(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
    int r;

    setbuf(stdout, NULL);

    struct timespec sleeptime = { 0, 0 };

    jack_status_t status;
    CHECKING(smaths_jbridge_create);
    struct smaths_jbridge *bridge;
    bridge = smaths_jbridge_create("smaths_jbridge", 0, &status, NULL);
    if(bridge == NULL) {
	if(status & JackServerFailed) {
	    error(0, 0, "Unable to connect to the JACK server");
	}
	if(status & JackServerError) {
	    error(0, 0, "Communication error with the JACK server");
	}
	if(status & JackNoSuchClient) {
	    error(0, 0, "Requested client does not exist");
	}
	if(status & JackLoadFailure) {
	    error(0, 0, "Unable to load internal client");
	}
	if(status & JackInitFailure) {
	    error(0, 0, "Unable to initialize client");
	}
	if(status & JackShmFailure) {
	    error(0, 0, "Unable to access shared memory");
	}
	if(status & JackVersionError) {
	    error(0, 0, "Client's protocol version does not match");
	}
	if(status & JackBackendError) {
	    error(0, 0, "Backend error");
	}
	if(status & JackClientZombie) {
	    error(0, 0, "Client zombie");
	}
	if(status & JackInvalidOption) {
	    error(0, 0, "The operation contained an invalid or unsupported option");
	}
	if(status & JackNameNotUnique) {
	    error(0, 0, "The desired client name was not unique");
	}
	error(1, 0, "Overall operation failed: %d", status);
    }
    OK();

    CHECKING(smaths_jbridge_create_socket);
    struct gln_socket *to_jack_socket;
    jack_port_t *to_jack_port;
    r = smaths_jbridge_create_socket(bridge, GLNS_OUTPUT, &to_jack_socket, &to_jack_port);
    CHECK_R();

    struct gln_socket *from_jack_socket;
    jack_port_t *from_jack_port;
    r = smaths_jbridge_create_socket(bridge, GLNS_INPUT, &from_jack_socket, &from_jack_port);
    CHECK_R();

    r = jack_connect(bridge->client, jack_port_name(to_jack_port), "system:playback_1");
    CHECK_R();
    r = jack_connect(bridge->client, jack_port_name(to_jack_port), "system:playback_2");
    CHECK_R();

    r = gln_socket_connect(from_jack_socket, to_jack_socket);
    CHECK_R();

    sleeptime.tv_nsec = 250000000;
    nanosleep(&sleeptime, NULL);
    OK();

    /* printf("Pausing to allow jack configuration, press enter to continue..."); */
    /* char buf[2048]; */
    /* fgets(buf, 2048, stdin); */
    /* OK(); */

    CHECKING(nullnode_create);
    struct nullnode *nn = nullnode_create(bridge);
    CHECK_NULL(nn);
    OK();

    CHECKING(smaths_sine_create);
    struct smaths_sine *sine;
    sine = smaths_sine_create(bridge);
    CHECK_NULL(sine);
    r = gln_socket_connect(sine->out, to_jack_socket);
    CHECK_R();
    OK();

    CHECKING(smaths_parameter_set);
    smaths_parameter_set(sine->freq, smaths_graph_normalized_frequency(bridge, SMATHS_C));
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(sine->amp, 0);
    OK();

    CHECKING(smaths_inst_create);

    struct smaths_inst *inst;
    inst = smaths_inst_create(bridge);
    CHECK_NULL(inst);
    r = gln_socket_connect(inst->out, sine->freq);
    CHECK_R();
    smaths_inst_set_nchannels(inst, 2);
    OK();

    CHECKING(smaths_inst_play);
    r = smaths_inst_play(inst, smaths_graph_normalized_frequency(bridge, SMATHS_C));
    CHECK_R();
    r = smaths_inst_play(inst, smaths_graph_normalized_frequency(bridge, 2*SMATHS_C));
    CHECK_R();
    smaths_parameter_set(sine->amp, 0.5f);
    nanosleep(&sleeptime, NULL);
    CHECKING(smaths_inst_stop);
    smaths_parameter_set(sine->amp, 0);
    r = smaths_inst_stop(inst, smaths_graph_normalized_frequency(bridge, SMATHS_C));
    CHECK_R();
    r = smaths_inst_stop(inst, smaths_graph_normalized_frequency(bridge, 2*SMATHS_C));
    CHECK_R();
    OK();

    CHECKING(smaths_mix_create);
    struct smaths_mix *mix = smaths_mix_create(bridge);
    CHECK_NULL(mix);
    r = gln_socket_connect(mix->out, to_jack_socket);
    CHECK_R();
    OK();

    CHECKING(smaths_mix_input_create);
    struct smaths_parameter *mix_in1;
    struct smaths_parameter *mix_in1_amp;
    r = smaths_mix_input_create(mix, &mix_in1, &mix_in1_amp);
    CHECK_R();
    struct smaths_parameter *tmp_mix_in2;
    struct smaths_parameter *tmp_mix_in2_amp;
    r = smaths_mix_input_create(mix, &tmp_mix_in2, &tmp_mix_in2_amp);
    CHECK_R();
    smaths_parameter_set(mix_in1_amp, 0.5f);
    smaths_parameter_set(tmp_mix_in2_amp, 0.5f);
    struct smaths_sine *tmpsine;
    tmpsine = smaths_sine_create(bridge);
    CHECK_NULL(tmpsine);
    r = gln_socket_connect(tmpsine->out, tmp_mix_in2);
    CHECK_R();
    r = gln_socket_connect(sine->out, mix_in1);
    CHECK_R();
    smaths_parameter_set(tmpsine->freq, smaths_graph_normalized_frequency(bridge, 2*SMATHS_G));
    smaths_parameter_set(sine->amp, 0.5f);
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(tmpsine->amp, 0);
    smaths_parameter_set(sine->amp, 0);
    arcp_release(tmp_mix_in2);
    arcp_release(tmp_mix_in2_amp);
    arcp_release(tmpsine);
    OK();

    CHECKING(smaths_envg_create);
    struct smaths_envg *envg;
    envg = smaths_envg_create(bridge);
    CHECK_R();
    smaths_parameter_set(envg->attack_t, smaths_graph_normalized_time(bridge, 0.125f));
    smaths_parameter_set(envg->decay_t, smaths_graph_normalized_time(bridge, 0.25f));
    smaths_parameter_set(envg->sustain_a, expf((float) (-M_PI * 0.2)));
    smaths_parameter_set(envg->release_t, smaths_graph_normalized_time(bridge, 0.125f));
    r = gln_socket_connect(sine->amp, envg->out);
    CHECK_R();
    r = gln_socket_connect(envg->ctl, inst->ctl);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, smaths_graph_normalized_frequency(bridge, SMATHS_C));
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, smaths_graph_normalized_frequency(bridge, SMATHS_G));
    CHECK_R();
    sleep(1);
    r = smaths_inst_stop(inst, smaths_graph_normalized_frequency(bridge, SMATHS_C));
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, smaths_graph_normalized_frequency(bridge, SMATHS_G));
    CHECK_R();
    sleep(1);
    OK();

    CHECKING(smaths_key_create);
    struct smaths_key *key;
    key = smaths_key_create(bridge);
    CHECK_NULL(key);
    r = gln_socket_connect(key->note, inst->out);
    CHECK_R();
    r = gln_socket_connect(sine->freq, key->freq);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING(smaths_key_set_tuning);
    smaths_key_set_tuning(key, SMATHS_MAJOR_TUNING);
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 2);
    CHECK_R();
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 2);
    CHECK_R();
    sleep(1);
    smaths_key_set_tuning(key, SMATHS_EQUAL_TUNING);
    OK();

    CHECKING(smaths_key_note2freq);
    r = gln_socket_disconnect(sine->freq);
    CHECK_R();
    smaths_parameter_set(sine->freq, smaths_key_note2freq(key, 5));
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    sleep(1);
    r = gln_socket_connect(sine->freq, key->freq);
    CHECK_R();
    OK();

    CHECKING(smaths_porta_create);
    struct smaths_porta *porta = smaths_porta_create(bridge);
    CHECK_NULL(porta);
    smaths_parameter_set(porta->lag, 0);
    smaths_parameter_set(porta->in, 0);
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->lag, smaths_graph_normalized_time(bridge, 0.5f) / 7);
    r = gln_socket_connect(porta->out, key->note);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->in, 7);
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    sleep(1);
    r = gln_socket_connect(key->note, inst->out);
    CHECK_R();
    OK();

    /* /\* CHECKING(smaths_clock_init); *\/ */
    /* /\* struct smaths_clock clock; *\/ */
    /* /\* r = smaths_clock_init(&clock, &bridge.graph); *\/ */
    /* /\* CHECK_R(); *\/ */
    /* /\* smaths_parameter_set(&clock.rate, smaths_graph_normalized_rate(&bridge.graph, 144.0f/60.0f)); *\/ */
    /* /\* OK(); *\/ */

    /* /\* CHECKING(smaths_sched_init); *\/ */
    /* /\* struct smaths_sched sched; *\/ */
    /* /\* r = smaths_sched_init(&sched, &bridge.graph); *\/ */
    /* /\* CHECK_R(); *\/ */
    /* /\* OK(); *\/ */

    /* /\* CHECKING(smaths_sched_schedule); *\/ */
    /* /\* struct smaths_sched_event first_event = { 1.0f, (smaths_event_fp_t) play_first, &inst }; *\/ */
    /* /\* struct smaths_sched_event second_event = { 2.0f, (smaths_event_fp_t) play_second, &inst }; *\/ */
    /* /\* struct smaths_sched_event third_event = { 3.0f, (smaths_event_fp_t) play_third, &inst }; *\/ */
    /* /\* struct smaths_sched_event stop_event = { 3.85f, (smaths_event_fp_t) stop_it, &inst }; *\/ */
    /* /\* r = smaths_sched_schedule(&sched, &stop_event); *\/ */
    /* /\* CHECK_R(); *\/ */
    /* /\* r = smaths_sched_schedule(&sched, &first_event); *\/ */
    /* /\* CHECK_R(); *\/ */
    /* /\* r = smaths_sched_schedule(&sched, &third_event); *\/ */
    /* /\* CHECK_R(); *\/ */
    /* /\* r = smaths_sched_schedule(&sched, &second_event); *\/ */
    /* /\* CHECK_R(); *\/ */
    /* /\* r = gln_socket_connect(&sched.clock, &clock.clock); *\/ */
    /* /\* CHECK_R(); *\/ */
    /* /\* sleep(5.0 * 60.0/144.0); *\/ */
    /* /\* OK(); *\/ */

    /* /\* CHECKING(smaths_sched_cancel); *\/ */
    /* /\* first_event.time = 6.0f; *\/ */
    /* /\* second_event.time = 7.0f; *\/ */
    /* /\* third_event.time = 8.0f; *\/ */
    /* /\* stop_event.time = 8.85f; *\/ */
    /* /\* r = smaths_sched_schedule(&sched, &stop_event); *\/ */
    /* /\* CHECK_R(); *\/ */
    /* /\* r = smaths_sched_schedule(&sched, &first_event); *\/ */
    /* /\* CHECK_R(); *\/ */
    /* /\* r = smaths_sched_schedule(&sched, &third_event); *\/ */
    /* /\* CHECK_R(); *\/ */
    /* /\* r = smaths_sched_schedule(&sched, &second_event); *\/ */
    /* /\* CHECK_R(); *\/ */
    /* /\* smaths_sched_cancel(&sched, &third_event); *\/ */
    /* /\* sleep(6); *\/ */
    /* /\* OK(); *\/ */

    CHECKING(smaths_noise_init);
    struct smaths_noise *noise = smaths_noise_create(bridge);
    CHECK_NULL(noise);
    OK();

    CHECKING_S("smaths_noise: uniform");
    smaths_noise_set_kind(noise, SMATHSN_UNIFORM);
    r = gln_socket_connect(mix_in1, noise->out);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING_S("smaths_noise: gaussian");
    smaths_noise_set_kind(noise, SMATHSN_GAUSSIAN);
    sleep(1);
    OK();

    /* CHECKING(smaths_dsf_init); */
    /* struct smaths_dsf dsf; */
    /* r = smaths_dsf_init(&dsf, &bridge.graph); */
    /* CHECK_R(); */
    /* smaths_parameter_set(&dsf.bright, 0.8f); */
    /* r = smaths_parameter_connect(&mix_in1, &dsf.out); */
    /* CHECK_R(); */
    /* r = smaths_parameter_connect(&dsf.amp, &envg.out); */
    /* CHECK_R(); */
    /* r = smaths_parameter_connect(&dsf.freq, &key.freq); */
    /* CHECK_R(); */
    /* smaths_inst_play(&inst, 0.0f); */
    /* sleep(1); */
    /* smaths_inst_play(&inst, 2.0f); */
    /* sleep(1); */
    /* smaths_inst_stop(&inst); */
    /* sleep(1); */
    /* OK(); */

    CHECKING(smaths_itrain_create);
    struct smaths_itrain *itrain = smaths_itrain_create(bridge);
    CHECK_NULL(itrain);
    atomic_store_explicit(&itrain->scale, true, memory_order_release);
    r = gln_socket_connect(mix_in1, itrain->out);
    CHECK_R();
    r = gln_socket_connect(itrain->amp, envg->out);
    CHECK_R();
    r = gln_socket_connect(itrain->freq, key->freq);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING(smaths_saw_create);
    struct smaths_saw *saw = smaths_saw_create(bridge);
    CHECK_NULL(saw);
    atomic_store_explicit(&saw->scale, true, memory_order_release);
    r = gln_socket_connect(mix_in1, saw->out);
    CHECK_R();
    r = gln_socket_connect(saw->amp, envg->out);
    CHECK_R();
    r = gln_socket_connect(saw->freq, key->freq);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING(smaths_bola_create);
    struct smaths_bola *bola = smaths_bola_create(bridge);
    CHECK_NULL(bola);
    atomic_store_explicit(&bola->scale, true, memory_order_release);
    r = gln_socket_connect(mix_in1, bola->out);
    CHECK_R();
    r = gln_socket_connect(bola->amp, envg->out);
    CHECK_R();
    r = gln_socket_connect(bola->freq, key->freq);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING(smaths_square_create);
    struct smaths_square *square = smaths_square_create(bridge);
    CHECK_NULL(square);
    atomic_store_explicit(&square->scale, true, memory_order_release);
    smaths_parameter_set(porta->lag, 0);
    smaths_parameter_set(porta->in, -0.5f);
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->lag, smaths_graph_normalized_time(bridge, 0.5f));
    r = gln_socket_connect(square->skew, porta->out);
    CHECK_R();
    r = gln_socket_connect(mix_in1, square->out);
    CHECK_R();
    r = gln_socket_connect(square->amp, envg->out);
    CHECK_R();
    r = gln_socket_connect(square->freq, key->freq);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->in, 0.5f);
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING(smaths_triangle_create);
    struct smaths_triangle *triangle = smaths_triangle_create(bridge);
    CHECK_NULL(triangle);
    atomic_store_explicit(&triangle->scale, true, memory_order_release);
    smaths_parameter_set(porta->lag, 0);
    smaths_parameter_set(porta->in, -0.5f);
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->lag, smaths_graph_normalized_time(bridge, 0.5f));
    r = gln_socket_connect(triangle->skew, porta->out);
    CHECK_R();
    r = gln_socket_connect(mix_in1, triangle->out);
    CHECK_R();
    r = gln_socket_connect(triangle->amp, envg->out);
    CHECK_R();
    r = gln_socket_connect(triangle->freq, key->freq);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->in, 0.5f);
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    r = gln_socket_connect(saw->amp, envg->out);
    CHECK_R();
    r = gln_socket_connect(saw->freq, key->freq);
    CHECK_R();

    CHECKING(smaths_distort_create);
    struct smaths_distort *distort = smaths_distort_create(bridge);
    CHECK_NULL(distort);
    r = gln_socket_connect(mix_in1, distort->out);
    CHECK_R();
    r = gln_socket_connect(distort->in, saw->out);
    CHECK_R();
    smaths_parameter_set(distort->gain, expf(1.0f));
    OK();

    CHECKING_S("smaths_distort: exponential");
    smaths_distort_set_kind(distort, SMATHSD_EXP);
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING_S("smaths_distort: hyperbolic");
    smaths_distort_set_kind(distort, SMATHSD_HYP);
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING_S("smaths_distort: arctangent");
    smaths_distort_set_kind(distort, SMATHSD_HYP);
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    /* CHECKING_S("smaths_distort: tube simulation"); */
    /* atomic_set(&distort.kind, SMATHS_TUBE); */
    /* smaths_inst_play(&inst, 0.0f); */
    /* sleep(1); */
    /* smaths_inst_play(&inst, 4.0f); */
    /* sleep(1); */
    /* smaths_inst_stop(&inst); */
    /* sleep(1); */
    /* OK(); */

    smaths_parameter_set(mix_in1_amp, 0.0625f);

    CHECKING(smaths_integrator_create);
    struct smaths_integrator *integrator = smaths_integrator_create(bridge);
    CHECK_NULL(integrator);
    r = gln_socket_connect(mix_in1, integrator->out);
    CHECK_R();
    r = gln_socket_connect(integrator->in, saw->out);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    smaths_parameter_set(mix_in1_amp, 0.5f);

    CHECKING(smaths_lowpass_create);
    struct smaths_lowpass *lowpass = smaths_lowpass_create(bridge);
    CHECK_NULL(lowpass);
    smaths_parameter_set(porta->lag, 0);
    smaths_parameter_set(porta->in, 0.25);
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->lag, smaths_graph_normalized_time(bridge, 0.5f/(0.25f - smaths_key_note2freq(key, 0))));
    r = gln_socket_connect(lowpass->freq, porta->out);
    CHECK_R();
    r = gln_socket_connect(mix_in1, lowpass->out);
    CHECK_R();
    r = gln_socket_connect(lowpass->in, saw->out);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->in, smaths_key_note2freq(key, 0));
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING(smaths_bandpass_create);
    struct smaths_bandpass *bandpass = smaths_bandpass_create(bridge);
    CHECK_NULL(bandpass);
    smaths_parameter_set(porta->lag, 0);
    smaths_parameter_set(porta->in, smaths_key_note2freq(key, 0));
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->lag, smaths_graph_normalized_time(bridge, 0.5f/(0.25f - smaths_key_note2freq(key, 0))));
    r = gln_socket_connect(bandpass->freq, porta->out);
    CHECK_R();
    r = gln_socket_connect(mix_in1, bandpass->out);
    CHECK_R();
    r = gln_socket_connect(bandpass->in, saw->out);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->in, 0.25f);
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING(smaths_highpass_create);
    struct smaths_highpass *highpass = smaths_highpass_create(bridge);
    CHECK_NULL(highpass);
    smaths_parameter_set(porta->lag, 0);
    smaths_parameter_set(porta->in, smaths_key_note2freq(key, 0));
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->lag, smaths_graph_normalized_time(bridge, 0.25f/(0.5f - smaths_key_note2freq(key, 0))));
    r = gln_socket_connect(highpass->freq, porta->out);
    CHECK_R();
    r = gln_socket_connect(mix_in1, highpass->out);
    CHECK_R();
    r = gln_socket_connect(highpass->in, saw->out);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->in, 0.25f);
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING(smaths_notch_create);
    struct smaths_notch *notch = smaths_notch_create(bridge);
    CHECK_NULL(notch);
    smaths_parameter_set(porta->lag, 0);
    smaths_parameter_set(porta->in, 0.25);
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->lag, smaths_graph_normalized_time(bridge, 0.5f/(0.25f - smaths_key_note2freq(key, 0))));
    r = gln_socket_connect(notch->freq, porta->out);
    CHECK_R();
    r = gln_socket_connect(mix_in1, notch->out);
    CHECK_R();
    r = gln_socket_connect(notch->in, saw->out);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    smaths_parameter_set(porta->in, smaths_key_note2freq(key, 0));
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING(smaths_modu_create);
    struct smaths_modu *modu = smaths_modu_create(bridge);
    CHECK_NULL(modu);
    tmpsine = smaths_sine_create(bridge);
    CHECK_NULL(tmpsine);
    smaths_parameter_set(tmpsine->freq, smaths_graph_normalized_frequency(bridge, SMATHS_E));
    r = gln_socket_connect(mix_in1, modu->out);
    CHECK_R();
    r = gln_socket_connect(modu->in1, saw->out);
    CHECK_R();
    r = gln_socket_connect(modu->in2, tmpsine->out);
    CHECK_R();
    r = smaths_inst_play(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_play(inst, 7);
    CHECK_R();
    sleep(1);
    r = smaths_inst_stop(inst, 0);
    CHECK_R();
    nanosleep(&sleeptime, NULL);
    r = smaths_inst_stop(inst, 7);
    CHECK_R();
    sleep(1);
    OK();

    CHECKING(smaths_jbridge_create_midi_socket);
    struct gln_socket *from_jack_midi_socket;
    jack_port_t *from_jack_midi_port;
    r = smaths_jbridge_create_midi_socket(bridge, GLNS_INPUT, &from_jack_midi_socket, &from_jack_midi_port);
    OK();

    CHECKING(smaths_midi_ctlr_create);
    struct smaths_midi_ctlr *midi = smaths_midi_ctlr_create(bridge);
    atomic_float_store_explicit(&midi->attack_velocity_min, smaths_graph_normalized_time(bridge, 0.375f), memory_order_release);
    atomic_float_store_explicit(&midi->attack_velocity_max, smaths_graph_normalized_time(bridge, 0.125f), memory_order_release);
    /* atomic_store_explicit(&midi->chromatic, false, memory_order_release); */
    r = gln_socket_connect(midi->midi_in, from_jack_midi_socket);
    CHECK_R();
    r = gln_socket_connect(envg->ctl, midi->ctl);
    CHECK_R();
    r = gln_socket_connect(key->note, midi->out);
    CHECK_R();
    r = gln_socket_connect(mix_in1, saw->out);
    CHECK_R();
    r = gln_socket_connect(saw->amp, envg->out);
    CHECK_R();
    r = gln_socket_connect(saw->freq, key->freq);
    CHECK_R();
    r = gln_socket_connect(envg->attack_t, midi->attack_velocity);
    CHECK_R();
    OK();

    CHECKING_S("presence of system:midi_capture_1");
    r = jack_connect(bridge->client, "system:midi_capture_1", jack_port_name(from_jack_midi_port));
    if(r == 0) {
    	printf("found\nPausing to allow midi test, press enter to continue...");
    	getchar();
    	OK();
    } else {
    	printf("midi not found or error with jack_connect\n");
    }

    /* CHECKING_S("smaths_jbridge_destroy\n\t(expected to fail if jack server is not run seperately)"); */
    /* smaths_notch_destroy(&notch); */
    /* smaths_highpass_destroy(&highpass); */
    /* smaths_bandpass_destroy(&bandpass); */
    /* smaths_lowpass_destroy(&lowpass); */
    /* smaths_porta_destroy(&porta); */
    /* smaths_integrator_destroy(&intgr2); */
    /* smaths_integrator_destroy(&integrator); */
    /* smaths_distort_destroy(&distort); */
    /* smaths_itrain_destroy(&itrain); */
    /* smaths_dsf_destroy(&dsf); */
    /* smaths_saw_destroy(&saw); */
    /* smaths_bola_destroy(&bola); */
    /* smaths_square_destroy(&square); */
    /* smaths_triangle_destroy(&triangle); */
    /* smaths_noise_destroy(&noise); */
    /* smaths_mix_destroy(&mix); */
    /* smaths_jmidi_destroy(&jmidi); */
    /* smaths_clock_destroy(&clock); */
    /* smaths_sched_destroy(&sched); */
    /* smaths_key_destroy(&key); */
    /* smaths_key_destroy(&key2); */
    /* smaths_envg_destroy(&envg); */
    /* smaths_envg_destroy(&envg2); */
    /* smaths_inst_destroy(&inst); */
    /* smaths_inst_destroy(&inst2); */
    /* smaths_sine_destroy(&sine); */
    /* smaths_sine_destroy(&sine2); */
    /* smaths_sine_destroy(&sine3); */
    /* r = smaths_jbridge_destroy(&bridge); */
    /* if(r != 0) { */
    /* 	error(0, errno, "Error"); */
    /* } else { */
    /* 	OK(); */
    /* } */

    return 0;
}
