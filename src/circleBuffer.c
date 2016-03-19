/*
 * circleBuffer.c
 *
 *  Created on: Jan 21, 2016
 *      Author: shapa
 */


#include "circleBuffer.h"
#include <stddef.h>
#include <stdlib.h>


CircleBuffer_p CircleBuffer_new(uint32_t size) {
	CircleBuffer_p newBuff = NULL;

	do {
		newBuff = (CircleBuffer_p)malloc(sizeof(CircleBuffer_t));
		if (!newBuff)
			return NULL;
		newBuff->data = (uint32_t*)malloc(size*sizeof(*newBuff->data));
		if (!newBuff->data) {
			CircleBuffer_delete(newBuff);
			return NULL;
		}
		newBuff->size = size;
		newBuff->firstIdx = 0;
		newBuff->curIdx = 0;
		newBuff->occupied = 0;

	} while (0);

	return newBuff;
}
void CircleBuffer_delete(CircleBuffer_p buff) {
	if (buff && buff->data)
		free(buff->data);
	if (buff)
		free(buff);
}

uint32_t CircleBuffer_size(CircleBuffer_p buff) {
	uint32_t size = 0;
	if (buff && buff->data) {
		size = buff->size;
	}
	return size;
}

uint32_t CircleBuffer_getOccupiedSize(CircleBuffer_p buff) {
	uint32_t size = 0;
	if (buff && buff->data) {
		size = buff->occupied;
	}
	return size;
}

uint8_t CircleBuffer_getAt(CircleBuffer_p buff, uint32_t idx) {
	uint8_t value = 0;
	uint32_t maxIdx = 0;

	do {
		if (!buff || idx >= buff->occupied) {
			break;
		}
		maxIdx = buff->occupied - 1;
		idx += buff->firstIdx;
		if (idx > maxIdx) {
			idx -= buff->occupied;
		}
		value = buff->data[idx];
	} while (0);

	return value;
}

void CircleBuffer_pushEnd(CircleBuffer_p buff, uint8_t data) {

	if (!buff || !buff->size) {
		return;
	}

	buff->data[buff->curIdx++] = data;

	if (buff->curIdx >= buff->size) {
		buff->curIdx = 0;
	}

	if (buff->occupied < buff->size) {
		buff->occupied++;
	} else {
		if (++buff->firstIdx >= buff->size) {
			buff->firstIdx = 0;
		}
	}
}
