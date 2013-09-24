/** @file buffer.h
 *
 * A Sonic Maths buffer object.
 */
/*
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
#ifndef SONICMATHS_BUFFER_H
#define SONICMATHS_BUFFER_H 1

#include <graphline.h>

struct smaths_buffer {
    int nframes;
    int nchannels;
    float data[];
};

#define SMATHS_BUFFER_OVERHEAD (sizeof(struct smaths_buffer))

#define SMATHS_BUFFER_SIZE(nframes, nchannels) (nframes * nchannels * sizeof(float) + SMATHS_BUFFER_OVERHEAD)

struct smaths_buffer *smaths_alloc_buffer(struct gln_socket *socket, int nframes, int nchannels);

static inline float smaths_value(struct smaths_buffer *buf, int frame, int channel, float value) {
    if(buf == NULL) {
	return value;
    }
    int nchannels = buf->nchannels;
    if(nchannels <= channel) {
	/* use first channel. */
	return buf->data[nchannels * frame];
    }
    return buf->data[nchannels * frame + channel];
}

#define SMATHS_MAX_NCHANNELS(n, ...) ({					\
	    struct smaths_buffer *__smn_buf[n] = { __VA_ARGS__ };	\
	    int __smn_nchannels = 1;					\
	    int __smn_i;						\
	    for(__smn_i = 0; __smn_i < n; __smn_i++) {			\
		if((__smn_buf[__smn_i] != NULL)				\
		   && (__smn_buf[__smn_i]->nchannels > __smn_nchannels)) { \
		    __smn_nchannels = __smn_buf[__smn_i]->nchannels;	\
		}							\
	    }								\
	    __smn_nchannels;						\
	})

#endif /* ! SONICMATHS_BUFFER_H */
