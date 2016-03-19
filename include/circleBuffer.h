/*
 * circleBuffer.h
 *
 *  Created on: Jan 21, 2016
 *      Author: shapa
 */

#ifndef CIRCLEBUFFER_H_
#define CIRCLEBUFFER_H_

#include <stdint.h>

typedef struct {
	uint8_t *data;
	uint32_t size;

	uint32_t firstIdx;
	uint32_t curIdx;
	uint32_t occupied;

} CircleBuffer_t, *CircleBuffer_p;

CircleBuffer_p CircleBuffer_new(uint32_t size);
void CircleBuffer_delete(CircleBuffer_p buff);

uint32_t CircleBuffer_size(CircleBuffer_p buff);
uint32_t CircleBuffer_getOccupiedSize(CircleBuffer_p buff);
uint8_t CircleBuffer_getAt(CircleBuffer_p buff, uint32_t idx);
void CircleBuffer_pushEnd(CircleBuffer_p buff, uint8_t data);



#endif /* CIRCLEBUFFER_H_ */
