// This function is a precompiled version of how libgcc provides
// div. Therefore you find the libgcc copyright below:

/* Copyright (C) 2000-2022 Free Software Foundation, Inc.
This file is part of GCC.
GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.
GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.
Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.
You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

	.text
	.global	"/"
	.syntax unified
	.code	16
	.thumb_func
	.type	"/", %function
"/":
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r2, r0
	movs	r3, #1
	cmp	r0, r1
	bhi	.L19
	b	.L2
.L21:
	lsls	r1, r1, #1
	lsls	r3, r3, #1
	cmp	r1, r0
	bcs	.L3
	cmp	r3, #0
	beq	.L10
.L19:
	cmp	r1, #0
	bge	.L21
.L2:
	movs	r0, #0
.L7:
	cmp	r1, r2
	bhi	.L6
	subs	r2, r2, r1
	orrs	r0, r3
.L6:
	lsrs	r3, r3, #1
	lsrs	r1, r1, #1
	cmp	r3, #0
	bne	.L7
.L1:
	@ sp needed
	bx	lr
.L3:
	cmp	r3, #0
	bne	.L2
.L10:
	movs	r0, #0
	b	.L1
	.size	"/", .-"/"
	.ident	"GCC: (GNU) 12.2.0"
