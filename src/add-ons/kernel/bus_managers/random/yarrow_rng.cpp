/* Yarrow Random Number Generator (True Randomness Achieved in Software) *
 * Copyright (c) 1998-2000 by Yarrow Charnot, Identikey <mailto://ycharnot@identikey.com>
 * All Lefts, Rights, Ups, Downs, Forwards, Backwards, Pasts and Futures Reserved *
 */

/* Made into a BeOS /dev/random and /dev/urandom by Daniel Berlin */


#include "yarrow_rng.h"

#include <stdlib.h>
#include <thread.h>


//#define TRACE_DRIVER
#ifdef TRACE_DRIVER
#	define TRACE(x...) dprintf("random: " x)
#else
#	define TRACE(x...) ;
#endif
#define CALLED() 			TRACE("CALLED %s\n", __PRETTY_FUNCTION__)


#define rotr32(x, n) ((((uint32)(x)) >> ((int) ((n) & 31))) | (((uint32)(x)) << ((int) ((32 - ((n) & 31))))))
#define rotl32(x, n) ((((uint32)(x)) << ((int) ((n) & 31))) | (((uint32)(x)) >> ((int) ((32 - ((n) & 31))))))

#define bswap32(x) \
	((rotl32((uint32)(x), 8) & 0x00ff00ff) | (rotr32((uint32)(x), 8) & 0xff00ff00))

typedef union _OCTET {
	uint64	Q[1];
	uint32	D[2];
	uint16	W[4];
	uint8	B[8];
} OCTET;

#define NK	257			/* internal state size */
#define NI	120			/* seed in increment */
#define NA	70			/* rand out increment A */
#define NB	139			/* rand out increment B */

typedef struct _ch_randgen {
	OCTET	ira[NK];	/* numbers live here */
	OCTET	*seedptr;	/* next seed pointer */
	OCTET	*rndptrA;	/* randomizing pointer #1 */
	OCTET	*rndptrB;	/* randomizing pointer #2 */
	OCTET	*rndptrX;	/* main randout pointer */
	OCTET	rndLeft;	/* left rand accumulator */
	OCTET	rndRite;	/* rite rand accumulator */
} ch_randgen;


static ch_randgen *sRandomEnv;
static uint32 sRandomCount = 0;

extern void hash_block(const unsigned char *block, const unsigned int block_byte_size, unsigned char *md);


#define HASH_BITS			160		/* I use Tiger. Modify it to match your HASH */
#define HASH_BLOCK_BITS		512		/* I use Tiger. Modify it to match your HASH */
#define HASH_BYTES			(HASH_BITS / 8)
#define HASH_BLOCK_BYTES	(HASH_BLOCK_BITS / 8)
#define HASH_OCTETS			(HASH_BITS / 64)
#define HASH_BLOCK_OCTETS	(HASH_BLOCK_BITS / 64)


/* attach by Yarrow Charnot. attaches x to y. can be seen as about 2-3 rounds of RC6 encryption
 */

static inline void
attach(OCTET *y, const OCTET *x, const uint32 anyA, const uint32 anyB,
	const uint32 oddC, const uint32 oddD)
{
	OCTET _x;
	OCTET _y;

	_x.D[0] = x->D[0];
	_x.D[1] = x->D[1];
	_y.D[0] = y->D[0];
	_y.D[1] = y->D[1];
	_x.D[0] = rotl32(((bswap32(_x.D[0]) | 1) * x->D[1]), 5);
	_x.D[1] = rotl32((bswap32(_x.D[1]) | 1) * x->D[0], 5);
	_y.D[0] = (bswap32(rotl32(_y.D[0] ^ _x.D[0], _x.D[1])) + anyA) * oddC;
	_y.D[1] = (bswap32(rotl32(_y.D[1] ^ _x.D[1], _x.D[0])) + anyB) * oddD;
	y->D[1] = _y.D[0];
	y->D[0] = _y.D[1];
}


/** detach by Yarrow Charnot. detaches x from y. can be seen as about
 *	2-3 rounds of RC6 decryption.
 */

static inline void
detach(OCTET *y, const OCTET *x, const uint32 sameA, const uint32 sameB,
	const uint32 invoddC, const uint32 invoddD)
{
	OCTET _x;
	OCTET _y;

	_x.D[0] = x->D[0];
	_x.D[1] = x->D[1];
	_y.D[0] = y->D[1];
	_y.D[1] = y->D[0];
	_x.D[0] = rotl32((bswap32(_x.D[0]) | 1) * x->D[1], 5);
	_x.D[1] = rotl32((bswap32(_x.D[1]) | 1) * x->D[0], 5);
	_y.D[0] = rotr32(bswap32(_y.D[0] * invoddC - sameA), _x.D[1]) ^ _x.D[0];
	_y.D[1] = rotr32(bswap32(_y.D[1] * invoddD - sameB), _x.D[0]) ^ _x.D[1];
	y->D[0] = _y.D[0];
	y->D[1] = _y.D[1];
}


/**	QUICKLY seeds in a 64 bit number, modified so that a subsequent call really
 *	"stirs" in another seed value (no bullshit XOR here!)
 */

static inline void
chseed(ch_randgen *prandgen, const uint64 seed)
{
	prandgen->seedptr += NI;
	if (prandgen->seedptr >= (prandgen->ira + NK))
		prandgen->seedptr -= NK;

	attach(prandgen->seedptr, (OCTET *) &seed, 0x213D42F6U, 0x6552DAF9U,
		0x2E496B7BU, 0x1749A255U);
}


/**	The heart of Yarrow 2000 Chuma Random Number Generator: fast and reliable
 *	randomness collection.
 *	Thread yielding function is the most OPTIMAL source of randomness combined
 *	with a clock counter.
 *	It doesn't have to switch to another thread, the call itself is random enough.
 *	Test it yourself.
 *	This FASTEST way to collect minimal randomness on each step couldn't use the
 *	processor any LESS. Even functions based on just creation of threads and their
 *	destruction can not compare by speed.
 *	Temporary file creation is just a little extra thwart to bewilder the processor
 *	cache and pipes.
 *	If you make clock_counter() (system_time()) return all 0's, still produces a
 *	stream indistinguishable from random.
 */

static void
reseed(ch_randgen *prandgen, const uint32 initTimes)
{
	volatile uint32 i, j;
	OCTET x, y;

	x.Q[0] = 0;

	for (j = initTimes; j; j--) {
		for (i = NK * initTimes; i; i--) {
			// TODO: Yielding sounds all nice in principle, but this will take
			// ages (at least initTimes * initTimes * NK * quantum, i.e. ca. 49s
			// for initTimes == 8) in a busy system. Since perl initializes its
			// random seed on startup by reading from /dev/urandom, perl
			// programs are all but unusable when at least one other thread
			// hogs the CPU.
			thread_yield();

			// TODO: Introduce a clock_counter() function that directly returns
			// the value of the hardware clock counter. This will be cheaper
			// and will yield more randomness.
			y.Q[0] += system_time();
			attach(&x, &y, 0x52437EFFU, 0x026A4CEBU, 0xD9E66AC9U, 0x56E5A975U);
			attach(&y, &x, 0xC70B8B41U, 0x9126B036U, 0x36CC6FDBU, 0x31D477F7U);
			chseed(prandgen, y.Q[0]);
		}
	}
}


/* returns a 64 bit of Yarrow 2000 Chuma RNG random number */

static inline uint64
chrand(ch_randgen *prandgen)
{
	prandgen->rndptrX++;
	prandgen->rndptrA += NA;
	prandgen->rndptrB += NB;
	if (prandgen->rndptrX >= (prandgen->ira + NK)) {
		prandgen->rndptrX -= NK;
		reseed (prandgen, 1);
	}

	if (prandgen->rndptrA >= (prandgen->ira + NK))
		prandgen->rndptrA -= NK;
	if (prandgen->rndptrB >= (prandgen->ira + NK))
		prandgen->rndptrB -= NK;

	attach(&prandgen->rndLeft, prandgen->rndptrX, prandgen->rndptrA->D[0],
		prandgen->rndptrA->D[1], 0x49A3BC71UL, 0x60E285FDUL);
	attach(&prandgen->rndRite, &prandgen->rndLeft, prandgen->rndptrB->D[0],
		prandgen->rndptrB->D[1], 0xC366A5FDUL, 0x20C763EFUL);

	chseed(prandgen, prandgen->rndRite.Q[0]);

	return prandgen->rndRite.Q[0] ^ prandgen->rndLeft.Q[0];
}


/** returns a 32 bit random number */

static inline uint32
chrand32(ch_randgen *prandgen)
{
	OCTET r = {{chrand(prandgen)}};
	return r.D[0] ^ r.D[1];
}


/** returns an 8 bit random number */

static inline uint8
chrand8(ch_randgen *prandgen)
{
	OCTET r = {{chrand(prandgen)}};
	return r.B[0] ^ r.B[1] ^ r.B[2] ^ r.B[3] ^ r.B[4] ^ r.B[5] ^ r.B[6] ^ r.B[7];
}

/* generates a cryptographically secure random big number 0 <= x < 32^n */
/* automatically reseeds if necessary or if requested 1/16 of the internal state or more */
/*
   __inline void bigrand (ch_randgen *prandgen, unsigned __int32 *x, unsigned __int32 n)
   {
   unsigned int i;
   OCTET block[HASH_BLOCK_OCTETS];
   OCTET hash[HASH_OCTETS];
   OCTET *j;
   if (n >= NK/8) reseed (prandgen, 1);
   for (*x++ = n; (signed) n > 0; )
   {
   for (i = 0; i < HASH_BLOCK_OCTETS; i++) block->Q[i] += chrand (prandgen) + hash
   ->Q[i % HASH_OCTETS];
   hash_block (block->B, HASH_BLOCK_BYTES, hash->B);
   for (i = HASH_OCTETS, j = hash; i && ((signed) n > 0); i--, j++, x += 2, n -= 2)
   {
   attach ((OCTET *) &x, j, 0x0AEF7ED2U, 0x3F85C5C1U, 0xD3EFB373U,
   0x13ECF0B9U);
   }
   }
   }
 */


/** Initializes Yarrow 2000 Chuma Random Number Generator.
 *	Reseeding about 8 times prior to the first use is recommended.
 *	More than 16 will probably be a bit too much as time increases
 *	by n^2.
 */

static ch_randgen *
new_chrand(const unsigned int inittimes)
{
	ch_randgen *prandgen;

	prandgen = (ch_randgen *)malloc(sizeof(ch_randgen));
	if (prandgen == NULL)
		return NULL;

	prandgen->seedptr = prandgen->ira;
	prandgen->rndptrX = prandgen->ira;
	prandgen->rndptrA = prandgen->ira;
	prandgen->rndptrB = prandgen->ira;
	prandgen->rndLeft.Q[0] = 0x1A4B385C72D69E0FULL;
	prandgen->rndRite.Q[0] = 0x9C805FE7361A42DBULL;
	reseed (prandgen, inittimes);

	prandgen->seedptr = prandgen->ira + chrand (prandgen) % NK;
	prandgen->rndptrX = prandgen->ira + chrand (prandgen) % NK;
	prandgen->rndptrA = prandgen->ira + chrand (prandgen) % NK;
	prandgen->rndptrB = prandgen->ira + chrand (prandgen) % NK;
	return prandgen;
}


/** Clean up after chuma */

static void
kill_chrand(ch_randgen *randgen)
{
	free(sRandomEnv);
}


//	#pragma mark -


status_t
yarrow_init()
{
	CALLED();
	sRandomEnv = new_chrand(8);
	if (sRandomEnv == NULL)
		return B_NO_MEMORY;
	return B_OK;
}


void
yarrow_uninit()
{
	CALLED();
	kill_chrand(sRandomEnv);
}


status_t
yarrow_rng_read(void* cookie, void *_buffer, size_t *_numBytes)
{
	if (!IS_USER_ADDRESS(_buffer))
		return B_BAD_ADDRESS;

	sRandomCount += *_numBytes;

	/* Reseed if we have or are gonna use up > 1/16th the entropy around */
	if (sRandomCount >= NK/8) {
		sRandomCount = 0;
		reseed(sRandomEnv, 1);
	}

	/* ToDo: Yes, i know this is not the way we should do it. What we really should do is
	 * take the md5 or sha1 hash of the state of the pool, and return that. Someday.
	 */
	int32 *buffer = (int32 *)_buffer;
	uint32 i;
	for (i = 0; i < *_numBytes / 4; i++) {
		int32 data = chrand32(sRandomEnv);
		if (user_memcpy(&buffer[i], &data, sizeof(data)) < B_OK)
			return B_BAD_ADDRESS;
	}
	uint8 *buffer8 = (uint8 *)_buffer;
	for (uint32 j = 0; j < *_numBytes % 4; j++) {
		int8 data = chrand8(sRandomEnv);
		if (user_memcpy(&buffer8[(i * 4) + j], &data, sizeof(data)) < B_OK)
			return B_BAD_ADDRESS;
	}
	return B_OK;
}


status_t
yarrow_rng_write(void* cookie, const void *_buffer, size_t *_numBytes)
{
	OCTET *buffer = (OCTET*)_buffer;

	if (!IS_USER_ADDRESS(buffer))
		return B_BAD_ADDRESS;

	for (size_t i = 0; i < *_numBytes / sizeof(OCTET); i++) {
		OCTET data;
		if (user_memcpy(&data, buffer, sizeof(data)) < B_OK)
			return B_BAD_ADDRESS;
		chseed(sRandomEnv, data.Q[0]);
		buffer++;
	}
	return B_OK;
}


void
yarrow_enqueue_randomness(const uint64 value)
{
	CALLED();
	chseed(sRandomEnv, value);
}
