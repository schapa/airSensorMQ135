/*
 * Queue.c
 *
 *  Created on: Apr 20, 2016
 *      Author: shapa
 */

#include "Queue.h"
#include <stdlib.h>
#include <string.h>

static EventQueue_p newNode(Event_p pEvent);

EventQueue_p Queue_pushEvent(EventQueue_p pQueue, Event_p pEvent) {
	do {
		if (!pEvent)
			break;
		if (!pQueue) {
			pQueue = newNode(pEvent);
			break;
		}
		EventQueue_p node = newNode(pEvent);
		if (!node)
			break;
		if (pQueue->last) {
			pQueue->last->next = node;
			pQueue->last = node;
		} else {
			pQueue->last = node;
			pQueue->next = pQueue->last;
		}
	} while (0);
	return pQueue;
}

EventQueue_p Queue_getEvent(EventQueue_p pQueue, Event_p pEvent) {
	if (!pQueue || !pEvent)
		return pQueue;
	do {
		EventQueue_p cur = pQueue;
		*pEvent = pQueue->event;
		pQueue = pQueue->next;
		free(cur);
	} while (0);
	return pQueue;
}

static EventQueue_p newNode(Event_p pEvent) {
	EventQueue_p node = malloc(sizeof(EventQueue_t));
	if (node) {
		node->event = *pEvent;
		node->next = NULL;
		node->last = NULL;
	}
	return node;
}
