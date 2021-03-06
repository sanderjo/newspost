/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Author: William McBrine <wmcbrine@users.sf.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

/* yencode for newspost
 * Loosely based on the GPL'ed uuencode.c (q.v.), and on the public domain
 * yencode.c by Juergen Helbing.
 */

#include "../base/newspost.h"
#include "../cksfv/sfv.h"
#include "../ui/ui.h"
#include "yencode.h"

/**
*** Public Routines
**/

long yencode(FILE *infile, char *outbuf, long psize, n_uint32 *crc)
{
	long counter;
	int n, ylinepos;
	unsigned char *p, *ch, c;
	unsigned char inbuf[YENC_LINE_LENGTH];

	ylinepos = 0;
	n = 0;
	counter = 0;

	ch = (unsigned char *) outbuf;
	p = inbuf;

	while (counter < psize) {

		if (n == 0) {
			n = fread(inbuf, 1, YENC_LINE_LENGTH, infile);
			p = inbuf;
			*crc = crc32((char *) inbuf, n, *crc);
		}

		if (ylinepos >= YENC_LINE_LENGTH) {
			*ch++ = '\r';
			*ch++ = '\n';
			ylinepos = 0;
		}

		c = *p + 42;

		switch (c) {
		case '.':
			if (ylinepos > 0)
				break;
		case '\0':
		case 9:
		case '\n':
		case '\r':
		case '=':
			*ch++ = '=';
			c += 64;
			ylinepos++;
		}

		*ch++ = c;

		counter++;
		ylinepos++;
		n--;
		p++;
	}

	*ch++ = '\r';
	*ch++ = '\n';
	*ch = '\0';

	if (ferror(infile)) {
		ui_generic_error(errno);
		return 0;
	}

	return ch - (unsigned char *) outbuf;
}
