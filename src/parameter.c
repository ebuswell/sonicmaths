/*
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

#include <math.h>
#include <graphline.h>
#include "sonicmaths/parameter.h"

int smaths_parameter_init(struct smaths_parameter *p, struct gln_node *node, float value) {
    int r;

    r = gln_socket_init(&p->p_dynamic, node, INPUT);
    if(r != 0) {
	return r;
    }

    r = gln_socket_init(&p->p_static, node, INPUT);
    if(r != 0) {
	return r;
    }

    atomic_float_set(&p->value, value);

    return 0;
}

void smaths_parameter_destroy(struct smaths_parameter *p) {
    gln_socket_destroy(&p->p_static);
    gln_socket_destroy(&p->p_dynamic);
}

float *smaths_parameter_get_buffer(struct smaths_parameter *p) {
    float value = atomic_float_read(&p->value);
    if(isnanf(value)) {
	return gln_socket_get_buffer(&p->p_dynamic);
    } else if(gln_socket_ready(&p->p_static)) {
	return gln_socket_get_buffer(&p->p_static);
    } else {
	float *buffer = gln_socket_alloc_buffer(&p->p_static);
	size_t i;
	for(i = 0; i < p->p_static.graph->buffer_nmemb; i++) {
	    buffer[i] = value;
	}
	return buffer;
    }
}

/* int smaths_parameter_set(struct smaths_parameter *p, float value) { */
/*     if(p->sched->offset >= 0) { */
/* 	/\* called from within the scheduler *\/ */
/* 	float *buffer; */
/* 	int i; */
/* 	if(!gln_socket_ready(p->p_static)) { */
/* 	    /\* we were dynamic until now *\/ */
/* 	    float *old_buffer = gln_socket_get_buffer(p->p_dynamic); */
/* 	    if(old_buffer == NULL) { */
/* 		return -1; */
/* 	    } */
/* 	    buffer = gln_socket_alloc_buffer(p->p_static); */
/* 	    if(buffer == NULL) { */
/* 		return -1; */
/* 	    } */
/* 	    memcpy(buffer, old_buffer, sizeof(float) * p->sched->offset); */
/* 	} else { */
/* 	    buffer = (float *) gln_socket_get_buffer(p->p_static); */
/* 	} */
/* 	for(i = p->sched->offset; i < p->p_static->graph->buffer_nmem; i++) { */
/* 	    buffer[i] = value; */
/* 	} */
/*     } */
/*     atomic_float_set(&p->value, value); */
/*     return 0; */
/* } */

/* int smaths_parameter_connect(struct smaths_parameter *p, struct gln_socket *other) { */
/*     if(p->sched->offset >= 0) { */
/* 	/\* called from within the scheduler *\/ */
/* 	float *buffer; */
/* 	float *new_buffer; */
/* 	if(!gln_socket_ready(p->p_static)) { */
/* 	    float *old_buffer = gln_socket_get_buffer(p->p_dynamic); */
/* 	    buffer = gln_socket_alloc_buffer(p->p_static); */
/* 	    if(buffer == NULL) { */
/* 		return -1; */
/* 	    } */
/* 	    memcpy(buffer, old_buffer, sizeof(float) * p->sched->offset); */
/* 	    int r = gln_socket_connect(p->p_dynamic, other); */
/* 	    if(r != 0) { */
/* 		memcpy(buffer + p->sched->offset, old_buffer + p->sched->offset, sizeof(float) * (p->p_static->graph->buffer_nmem - p->sched->offset)); */
/* 		return r; */
/* 	    } */
/* 	    new_buffer = gln_socket_reget_buffer(p->p_dynamic); */
/* 	    if(new_buffer == NULL) { */
/* 		memcpy(buffer + p->sched->offset, old_buffer + p->sched->offset, sizeof(float) * (p->p_static->graph->buffer_nmem - p->sched->offset)); */
/* 		return -1; */
/* 	    } */
/* 	} else { */
/* 	    buffer = gln_socket_get_buffer(p->p_static); */
/* 	    int r = gln_socket_connect(p->p_dynamic, other); */
/* 	    if(r != 0) { */
/* 		return r; */
/* 	    } */
/* 	    new_buffer = gln_socket_reget_buffer(p->p_dynamic); */
/* 	    if(new_buffer == NULL) { */
/* 		return -1; */
/* 	    } */
/* 	} */
/* 	memcpy(buffer + p->sched->offset, new_buffer + p->sched->offset, sizeof(float) * (p->p_static->graph->buffer_nmem - p->sched->offset)); */
/*     } else { */
/* 	int r = gln_socket_connect(p->p_dynamic, other); */
/* 	if(r != 0) { */
/* 	    return r; */
/* 	} */
/*     } */
/*     atomic_float_set(&p->value, NAN); */
/*     return 0; */
/* } */

/* int smaths_setup_parameter(struct smaths_parameter *p) { */
/*     float value = atomic_float_read(&p->value); */
/*     if(!isnanf(value)) { */
/* 	float *buffer = (float *) gln_socket_alloc_buffer(p->p_static); */
/* 	if(buffer == NULL) { */
/* 	    return -1; */
/* 	} */
/* 	int i; */
/* 	for(i = 0; i < p->p_static->graph->buffer_nmem; i++) { */
/* 	    buffer[i] = value; */
/* 	} */
/* 	return 0; */
/*     } */
/* } */
