	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "-"
	.type "-", %function
	.func "-", "-"
"-":
	subs	r0, r0, r1
        bx      lr
	.size "-", .-"-"
