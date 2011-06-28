#include <graphline.h>
#include "sonicmaths/graph.h"

int smaths_graph_init(struct smaths_graph *graph, gln_process_fp_t process, void *arg, size_t buffer_nmemb, float sample_rate) {
    int r;

    graph->sample_rate = sample_rate;

    r = gln_graph_init(&graph->graph, buffer_nmemb, sizeof(float));
    if(r != 0) {
	return r;
    }

    r = gln_node_init(&graph->node, &graph->graph, process, arg);
    if(r != 0) {
	gln_graph_destroy(&graph->graph);
	return r;
    }

    r = atomic_list_init(&graph->from_outside_sockets);
    if(r != 0) {
	gln_node_destroy(&graph->node);
	gln_graph_destroy(&graph->graph);
	return r;
    }
    r = atomic_list_init(&graph->to_outside_sockets);
    if(r != 0) {
	atomic_list_destroy(&graph->from_outside_sockets);
	gln_node_destroy(&graph->node);
	gln_graph_destroy(&graph->graph);
	return r;
    }
    r = atomic_list_init(&graph->run_first);
    if(r != 0) {
	atomic_list_destroy(&graph->to_outside_sockets);
	atomic_list_destroy(&graph->from_outside_sockets);
	gln_node_destroy(&graph->node);
	gln_graph_destroy(&graph->graph);
	return r;
    }
    return 0;
}

void smaths_graph_destroy(struct smaths_graph *graph) {
    while(1) {
	struct smaths_graph_socketpair *s = atomic_list_last(&graph->to_outside_sockets);
	if(s == ALST_EMPTY) {
	    break;
	}
	atomic_list_remove_by_value(&graph->to_outside_sockets, s);
	gln_socket_disconnect(&s->socket);
	free(s);
    }

    while(1) {
	struct smaths_graph_socketpair *s = atomic_list_last(&graph->from_outside_sockets);
	if(s == ALST_EMPTY) {
	    break;
	}
	atomic_list_remove_by_value(&graph->from_outside_sockets, s);
	gln_socket_disconnect(&s->socket);
	free(s);
    }

    atomic_list_destroy(&graph->run_first);
    atomic_list_destroy(&graph->to_outside_sockets);
    atomic_list_destroy(&graph->from_outside_sockets);
    gln_node_destroy(&graph->node);
    gln_graph_destroy(&graph->graph);
}

int smaths_graph_process(struct smaths_graph *graph) {
    int r;
    atomic_iterator_t i;
    r = atomic_iterator_init(&graph->run_first, &i);
    if(r != 0) {
	return r;
    }
    struct smaths_graph_process_closure *f;
    while((f = atomic_iterator_next(&graph->run_first, &i)) != ALST_EMPTY) {
	f->func(f->arg);
    }
    atomic_iterator_destroy(&graph->run_first, &i);
    return 0;
}
