/*
 * Copyright 2022, Haiku Inc. All rights reserved.
 * Copyright 2018, Jaroslaw Pelczar <jarek@jpelczar.com>
 * Distributed under the terms of the MIT License.
 */
#ifndef _KERNEL_ARCH_ARM64_ARCH_THREAD_H_
#define _KERNEL_ARCH_ARM64_ARCH_THREAD_H_


#include <arch/cpu.h>


#ifdef __cplusplus
extern "C" {
#endif

void arm64_push_iframe(struct iframe_stack *stack, struct iframe *frame);
void arm64_pop_iframe(struct iframe_stack *stack);

static inline Thread * arch_thread_get_current_thread(void)
{
	return (Thread *)READ_SPECIALREG(tpidr_el1);
}


static inline void arch_thread_set_current_thread(Thread *t)
{
	WRITE_SPECIALREG(tpidr_el1, t);
}


#ifdef __cplusplus
}
#endif


#endif /* _KERNEL_ARCH_ARM64_ARCH_THREAD_H_ */
