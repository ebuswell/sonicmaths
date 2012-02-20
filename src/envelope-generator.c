/*
 * envelope_generator.c
 * 
 * Copyright 2011 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 2 of the License,
 * or (at your option) any later version.
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

#include <stdbool.h>
#include <math.h>
#include <atomickit/atomic.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/envelope-generator.h"

static inline float smaths_envg_process_stage(bool linear, float from, float to, float last, double time) {
    if(linear) {
	return last + (to - from) / time;
    } else {
	return to + (last - to) * exp(-M_PI / time);
    }
}

static int smaths_envg_process(struct smaths_envg *self) {
    float *ctl_buffer = gln_socket_get_buffer(&self->ctl);
    if(ctl_buffer == NULL) {
	return -1;
    }
    float *out_buffer = gln_socket_get_buffer(&self->out);
    if(out_buffer == NULL) {
	return -1;
    }
    float *attack_t = smaths_parameter_get_buffer(&self->attack_t);
    if(attack_t == NULL) {
	return -1;
    }
    float *attack_a = smaths_parameter_get_buffer(&self->attack_a);
    if(attack_a == NULL) {
	return -1;
    }
    float *decay_t = smaths_parameter_get_buffer(&self->decay_t);
    if(decay_t == NULL) {
	return -1;
    }
    float *sustain_a = smaths_parameter_get_buffer(&self->sustain_a);
    if(sustain_a == NULL) {
	return -1;
    }
    float *release_t = smaths_parameter_get_buffer(&self->release_t);
    if(release_t == NULL) {
	return -1;
    }
    float *release_a = smaths_parameter_get_buffer(&self->release_a);
    if(release_a == NULL) {
	return -1;
    }
    bool linear = (bool) atomic_read(&self->linear);
    size_t i;
    for(i = 0; i < self->graph->graph.buffer_nmemb; i++) {
	float ctl = ctl_buffer[i];
	if(isnanf(ctl)) {
	    ctl = 0.0f;
	}
	if(ctl > 0.0f) {
	    // attack event
	    self->state = ATTACK;
	    self->release = false;
	    self->upwards = (attack_a[i] > self->last_a);
	} else if(ctl < 0.0f) {
	    // release event
	    self->release = true;
	}
	switch(self->state) {
	case ATTACK:
	    if(attack_t[i] <= 0.0f) {
		self->last_a = attack_a[i];
		self->state = DECAY;
		self->upwards = (sustain_a[i] > self->last_a);
		goto start_decay;
	    }
	    if(!linear) {
		/* Adjust attack_a */
		float attack_adj = (attack_a[i] - release_a[i] * exp(-M_PI))
		                 /*-----------------------------------------*/
		                  /           (1.0 - exp(-M_PI));
		out_buffer[i] = self->last_a = smaths_envg_process_stage(linear, release_a[i], attack_adj, self->last_a, attack_t[i]);
	    } else {
		out_buffer[i] = self->last_a = smaths_envg_process_stage(linear, release_a[i], attack_a[i], self->last_a, attack_t[i]);
	    }
	    if(self->upwards ? (self->last_a >= attack_a[i]) : (self->last_a <= attack_a[i])) {
		out_buffer[i] = self->last_a = attack_a[i];
		self->state = DECAY;
		self->upwards = (sustain_a[i] > self->last_a);
	    }
	    break;
	case DECAY:
	start_decay:
	    if(decay_t[i] <= 0.0f) {
		self->state = SUSTAIN;
		goto start_sustain;
	    }
	    out_buffer[i] = self->last_a = smaths_envg_process_stage(linear, attack_a[i], sustain_a[i], self->last_a, decay_t[i]);
	    if(linear
	       && (self->upwards ? (self->last_a >= sustain_a[i])
		   : (self->last_a <= sustain_a[i]))) {
		self->state = SUSTAIN;
		goto start_sustain;
	    } else if(self->release // find out if we are within the range for releasing:
		      && (self->upwards
			  ? (self->last_a >= sustain_a[i] + (attack_a[i] - sustain_a[i]) * exp(-M_PI))
			  : (self->last_a <= sustain_a[i] + (attack_a[i] - sustain_a[i]) * exp(-M_PI)))) {
		self->state = RELEASE;
		self->upwards = (release_a[i] > self->last_a);
		goto start_release;
	    } else if(self->upwards ? (self->last_a >= sustain_a[i]) : (self->last_a <= sustain_a[i])) {
		self->state = SUSTAIN;
		goto start_sustain;
	    }
	    break;
	case SUSTAIN:
	start_sustain:
	    out_buffer[i] = self->last_a = sustain_a[i];
	    if(self->release) {
		self->state = RELEASE;
		self->upwards = (release_a[i] > self->last_a);
		goto start_release;
	    }
	    break;
	case RELEASE:
	start_release:
	    if(release_t[i] <= 0.0f) {
		self->state = FINISHED;
		goto start_finished;
	    }
	    out_buffer[i] = self->last_a = smaths_envg_process_stage(linear, sustain_a[i], release_a[i], self->last_a, release_t[i]);
	    if(self->upwards ? (self->last_a >= release_a[i]) : (self->last_a <= release_a[i])) {
		self->state = FINISHED;
		goto start_finished;
	    }
	    break;
	case FINISHED:
	start_finished:
	    out_buffer[i] = self->last_a = release_a[i];
	}
    }
    return 0;
}

int smaths_envg_init(struct smaths_envg *self, struct smaths_graph *graph) {
    int r;
    self->graph = graph;

    r = gln_node_init(&self->node, &graph->graph, (gln_process_fp_t) smaths_envg_process, self);
    if(r != 0) {
	return r;
    }

    r = gln_socket_init(&self->ctl, &self->node, INPUT);
    if(r != 0) {
	gln_node_destroy(&self->node);
	return r;
    }

    r = gln_socket_init(&self->out, &self->node, OUTPUT);
    if(r != 0) {
	gln_socket_destroy(&self->ctl);
	gln_node_destroy(&self->node);
	return r;
    }

    r = smaths_parameter_init(&self->attack_t, &self->node, 0.0f);
    if(r != 0) {
	gln_socket_destroy(&self->out);
	gln_socket_destroy(&self->ctl);
	gln_node_destroy(&self->node);
	return r;
    }
    r = smaths_parameter_init(&self->attack_a, &self->node, 1.0f);
    if(r != 0) {
	smaths_parameter_destroy(&self->attack_t);
	gln_socket_destroy(&self->out);
	gln_socket_destroy(&self->ctl);
	gln_node_destroy(&self->node);
	return r;
    }
    r = smaths_parameter_init(&self->decay_t, &self->node, 0.0f);
    if(r != 0) {
	smaths_parameter_destroy(&self->attack_a);
	smaths_parameter_destroy(&self->attack_t);
	gln_socket_destroy(&self->out);
	gln_socket_destroy(&self->ctl);
	gln_node_destroy(&self->node);
	return r;
    }
    r = smaths_parameter_init(&self->sustain_a, &self->node, 1.0f);
    if(r != 0) {
	smaths_parameter_destroy(&self->decay_t);
	smaths_parameter_destroy(&self->attack_a);
	smaths_parameter_destroy(&self->attack_t);
	gln_socket_destroy(&self->out);
	gln_socket_destroy(&self->ctl);
	gln_node_destroy(&self->node);
	return r;
    }
    r = smaths_parameter_init(&self->release_t, &self->node, 0.0f);
    if(r != 0) {
	smaths_parameter_destroy(&self->sustain_a);
	smaths_parameter_destroy(&self->decay_t);
	smaths_parameter_destroy(&self->attack_a);
	smaths_parameter_destroy(&self->attack_t);
	gln_socket_destroy(&self->out);
	gln_socket_destroy(&self->ctl);
	gln_node_destroy(&self->node);
	return r;
    }
    r = smaths_parameter_init(&self->release_a, &self->node, 0.0f);
    if(r != 0) {
	smaths_parameter_destroy(&self->release_t);
	smaths_parameter_destroy(&self->sustain_a);
	smaths_parameter_destroy(&self->decay_t);
	smaths_parameter_destroy(&self->attack_a);
	smaths_parameter_destroy(&self->attack_t);
	gln_socket_destroy(&self->out);
	gln_socket_destroy(&self->ctl);
	gln_node_destroy(&self->node);
	return r;
    }
    atomic_set(&self->linear, false);
    self->state = FINISHED;
    self->last_a = 0.0;
    self->release = false;
    self->upwards = true;
    return 0;
}

void smaths_envg_destroy(struct smaths_envg *self) {
    smaths_parameter_destroy(&self->release_a);
    smaths_parameter_destroy(&self->release_t);
    smaths_parameter_destroy(&self->sustain_a);
    smaths_parameter_destroy(&self->decay_t);
    smaths_parameter_destroy(&self->attack_a);
    smaths_parameter_destroy(&self->attack_t);
    gln_socket_destroy(&self->out);
    gln_socket_destroy(&self->ctl);
    gln_node_destroy(&self->node);
}
