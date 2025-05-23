#include "main.h"

/*
* rc4.c
*
* Copyright (c) 1996-2000 Whistle Communications, Inc.
* All rights reserved.
*
* Subject to the following obligations and disclaimer of warranty, use and
* redistribution of this software, in source or object code forms, with or
* without modifications are expressly permitted by Whistle Communications;
* provided, however, that:
* 1. Any and all reproductions of the source or object code must include the
*    copyright notice above and the following disclaimer of warranties; and
* 2. No rights are granted, in any manner or form, to use Whistle
*    Communications, Inc. trademarks, including the mark "WHISTLE
*    COMMUNICATIONS" on advertising, endorsements, or otherwise except as
*    such appears in the above copyright notice or in the software.
*
* THIS SOFTWARE IS BEING PROVIDED BY WHISTLE COMMUNICATIONS "AS IS", AND
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, WHISTLE COMMUNICATIONS MAKES NO
* REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED, REGARDING THIS SOFTWARE,
* INCLUDING WITHOUT LIMITATION, ANY AND ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
* WHISTLE COMMUNICATIONS DOES NOT WARRANT, GUARANTEE, OR MAKE ANY
* REPRESENTATIONS REGARDING THE USE OF, OR THE RESULTS OF THE USE OF THIS
* SOFTWARE IN TERMS OF ITS CORRECTNESS, ACCURACY, RELIABILITY OR OTHERWISE.
* IN NO EVENT SHALL WHISTLE COMMUNICATIONS BE LIABLE FOR ANY DAMAGES
* RESULTING FROM OR ARISING OUT OF ANY USE OF THIS SOFTWARE, INCLUDING
* WITHOUT LIMITATION, ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* PUNITIVE, OR CONSEQUENTIAL DAMAGES, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES, LOSS OF USE, DATA OR PROFITS, HOWEVER CAUSED AND UNDER ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF WHISTLE COMMUNICATIONS IS ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
*
* $FreeBSD: src/sys/crypto/rc4/rc4.c,v 1.2.2.1 2000/04/18 04:48:31 archie Exp $
*/


void swap_bytes(unsigned char *a, unsigned char *b)
{
	unsigned char temp;
	temp = *a;
	*a = *b;
	*b = temp;
}


void rc4_init(rc4_state *const state, const unsigned char *key, int keylen)
{
	unsigned char j;
	int i;

	/* Initialize state with identity permutation */
	for (i = 0; i < 256; i++)
		state->perm[i] = (unsigned char)i;
	state->index1 = 0;
	state->index2 = 0;

	/* Randomize the permutation using key data */
	for (j = i = 0; i < 256; i++) {
		j += state->perm[i] + key[i % keylen];
		swap_bytes(&state->perm[i], &state->perm[j]);
	}
}


void rc4_crypt(rc4_state *const state, const unsigned char *inbuf, unsigned char *outbuf, int buflen)
{
	int i;
	unsigned char j;

	for (i = 0; i < buflen; i++) {

		/* Update modification indicies */
		state->index1++;
		state->index2 += state->perm[state->index1];

		/* Modify permutation */
		swap_bytes(&state->perm[state->index1],
			&state->perm[state->index2]);

		/* Encrypt/decrypt next byte */
		j = state->perm[state->index1] + state->perm[state->index2];
		outbuf[i] = inbuf[i] ^ state->perm[j];
	}
}
