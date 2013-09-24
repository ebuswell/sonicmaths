/*
 * buffer.c
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

#include <graphline.h>
#include "sonicmaths/buffer.h"

struct smaths_buffer *smaths_alloc_buffer(struct gln_socket *socket, int nframes, int nchannels) {
    struct smaths_buffer *buffer = gln_alloc_buffer(socket, SMATHS_BUFFER_SIZE(nframes, nchannels));
    if(buffer == NULL) {
	return NULL;
    }
    buffer->nframes = nframes;
    buffer->nchannels = nchannels;
    return buffer;
}
