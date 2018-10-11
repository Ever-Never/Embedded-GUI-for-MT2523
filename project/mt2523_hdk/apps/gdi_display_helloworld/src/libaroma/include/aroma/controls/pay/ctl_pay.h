/*
 * ctl_clock.h
 *
 *  Created on: 2016年8月7日
 *      Author: fine
 */

#ifndef __libaroma_aroma_h__
	#error "Include <aroma.h> instead."
#endif

#ifndef INCLUDE_AROMA_CONTROLS_CTL_PAY_H_
#define INCLUDE_AROMA_CONTROLS_CTL_PAY_H_

LIBAROMA_CONTROLP libaroma_ctl_pay(LIBAROMA_WINDOWP win, word id, int x,
		int y, int w, int h, const char * text, byte clock_style);

#endif /* INCLUDE_AROMA_CONTROLS_CTL_CLOCK_H_ */
