/*
 * instrument.c
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
#include <string.h>
#include <atomickit/atomic.h>
#include <atomickit/atomic-malloc.h>
#include <atomickit/atomic-rcp.h>
#include <atomickit/atomic-queue.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/buffer.h"
#include "sonicmaths/controller.h"
#include "sonicmaths/instrument.h"

static int smaths_inst_process(struct smaths_inst *inst) {
    int i, j;

    struct smaths_buffer *out_buffer;
    struct smaths_buffer *ctl_buffer;

    int nframes;
    int nchannels;

    nframes = smaths_node_frames_per_period(inst);
    if(nframes < 0) {
	return nframes;
    }

    nchannels = atomic_load_explicit(&inst->user_nchannels, memory_order_acquire);

    if(nchannels != inst->nchannels) {
	float *out_v;
	int *channels_lru_start;
	out_v = arealloc(inst->out_v,
			 inst->nchannels * sizeof(float),
			 nchannels * sizeof(float));
	if(out_v == NULL) {
	    return -1;
	}
	if(inst->nchannels < nchannels) {
	    memset(out_v + inst->nchannels, 0, (nchannels - inst->nchannels) * sizeof(float));
	}
	inst->out_v = out_v;
	channels_lru_start = arealloc(inst->channels_lru_start, inst->nchannels * sizeof(int), nchannels * sizeof(int));
	if(channels_lru_start == NULL) {
	    afree(inst->out_v, nchannels * sizeof(float));
	    afree(inst->channels_lru_start, inst->nchannels * sizeof(int));
	    afree(inst->channels_lru_stop, inst->nchannels * sizeof(int));
	    inst->nchannels = 0;
	    return -1;
	}
	for(i = inst->nchannels; i < nchannels; i++) {
	    channels_lru_start[i] = -1;
	}
	inst->channels_lru_start = channels_lru_start;
	if(atryrealloc(inst->channels_lru_stop, inst->nchannels * sizeof(int), nchannels * sizeof(int))) {
	    if(nchannels > inst->nchannels) {
		/* All new channels jump the queue */
		memmove(&inst->channels_lru_stop[nchannels - inst->nchannels], inst->channels_lru_stop, inst->nchannels * sizeof(int));
	    }
	} else {
	    int *channels_lru_stop = amalloc(nchannels * sizeof(int));
	    if(channels_lru_stop == NULL) {
		afree(inst->out_v, nchannels * sizeof(float));
		afree(inst->channels_lru_start, nchannels * sizeof(int));
		afree(inst->channels_lru_stop, inst->nchannels * sizeof(int));
		inst->nchannels = 0;
		return -1;
	    }
	    if(nchannels > inst->nchannels) {
		/* All new channels jump the queue */
		memcpy(&channels_lru_stop[nchannels - inst->nchannels], inst->channels_lru_stop, inst->nchannels * sizeof(int));
	    } else {
		memcpy(channels_lru_stop, inst->channels_lru_stop, nchannels * sizeof(int));
	    }
	    afree(inst->channels_lru_stop, inst->nchannels * sizeof(int));
	    inst->channels_lru_stop = channels_lru_stop;
	}
	for(j = 0, i = inst->nchannels; i < nchannels; i++) {
	    inst->channels_lru_stop[j++] = i;
	}
	inst->nchannels = nchannels;
    }
    
    out_buffer = smaths_alloc_buffer(inst->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    ctl_buffer = smaths_alloc_buffer(inst->ctl, nframes, nchannels);
    if(ctl_buffer == NULL) {
	return -1;
    }

    memset(ctl_buffer->data, 0, sizeof(float) * nframes * nchannels);

    /* Process new commands */
    {
	struct smaths_inst_cmd_s *cmd;
	while((cmd = (struct smaths_inst_cmd_s *) aqueue_deq(&inst->cmd_queue)) != NULL) {
	    int channel;
	    if(cmd->cmd == SMATHSIC_START) {
		/* Find a suitable channel */
		if(inst->channels_lru_stop[0] != -1) {
		    channel = inst->channels_lru_stop[0];
		    memmove(inst->channels_lru_stop, inst->channels_lru_stop + 1, sizeof(int) * (nchannels - 1));
		    inst->channels_lru_stop[nchannels - 1] = -1;
		    /* Append to start */
		    for(i = 0; inst->channels_lru_start[i] != -1; i++);
		    inst->channels_lru_start[i] = channel;
		} else {
		    /* Steal first started channel */
		    channel = inst->channels_lru_start[0];
		    memmove(inst->channels_lru_start, inst->channels_lru_start + 1, sizeof(int) * (nchannels - 1));
		    inst->channels_lru_start[nchannels - 1] = channel;
		}
		ctl_buffer->data[channel] = 1.0f;
		inst->out_v[channel] = cmd->value;
		arcp_release(cmd);
	    } else { /* SMATHSIC_STOP */
		channel = -1;
		for(i = 0; i < nchannels; i++) {
		    if(inst->out_v[i] == cmd->value) {
			channel = i;
			break;
		    }
		}
		arcp_release(cmd);
		if(channel == -1) {
		    /* no such note... */
		    continue;
		}
		for(i = 0; i < nchannels; i++) {
		    if(inst->channels_lru_start[i] == channel) {
			/* Found it! */
			memmove(inst->channels_lru_start + i, inst->channels_lru_start, sizeof(int) * (nchannels - i));
			inst->channels_lru_start[i] = -1;
			for(i = 0; inst->channels_lru_stop[i] != -1; i++);
			inst->channels_lru_stop[i] = channel;
			ctl_buffer->data[channel] = -1.0f;
		    }
		}
	    }
	}
    }

    for(i = 0; i < nframes; i++) {
	memcpy(&out_buffer->data[i * nchannels], inst->out_v, sizeof(float) * nchannels);
    }
    return 0;
}

static void __smaths_inst_cmd_destroy(struct smaths_inst_cmd_s *cmd) {
    afree(cmd, sizeof(struct smaths_inst_cmd_s));
}

int smaths_inst_cmd(struct smaths_inst *inst, float value, enum smaths_inst_cmd cmd) {
    int r;
    struct smaths_inst_cmd_s *cmd_s = amalloc(sizeof(struct smaths_inst_cmd_s));
    if(cmd_s == NULL) {
	return -1;
    }
    cmd_s->cmd = cmd;
    cmd_s->value = value;
    arcp_region_init(cmd_s, (void (*)(struct arcp_region *)) __smaths_inst_cmd_destroy);
    r = aqueue_enq(&inst->cmd_queue, cmd_s);
    if(r != 0) {
	afree(cmd_s, sizeof(struct smaths_inst_cmd_s));
	return r;
    }

    return 0;
}

int smaths_inst_init(struct smaths_inst *inst, struct smaths_graph *graph, void (*destroy)(struct smaths_inst *)) {
    int r = -1;

    inst->out_v = amalloc(sizeof(float));
    if(inst->out_v == NULL) {
	goto undo0;
    }

    inst->channels_lru_start = amalloc(sizeof(int));
    if(inst->channels_lru_start == NULL) {
	goto undo1;
    }

    inst->channels_lru_stop = amalloc(sizeof(int));
    if(inst->channels_lru_stop == NULL) {
	goto undo2;
    }

    r = aqueue_init(&inst->cmd_queue);
    if(r != 0) {
	goto undo3;
    }

    r = smaths_ctlr_init(inst, graph, (gln_process_fp_t) smaths_inst_process, (void (*)(struct smaths_ctlr *)) destroy);
    if(r != 0) {
	goto undo4;
    }

    inst->out_v[0] = 0.0f;
    inst->channels_lru_start[0] = -1;
    inst->channels_lru_stop[0] = 0;
    inst->nchannels = 1;
    atomic_init(&inst->user_nchannels, 1);

    return 0;

undo4:
    aqueue_destroy(&inst->cmd_queue);
undo3:
    afree(inst->channels_lru_stop, sizeof(int));
undo2:
    afree(inst->channels_lru_start, sizeof(int));
undo1:
    afree(inst->out_v, sizeof(float));
undo0:
    return r;
}

void smaths_inst_destroy(struct smaths_inst *inst) {
    aqueue_destroy(&inst->cmd_queue);
    afree(inst->channels_lru_stop, inst->nchannels * sizeof(int));
    afree(inst->channels_lru_start, inst->nchannels * sizeof(int));
    afree(inst->out_v, inst->nchannels * sizeof(float));
    smaths_ctlr_destroy(inst);
}

static void __smaths_inst_destroy(struct smaths_inst *inst) {
    smaths_inst_destroy(inst);
    afree(inst, sizeof(struct smaths_inst));
}

struct smaths_inst *smaths_inst_create(struct smaths_graph *graph) {
    int r;
    struct smaths_inst *ret;

    ret = amalloc(sizeof(struct smaths_inst));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_inst_init(ret, graph, __smaths_inst_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_inst));
	return NULL;
    }

    return ret;
}
