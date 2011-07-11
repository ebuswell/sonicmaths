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
#include <sonicmaths/jbridge.h>
#include <sonicmaths/sine.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/instrument.h>
#include <sonicmaths/envelope-generator.h>
#include <sonicmaths/clock.h>
#include <sonicmaths/scheduler.h>
#include <sonicmaths/key.h>
#include <graphline.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

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
    smaths_parameter_set(&envg.sustain_a, 0.65f);
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
    sleep(5);
    OK();

    CHECKING_S("smaths_jbridge_destroy\n\t(expected to fail if jack server is not run seperately)");
    smaths_clock_destroy(&clock);
    smaths_sched_destroy(&sched);
    smaths_envg_destroy(&envg);
    smaths_inst_destroy(&inst);
    smaths_sine_destroy(&sine);
    r = smaths_jbridge_destroy(&bridge);
    if(r != 0) {
	error(0, errno, "Error");
    } else {
	OK();
    }

    return 0;
}
