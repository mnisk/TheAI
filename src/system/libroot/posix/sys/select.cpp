/*
 * Copyright 2002-2007, Axel Dörfler, axeld@pinc-software.de.
 * All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <sys/select.h>

#include <errno.h>
#include <pthread.h>

#include <syscall_utils.h>

#include <errno_private.h>
#include <signal_private.h>
#include <symbol_versioning.h>
#include <syscalls.h>
#include <time_private.h>


extern "C" int __pselect_beos(int numBits, struct fd_set *readBits,
	struct fd_set *writeBits, struct fd_set *errorBits,
	const struct timespec *tv, const sigset_t *beosSignalMask);
extern "C" int __pselect(int numBits, struct fd_set *readBits, struct fd_set *writeBits,
	struct fd_set *errorBits, const struct timespec *tv,
	const sigset_t *sigMask);


int
__pselect_beos(int numBits, struct fd_set *readBits, struct fd_set *writeBits,
	struct fd_set *errorBits, const struct timespec *tv,
	const sigset_t *beosSignalMask)
{
	int status;
	sigset_t signalMask;
	bigtime_t timeout = -1LL;
	if (tv != NULL && !timespec_to_bigtime(*tv, timeout))
		RETURN_AND_SET_ERRNO_TEST_CANCEL(EINVAL);

	if (beosSignalMask != NULL)
		signalMask = from_beos_sigset(*beosSignalMask);

	status = _kern_select(numBits, readBits, writeBits, errorBits, timeout,
		beosSignalMask != NULL ? &signalMask : NULL);

	RETURN_AND_SET_ERRNO_TEST_CANCEL(status);
}


int
__pselect(int numBits, struct fd_set *readBits, struct fd_set *writeBits,
	struct fd_set *errorBits, const struct timespec *tv,
	const sigset_t *sigMask)
{
	int status;
	bigtime_t timeout = -1LL;
	if (tv != NULL && !timespec_to_bigtime(*tv, timeout))
		RETURN_AND_SET_ERRNO_TEST_CANCEL(EINVAL);

	status = _kern_select(numBits, readBits, writeBits, errorBits, timeout,
		sigMask);

	RETURN_AND_SET_ERRNO_TEST_CANCEL(status);
}


int
select(int numBits, struct fd_set *readBits, struct fd_set *writeBits,
	struct fd_set *errorBits, struct timeval *tv)
{
	int status;
	bigtime_t timeout = -1LL;
	if (tv != NULL && !timeval_to_bigtime(*tv, timeout))
		RETURN_AND_SET_ERRNO_TEST_CANCEL(EINVAL);

	status = _kern_select(numBits, readBits, writeBits, errorBits, timeout,
		NULL);

	RETURN_AND_SET_ERRNO_TEST_CANCEL(status);
}


DEFINE_LIBROOT_KERNEL_SYMBOL_VERSION("__pselect_beos", "pselect@", "BASE");

DEFINE_LIBROOT_KERNEL_SYMBOL_VERSION("__pselect", "pselect@@", "1_ALPHA4");
