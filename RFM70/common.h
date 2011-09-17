/*
 * common.h
 *
 *  Created on: 08-09-2011
 *      Author: eagle
 */

#ifndef COMMON_H_
#define COMMON_H_



#define LED_1		D, 7
#define LED_2		D, 6
#define LED_3		B, 0









#define GLUE(a, b) a##b

#define SET_(what, p, m) GLUE(what, p) |= (1 << (m))
#define CLR_(what, p, m) GLUE(what, p) &= ~(1 << (m))
#define TOG_(what, p, m) GLUE(what, p) ^= (1 << (m))
#define GET_(p, m) GLUE(PIN, p) & (1 << (m))
#define SET(what, x) SET_(what, x)
#define CLR(what, x) CLR_(what, x)
#define TOG(what, x) TOG_(what, x)





#endif /* COMMON_H_ */
