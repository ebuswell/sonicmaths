/*
 * test.c
 * 
 * Copyright 2011 Evan Buswell
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
#include <atomickit/atomic.h>
#include <graphline.h>
#include "sonicmaths/jbridge.h"
#include "sonicmaths/sine.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/instrument.h"
#include "sonicmaths/envelope-generator.h"
#include "sonicmaths/clock.h"
#include "sonicmaths/scheduler.h"
#include "sonicmaths/key.h"
#include "sonicmaths/jmidi.h"
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
#include "sonicmaths/dsf.h"
#include "sonicmaths/sawtooth.h"

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

/* struct nullnode { */
/*     struct gln_node node; */
/*     struct gln_socket in; */
/*     struct gln_socket out; */
/* }; */

/* int nullnode_f(struct nullnode *self) { */
/*     void *in_buffer = gln_socket_get_buffer(&self->in); */
/*     if(in_buffer == NULL) */
/* 	return -1; */
/*     void *out_buffer = gln_socket_get_buffer(&self->out); */
/*     if(out_buffer == NULL) */
/* 	return -1; */

/*     memcpy(out_buffer, in_buffer, self->node.graph->buffer_size); */

/*     return 0; */
/* } */

int play_first(struct smaths_inst *inst) {
    smaths_inst_play(inst, 0.0f);
    return 0;
}

int play_second(struct smaths_inst *inst) {
    smaths_inst_play(inst, 2.0f);
    return 0;
}

int play_third(struct smaths_inst *inst) {
    smaths_inst_play(inst, 4.0f);
    return 0;
}

int stop_it(struct smaths_inst *inst) {
    smaths_inst_stop(inst);
    return 0;
}

int main(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
    int r;

    setbuf(stdout, NULL);

    jack_status_t status;
    CHECKING(smaths_jbridge_init);
    struct smaths_jbridge bridge;
    r = smaths_jbridge_init(&bridge, "smaths_jbridge", 0, &status, NULL);
    if(r != 0) {
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
    r = smaths_jbridge_create_socket(&bridge, OUTPUT, &to_jack_socket, &to_jack_port);
    CHECK_R();

    /* struct gln_socket *from_jack_socket; */
    /* jack_port_t *from_jack_port; */
    /* r = smaths_jbridge_create_socket(&bridge, INPUT, &from_jack_socket, &from_jack_port); */
    /* CHECK_R(); */

    r = jack_connect(bridge.client, jack_port_name(to_jack_port), "system:playback_1");
    CHECK_R();
    r = jack_connect(bridge.client, jack_port_name(to_jack_port), "system:playback_2");
    CHECK_R();
    OK();

    /* printf("Pausing to allow jack configuration, press enter to continue..."); */
    /* getchar(); */
    /* OK(); */

    /* CHECKING_S("nullnode creation and hook up"); */
    /* struct nullnode nn; */
    /* r = gln_node_init(&nn.node, &bridge.graph.graph, (gln_process_fp_t) nullnode_f, &nn); */
    /* CHECK_R(); */
    /* r = gln_socket_init(&nn.in, &nn.node, INPUT); */
    /* CHECK_R(); */
    /* r = gln_socket_init(&nn.out, &nn.node, OUTPUT); */
    /* CHECK_R(); */
    /* r = gln_socket_connect(&nn.in, from_jack_socket); */
    /* CHECK_R(); */
    /* r = gln_socket_connect(&nn.out, to_jack_socket); */
    /* CHECK_R(); */
    /* OK(); */

    CHECKING(smaths_sine_init);
    struct smaths_sine sine;
    r = smaths_sine_init(&sine, &bridge.graph);
    CHECK_R();
    r = gln_socket_connect(&sine.out, to_jack_socket);
    CHECK_R();
    OK();

    CHECKING(smaths_parameter_set);
    smaths_parameter_set(&sine.freq, smaths_graph_normalized_frequency(&bridge.graph, 440.0));
    sleep(1);
    smaths_parameter_set(&sine.freq, smaths_graph_normalized_frequency(&bridge.graph, 0.0));
    OK();

    CHECKING(smaths_inst_init);
    struct smaths_inst inst;
    r = smaths_inst_init(&inst, &bridge.graph);
    CHECK_R();
    OK();

    CHECKING(smaths_parameter_connect);
    r = smaths_parameter_connect(&sine.freq, &inst.ctlr.out);
    CHECK_R();
    OK();

    CHECKING(smaths_envg_init);
    struct smaths_envg envg;
    r = smaths_envg_init(&envg, &bridge.graph);
    CHECK_R();
    smaths_parameter_set(&envg.attack_t, smaths_graph_normalized_time(&bridge.graph, 0.5f));
    smaths_parameter_set(&envg.decay_t, smaths_graph_normalized_time(&bridge.graph, 0.5f));
    smaths_parameter_set(&envg.sustain_a, expf(-0.5f));
    smaths_parameter_set(&envg.release_t, smaths_graph_normalized_time(&bridge.graph, 1.0f));
    r = smaths_parameter_connect(&sine.amp, &envg.out);
    CHECK_R();
    r = gln_socket_connect(&envg.ctl, &inst.ctlr.ctl);
    CHECK_R();
    OK();

    CHECKING(smaths_inst_play);
    smaths_inst_play(&inst, smaths_graph_normalized_frequency(&bridge.graph, 880.0f));
    sleep(1);
    OK();

    CHECKING(smaths_inst_stop);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING(smaths_key_init);
    struct smaths_key key;
    r = smaths_key_init(&key, &bridge.graph);
    CHECK_R();
    r = smaths_parameter_connect(&key.note, &inst.ctlr.out);
    CHECK_R();
    r = smaths_parameter_connect(&sine.freq, &key.freq);
    CHECK_R();
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 1.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING(smaths_key_set_tuning);
    r = smaths_key_set_tuning(&key, SMATHS_MAJOR_TUNING);
    CHECK_R();
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 1.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING(smaths_key_note2freq);
    smaths_parameter_set(&sine.freq, smaths_key_note2freq(&key, 2.0f));
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    smaths_parameter_connect(&sine.freq, &key.freq);
    OK();

    CHECKING(smaths_clock_init);
    struct smaths_clock clock;
    r = smaths_clock_init(&clock, &bridge.graph);
    CHECK_R();
    smaths_parameter_set(&clock.rate, smaths_graph_normalized_rate(&bridge.graph, 144.0f/60.0f));
    OK();

    CHECKING(smaths_sched_init);
    struct smaths_sched sched;
    r = smaths_sched_init(&sched, &bridge.graph);
    CHECK_R();
    OK();

    CHECKING(smaths_sched_schedule);
    struct smaths_sched_event first_event = { 1.0f, (smaths_event_fp_t) play_first, &inst };
    struct smaths_sched_event second_event = { 2.0f, (smaths_event_fp_t) play_second, &inst };
    struct smaths_sched_event third_event = { 3.0f, (smaths_event_fp_t) play_third, &inst };
    struct smaths_sched_event stop_event = { 3.85f, (smaths_event_fp_t) stop_it, &inst };
    r = smaths_sched_schedule(&sched, &stop_event);
    CHECK_R();
    r = smaths_sched_schedule(&sched, &first_event);
    CHECK_R();
    r = smaths_sched_schedule(&sched, &third_event);
    CHECK_R();
    r = smaths_sched_schedule(&sched, &second_event);
    CHECK_R();
    r = gln_socket_connect(&sched.clock, &clock.clock);
    CHECK_R();
    sleep(5.0 * 60.0/144.0);
    OK();

    CHECKING(smaths_sched_cancel);
    first_event.time = 6.0f;
    second_event.time = 7.0f;
    third_event.time = 8.0f;
    stop_event.time = 8.85f;
    r = smaths_sched_schedule(&sched, &stop_event);
    CHECK_R();
    r = smaths_sched_schedule(&sched, &first_event);
    CHECK_R();
    r = smaths_sched_schedule(&sched, &third_event);
    CHECK_R();
    r = smaths_sched_schedule(&sched, &second_event);
    CHECK_R();
    smaths_sched_cancel(&sched, &third_event);
    sleep(6);
    OK();

    printf("Building another instrument...");
    gln_socket_disconnect(&sine.out);
    struct smaths_sine sine2;
    r = smaths_sine_init(&sine2, &bridge.graph);
    CHECK_R();
    struct smaths_inst inst2;
    r = smaths_inst_init(&inst2, &bridge.graph);
    CHECK_R();
    struct smaths_key key2;
    r = smaths_key_init(&key2, &bridge.graph);
    CHECK_R();
    struct smaths_envg envg2;
    r = smaths_envg_init(&envg2, &bridge.graph);
    CHECK_R();
    r = smaths_key_set_tuning(&key2, SMATHS_MAJOR_TUNING);
    CHECK_R();
    smaths_parameter_set(&envg2.attack_t, smaths_graph_normalized_time(&bridge.graph, 0.5f));
    smaths_parameter_set(&envg2.decay_t, smaths_graph_normalized_time(&bridge.graph, 0.5f));
    smaths_parameter_set(&envg2.sustain_a, expf(-0.5f));
    smaths_parameter_set(&envg2.release_t, smaths_graph_normalized_time(&bridge.graph, 1.0f));
    r = smaths_parameter_connect(&sine2.freq, &key2.freq);
    CHECK_R();
    r = smaths_parameter_connect(&sine2.amp, &envg2.out);
    CHECK_R();
    r = gln_socket_connect(&envg2.ctl, &inst2.ctlr.ctl);
    CHECK_R();
    r = smaths_parameter_connect(&key2.note, &inst2.ctlr.out);
    CHECK_R();
    r = gln_socket_connect(&sine2.out, to_jack_socket);
    CHECK_R();
    smaths_inst_play(&inst2, 0.0f);
    sleep(1);
    smaths_inst_stop(&inst2);
    sleep(1);
    OK();

    CHECKING(smaths_mix_init);
    struct smaths_mix mix;
    r = smaths_mix_init(&mix, &bridge.graph);
    CHECK_R();
    r = gln_socket_connect(&mix.out, to_jack_socket);
    CHECK_R();
    OK();

    CHECKING(smaths_mix_input_init);
    struct smaths_parameter mix_in1;
    struct smaths_parameter mix_in1_amp;
    r = smaths_mix_input_init(&mix, &mix_in1, &mix_in1_amp);
    CHECK_R();
    struct smaths_parameter mix_in2;
    struct smaths_parameter mix_in2_amp;
    r = smaths_mix_input_init(&mix, &mix_in2, &mix_in2_amp);
    CHECK_R();
    smaths_parameter_set(&mix_in1_amp, 0.5f);
    smaths_parameter_set(&mix_in2_amp, 0.5f);
    r = smaths_parameter_connect(&mix_in1, &sine.out);
    CHECK_R();
    r = smaths_parameter_connect(&mix_in2, &sine2.out);
    CHECK_R();
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst2, 2.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    smaths_inst_stop(&inst2);
    sleep(1);
    OK();

    CHECKING(smaths_mix_input_destroy);
    // smaths_parameter_set(&mix_in1, 0.0f);
    smaths_parameter_set(&mix_in2, 0.0f);
    // smaths_mix_input_destroy(&mix, &mix_in1, &mix_in1_amp);
    smaths_mix_input_destroy(&mix, &mix_in2, &mix_in2_amp);
    OK();

    CHECKING(smaths_noise_init);
    struct smaths_noise noise;
    r = smaths_noise_init(&noise, &bridge.graph);
    CHECK_R();
    OK();

    CHECKING_S("smaths_noise: white");
    r = smaths_parameter_connect(&mix_in1, &noise.out);
    CHECK_R();
    atomic_set(&noise.kind, SMATHS_WHITE);
    sleep(1);
    OK();

    CHECKING_S("smaths_noise: pink");
    atomic_set(&noise.kind, SMATHS_PINK);
    sleep(1);
    r = smaths_parameter_connect(&mix_in1, &sine.out);
    CHECK_R();
    OK();

    CHECKING(smaths_distort_init);
    struct smaths_distort distort;
    r = smaths_distort_init(&distort, &bridge.graph);
    CHECK_R();
    r = smaths_parameter_connect(&mix_in1, &distort.filter.out);
    CHECK_R();
    r = smaths_parameter_connect(&distort.filter.in, &sine.out);
    CHECK_R();
    smaths_parameter_set(&distort.gain, expf(1.5f));
    OK();

    CHECKING_S("smaths_distort: exponential");
    atomic_set(&distort.kind, SMATHS_EXP);
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 4.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING_S("smaths_distort: hyperbolic");
    atomic_set(&distort.kind, SMATHS_HYP);
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 4.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING_S("smaths_distort: arctangent");
    atomic_set(&distort.kind, SMATHS_ATAN);
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 4.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING_S("smaths_distort: tube simulation");
    atomic_set(&distort.kind, SMATHS_TUBE);
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 4.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING(smaths_dsf_init);
    struct smaths_dsf dsf;
    r = smaths_dsf_init(&dsf, &bridge.graph);
    CHECK_R();
    smaths_parameter_set(&dsf.bright, 0.8f);
    r = smaths_parameter_connect(&mix_in1, &dsf.synth.out);
    CHECK_R();
    r = smaths_parameter_connect(&dsf.synth.amp, &envg.out);
    CHECK_R();
    r = smaths_parameter_connect(&dsf.synth.freq, &key.freq);
    CHECK_R();
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 2.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING(smaths_saw_init);
    struct smaths_saw saw;
    r = smaths_saw_init(&saw, &bridge.graph);
    CHECK_R();
    atomic_set(&saw.scale, 1);
    r = smaths_parameter_connect(&mix_in1, &saw.synth.out);
    CHECK_R();
    r = smaths_parameter_connect(&saw.synth.amp, &envg.out);
    CHECK_R();
    r = smaths_parameter_connect(&saw.synth.freq, &key.freq);
    CHECK_R();
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 2.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING(smaths_itrain_init);
    struct smaths_itrain itrain;
    r = smaths_itrain_init(&itrain, &bridge.graph);
    CHECK_R();
    atomic_set(&itrain.scale, 1);
    r = smaths_parameter_connect(&mix_in1, &itrain.synth.out);
    CHECK_R();
    r = smaths_parameter_connect(&itrain.synth.amp, &envg.out);
    CHECK_R();
    r = smaths_parameter_connect(&itrain.synth.freq, &key.freq);
    CHECK_R();
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 2.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING(smaths_integrator_init);
    struct smaths_integrator integrator;
    r = smaths_integrator_init(&integrator, &bridge.graph);
    CHECK_R();
    OK();

    CHECKING_S("smaths_integrator: one pass");
    r = smaths_parameter_connect(&mix_in1, &integrator.filter.out);
    CHECK_R();
    smaths_parameter_set(&mix_in1_amp, 0.25f);
    r = smaths_parameter_connect(&integrator.filter.in, &itrain.synth.out);
    CHECK_R();
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 2.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING_S("smaths_integrator: two pass");
    struct smaths_integrator intgr2;
    r = smaths_integrator_init(&intgr2, &bridge.graph);
    CHECK_R();
    smaths_parameter_set(&mix_in1_amp, 0.0625f);
    r = smaths_parameter_connect(&integrator.filter.in, &intgr2.filter.out);
    CHECK_R();
    r = smaths_parameter_connect(&intgr2.filter.in, &itrain.synth.out);
    CHECK_R();
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 2.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();


    CHECKING(smaths_porta_init);
    struct smaths_porta porta;
    r = smaths_porta_init(&porta, &bridge.graph);
    CHECK_R();
    smaths_parameter_set(&porta.lag, 44100.0f);
    smaths_parameter_set(&mix_in1_amp, 0.5f);
    r = smaths_parameter_connect(&mix_in1, &itrain.synth.out);
    CHECK_R();
    r = smaths_parameter_connect(&porta.filter.in, &inst.ctlr.out);
    CHECK_R();
    r = smaths_parameter_connect(&key.note, &porta.filter.out);
    CHECK_R();
    smaths_inst_play(&inst, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 2.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING(smaths_lowpass_init);
    struct smaths_lowpass lowpass;
    r = smaths_lowpass_init(&lowpass, &bridge.graph);
    CHECK_R();
    r = smaths_parameter_connect(&key.note, &inst.ctlr.out);
    CHECK_R();
    r = smaths_parameter_connect(&lowpass.freq, &porta.filter.out);
    CHECK_R();
    r = smaths_parameter_connect(&mix_in1, &lowpass.filter.out);
    CHECK_R();
    r = smaths_parameter_connect(&lowpass.filter.in, &itrain.synth.out);
    CHECK_R();
    smaths_parameter_set(&porta.filter.in, 0.0f);
    sleep(1);
    smaths_inst_play(&inst, 0.0f);
    smaths_parameter_set(&porta.filter.in, 0.5f);
    sleep(1);
    smaths_parameter_set(&porta.filter.in, 0.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING(smaths_bandpass_init);
    struct smaths_bandpass bandpass;
    r = smaths_bandpass_init(&bandpass, &bridge.graph);
    CHECK_R();
    r = smaths_parameter_connect(&bandpass.lowpass.freq, &porta.filter.out);
    CHECK_R();
    r = smaths_parameter_connect(&mix_in1, &bandpass.lowpass.filter.out);
    CHECK_R();
    r = smaths_parameter_connect(&bandpass.lowpass.filter.in, &itrain.synth.out);
    CHECK_R();
    smaths_inst_play(&inst, 0.0f);
    smaths_parameter_set(&porta.filter.in, 0.5f);
    sleep(1);
    smaths_parameter_set(&porta.filter.in, 0.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING(smaths_highpass_init);
    struct smaths_highpass highpass;
    r = smaths_highpass_init(&highpass, &bridge.graph);
    CHECK_R();
    r = smaths_parameter_connect(&highpass.lowpass.freq, &porta.filter.out);
    CHECK_R();
    r = smaths_parameter_connect(&mix_in1, &highpass.lowpass.filter.out);
    CHECK_R();
    r = smaths_parameter_connect(&highpass.lowpass.filter.in, &itrain.synth.out);
    CHECK_R();
    smaths_inst_play(&inst, 0.0f);
    smaths_parameter_set(&porta.filter.in, 0.5f);
    sleep(1);
    smaths_parameter_set(&porta.filter.in, 0.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    CHECKING(smaths_notch_init);
    struct smaths_notch notch;
    r = smaths_notch_init(&notch, &bridge.graph);
    CHECK_R();
    r = smaths_parameter_connect(&notch.lowpass.freq, &porta.filter.out);
    CHECK_R();
    r = smaths_parameter_connect(&mix_in1, &notch.lowpass.filter.out);
    CHECK_R();
    r = smaths_parameter_connect(&notch.lowpass.filter.in, &itrain.synth.out);
    CHECK_R();
    smaths_inst_play(&inst, 0.0f);
    smaths_parameter_set(&porta.filter.in, 0.5f);
    sleep(1);
    smaths_parameter_set(&porta.filter.in, 0.0f);
    sleep(1);
    smaths_inst_stop(&inst);
    sleep(1);
    OK();

    // modu??

    smaths_parameter_set(&mix_in1_amp, 0.5f);

    r = smaths_parameter_connect(&mix_in1, &sine.out);
    CHECK_R();

    CHECKING(smaths_jmidi_init);
    struct smaths_jmidi jmidi;
    r = smaths_jmidi_init(&jmidi, &bridge, 0);
    CHECK_R();
    r = smaths_parameter_connect(&envg.attack_a, &jmidi.vel);
    CHECK_R();
    r = smaths_parameter_connect(&envg.sustain_a, &jmidi.vel);
    CHECK_R();
    smaths_parameter_set(&envg.decay_t, 0.0f);
    r = smaths_parameter_connect(&key.note, &jmidi.note);
    CHECK_R();
    r = gln_socket_connect(&envg.ctl, &jmidi.ctl);
    CHECK_R();
    r = smaths_key_set_tuning(&key, SMATHS_EQUAL_TUNING);
    CHECK_R();
    OK();

    struct smaths_sine sine3;
    r = smaths_sine_init(&sine3, &bridge.graph);
    CHECK_R();
    r = smaths_parameter_connect(&sine3.freq, &key.freq);
    CHECK_R();
    r = smaths_parameter_connect(&sine.phase, &sine3.out);
    CHECK_R();

    CHECKING_S("presence of system:midi_capture_1");
    r = jack_connect(bridge.client, "system:midi_capture_1", jack_port_name(jmidi.midi_port));
    if(r == 0) {
	printf("found\nPausing to allow midi test, press enter to continue...");
	getchar();
	OK();
    } else {
	printf("midi not found or error with jack_connect\n");
    }

    CHECKING_S("smaths_jbridge_destroy\n\t(expected to fail if jack server is not run seperately)");
    smaths_notch_destroy(&notch);
    smaths_highpass_destroy(&highpass);
    smaths_bandpass_destroy(&bandpass);
    smaths_lowpass_destroy(&lowpass);
    smaths_porta_destroy(&porta);
    smaths_integrator_destroy(&intgr2);
    smaths_integrator_destroy(&integrator);
    smaths_distort_destroy(&distort);
    smaths_itrain_destroy(&itrain);
    smaths_dsf_destroy(&dsf);
    smaths_saw_destroy(&saw);
    smaths_noise_destroy(&noise);
    smaths_mix_destroy(&mix);
    smaths_jmidi_destroy(&jmidi);
    smaths_clock_destroy(&clock);
    smaths_sched_destroy(&sched);
    smaths_key_destroy(&key);
    smaths_key_destroy(&key2);
    smaths_envg_destroy(&envg);
    smaths_envg_destroy(&envg2);
    smaths_inst_destroy(&inst);
    smaths_inst_destroy(&inst2);
    smaths_sine_destroy(&sine);
    smaths_sine_destroy(&sine2);
    smaths_sine_destroy(&sine3);
    r = smaths_jbridge_destroy(&bridge);
    if(r != 0) {
	error(0, errno, "Error");
    } else {
	OK();
    }

    return 0;
}
