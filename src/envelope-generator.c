/*
 * envelope_generator.c
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

#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <atomickit/atomic.h>
#include <atomickit/atomic-malloc.h>
#include <atomickit/atomic-rcp.h>
#include <graphline.h>
#include "sonicmaths/buffer.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/envelope-generator.h"

/* e^-pi, the base of the exponent and the fraction of the distance
 * from the origin to the target that will remain when this is
 * considered to be finished. */
#define EXP_NEG_PI 0.0432139182637723f

/* 1/(1-e^-pi), the magic number to adjust the approach rate such that
 * the attack reaches its target in time. */
#define ATTACK_MAGIC_ADJ 1.04516570536368f

static inline float smaths_envg_process_decay_exp(float x, float y1, float T) {
    return x + powf(EXP_NEG_PI, 1/T) * (y1 - x);
}

static inline float smaths_envg_process_attack_exp(float x, float y1, float t, float T) {
    return x
	- (x - y1) *
	    (1 - ATTACK_MAGIC_ADJ * (1 - powf(EXP_NEG_PI, t)))
      / /*------------------------------------------------------*/
	  (1 - ATTACK_MAGIC_ADJ * (1 - powf(EXP_NEG_PI, t-1/T)));
}

static inline float smaths_envg_process_stage_lin(float x, float y1, float t, float T) {
    return y1 + (x - y1) / (T * (1 - t));
}

static inline float smaths_envg_inverse_t_exp(float x, float o, float y1) {
    return -logf(
	1 -
	            (y1 - o)
	/ /*----------------------------*/
	    (ATTACK_MAGIC_ADJ * (x - o))
	) / ((float) M_PI);
}

static inline float smaths_envg_inverse_t_lin(float x, float o, float y1) {
    return (o - y1) / (o - x);
}

static inline int smaths_envg_redim_state(struct smaths_envg *envg, int nchannels) {
    if(nchannels != envg->nchannels) {
	/* Redim the state... */
	int i;
	struct smaths_envg_state *state;
	state = arealloc(
	    envg->envg_state,
	    sizeof(struct smaths_envg_state) * envg->nchannels,
	    sizeof(struct smaths_envg_state) * nchannels);
	if(state == NULL) {
	    return -1;
	}
	for(i = envg->nchannels; i < nchannels; i++) {
	    state[i].stage = ENVG_FINISHED;
	    state[i].y1 = 0;
	    state[i].release = false;
	}
	envg->envg_state = state;
	envg->nchannels = nchannels;
    }
    return 0;
}

static int smaths_envg_process(struct smaths_envg *envg) {
    int r, i, j;

    struct smaths_buffer *ctl_buffer;
    struct smaths_buffer *out_buffer;
    struct smaths_buffer *attack_t_buffer;
    float attack_t;
    struct smaths_buffer *attack_a_buffer;
    float attack_a;
    struct smaths_buffer *decay_t_buffer;
    float decay_t;
    struct smaths_buffer *sustain_a_buffer;
    float sustain_a;
    struct smaths_buffer *release_t_buffer;
    float release_t;
    struct smaths_buffer *release_a_buffer;
    float release_a;

    int nframes, nchannels;

    bool linear;

    r = gln_get_buffers(7, envg->ctl, &ctl_buffer,
			envg->attack_t, &attack_t_buffer,
			envg->attack_a, &attack_a_buffer,
			envg->decay_t, &decay_t_buffer,
			envg->sustain_a, &sustain_a_buffer,
			envg->release_t, &release_t_buffer,
			envg->release_a, &release_a_buffer);
    if(r != 0) {
	return r;
    }

    attack_t = smaths_parameter_go(envg->attack_t, attack_t_buffer);
    attack_a = smaths_parameter_go(envg->attack_a, attack_a_buffer);
    decay_t = smaths_parameter_go(envg->decay_t, decay_t_buffer);
    sustain_a = smaths_parameter_go(envg->sustain_a, sustain_a_buffer);
    release_t = smaths_parameter_go(envg->release_t, release_t_buffer);
    release_a = smaths_parameter_go(envg->release_a, release_a_buffer);

    nframes = smaths_node_frames_per_period(envg);
    if(nframes < 0) {
	return nframes;
    }

    nchannels = ctl_buffer == NULL ? envg->nchannels : ctl_buffer->nchannels;

    r = smaths_envg_redim_state(envg, nchannels);
    if(r != 0) {
	return r;
    }

    out_buffer = smaths_alloc_buffer(envg->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    linear = atomic_load(&envg->linear);

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    float ctl = smaths_value(ctl_buffer, i, j, 0);
	    struct smaths_envg_state *state = &envg->envg_state[j];
	    float x;
	    float y;
	    float y1 = state->y1;
	    float t = state->t;
	    float T;
	    if(ctl > 0) {
		/* attack event */
		state->stage = ENVG_ATTACK;
		state->release = false;
		x = smaths_value(attack_a_buffer, i, j, attack_a);
		float o = smaths_value(release_a_buffer, i, j, release_a);
		if((o < x && y1 > x)
		   || (o > x && y1 < x)) {
		    o = -o;
		}
		t = linear ? smaths_envg_inverse_t_lin(x, o, y1)
		    : smaths_envg_inverse_t_exp(x, o, y1);
		goto start_attack;
	    } else if(ctl < 0) {
		/* release event */
		state->release = true;
	    }

	    switch(state->stage) {
	    case ENVG_ATTACK:
	    start_attack:
		x = smaths_value(attack_a_buffer, i, j, attack_a);
		T = smaths_value(attack_t_buffer, i, j, attack_t);
		if(T == 0) {
		    if(isnormal(x) || x == 0) {
			y1 = x;
		    } else if(x == INFINITY) {
			y1 = FLT_MAX;
		    } else if(x == -INFINITY) {
			y1 = -FLT_MAX;
		    } else {
			y1 = 0.0f;
		    }
		    t = 0;
		    state->stage = ENVG_DECAY;
		    goto start_decay;
		}
		y = linear ? smaths_envg_process_stage_lin(x, y1, t, T)
		    : smaths_envg_process_attack_exp(x, y1, t, T);
		t += 1/T;
		if((y <= x && x <= y1)
		   || (y >= x && x >= y1)) {
		    y = x;
		    t = 0;
		    state->stage = ENVG_DECAY;
		}
		break;
	    case ENVG_DECAY:
	    start_decay:
		x = smaths_value(sustain_a_buffer, i, j, sustain_a);
		T = smaths_value(decay_t_buffer, i, j, decay_t);
		if(T == 0) {
		    state->stage = ENVG_SUSTAIN;
		    goto start_sustain;
		}
		y = linear ? smaths_envg_process_stage_lin(x, y1, t, T)
		    : smaths_envg_process_decay_exp(x, y1, T);
		t += 1/T;
		if(linear) {
		    if((y <= x && x <= y1)
		       || (y >= x && x >= y1)) {
			state->stage = ENVG_SUSTAIN;
			goto start_sustain;
		    }
		} else if(state->release
			  && t >= 1) {
		    if(isnormal(y) || y == 0) {
			y1 = y;
		    } else if(y == INFINITY) {
			y1 = FLT_MAX;
		    } else if(y == -INFINITY) {
			y1 = -FLT_MAX;
		    } else {
			y1 = 0.0f;
		    }
		    t = 0;
		    state->stage = ENVG_RELEASE;
		    goto start_release;
		} else if(y == x) {
		    state->stage = ENVG_SUSTAIN;
		}
		break;
	    case ENVG_SUSTAIN:
	    start_sustain:
		y = smaths_value(sustain_a_buffer, i, j, sustain_a);
		if(state->release) {
		    if(isnormal(y) || y == 0) {
			y1 = y;
		    } else if(y == INFINITY) {
			y1 = FLT_MAX;
		    } else if(y == -INFINITY) {
			y1 = -FLT_MAX;
		    } else {
			y1 = 0.0f;
		    }
		    t = 0;
		    state->stage = ENVG_RELEASE;
		    goto start_release;
		}
		break;
	    case ENVG_RELEASE:
	    start_release:
		T = smaths_value(release_t_buffer, i, j, release_t);
		x = smaths_value(release_a_buffer, i, j, release_a);
		if(T == 0) {
		    state->stage = ENVG_FINISHED;
		    goto start_finished;
		}
		y = linear ? smaths_envg_process_stage_lin(x, y1, t, T)
		    : smaths_envg_process_decay_exp(x, y1, T);
		t += 1/T;
		if((y <= x && x <= y1)
		   || (y >= x && x >= y1)) {
		    y = x;
		    state->stage = ENVG_FINISHED;
		}
		break;
	    case ENVG_FINISHED:
	    start_finished:
		y = smaths_value(release_a_buffer, i, j, release_a);
	    }

	    state->t = t;
	    if(isnormal(y) || y == 0) {
		state->y1 = y;
	    } else if(y == INFINITY) {
		state->y1 = FLT_MAX;
	    } else if(y == -INFINITY) {
		state->y1 = -FLT_MAX;
	    } else {
		state->y1 = 0.0f;
	    }
	    out_buffer->data[i * nchannels + j] = y;
	}
    }
    return 0;
}

int smaths_envg_init(struct smaths_envg *envg, struct smaths_graph *graph, void (*destroy)(struct smaths_envg *)) {
    int r = -1;

    envg->envg_state = amalloc(sizeof(struct smaths_envg_state));
    if(envg->envg_state == NULL) {
	goto undo0;
    }

    r = gln_node_init(envg, graph, (gln_process_fp_t) smaths_envg_process, (void (*)(struct gln_node *)) destroy);
    if(r != 0) {
	goto undo1;
    }

    envg->ctl = gln_socket_create(envg, GLNS_INPUT);
    if(envg->ctl == NULL) {
	r = -1;
	goto undo2;
    }

    envg->out = gln_socket_create(envg, GLNS_OUTPUT);
    if(envg->out == NULL) {
	r = -1;
	goto undo3;
    }

    envg->attack_t = smaths_parameter_create(envg, 0.0f);
    if(envg->attack_t == NULL) {
	r = -1;
	goto undo4;
    }

    envg->attack_a = smaths_parameter_create(envg, 1.0f);
    if(envg->attack_a == NULL) {
	r = -1;
	goto undo5;
    }

    envg->decay_t = smaths_parameter_create(envg, 0.0f);
    if(envg->decay_t == NULL) {
	r = -1;
	goto undo6;
    }

    envg->sustain_a = smaths_parameter_create(envg, 1.0f);
    if(envg->sustain_a == NULL) {
	r = -1;
	goto undo7;
    }

    envg->release_t = smaths_parameter_create(envg, 0.0f);
    if(envg->release_t == NULL) {
	r = -1;
	goto undo8;
    }

    envg->release_a = smaths_parameter_create(envg, 0.0f);
    if(envg->release_a == NULL) {
	r = -1;
	goto undo9;
    }

    atomic_init(&envg->linear, false);
    envg->nchannels = 1;
    envg->envg_state[0].stage = ENVG_FINISHED;
    envg->envg_state[0].y1 = 0.0f;
    envg->envg_state[0].release = false;

    return 0;

undo9:
    arcp_release(envg->release_t);
undo8:
    arcp_release(envg->sustain_a);
undo7:
    arcp_release(envg->decay_t);
undo6:
    arcp_release(envg->attack_a);
undo5:
    arcp_release(envg->attack_t);
undo4:
    arcp_release(envg->out);
undo3:
    arcp_release(envg->ctl);
undo2:
    gln_node_destroy(envg);
undo1:
    afree(envg->envg_state, sizeof(struct smaths_envg_state));
undo0:
    return r;
}

void smaths_envg_destroy(struct smaths_envg *envg) {
    arcp_release(envg->release_a);
    arcp_release(envg->release_t);
    arcp_release(envg->sustain_a);
    arcp_release(envg->decay_t);
    arcp_release(envg->attack_a);
    arcp_release(envg->attack_t);
    arcp_release(envg->out);
    arcp_release(envg->ctl);
    gln_node_destroy(envg);
    afree(envg->envg_state, sizeof(struct smaths_envg_state) * envg->nchannels);
}

static void __smaths_envg_destroy(struct smaths_envg *envg) {
    smaths_envg_destroy(envg);
    afree(envg, sizeof(struct smaths_envg));
}

struct smaths_envg *smaths_envg_create(struct smaths_graph *graph) {
    int r;
    struct smaths_envg *ret;

    ret = amalloc(sizeof(struct smaths_envg));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_envg_init(ret, graph, __smaths_envg_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_envg));
	return NULL;
    }

    return ret;
}
