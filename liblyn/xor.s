	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "xor"
	.type "xor", %function
	.func "xor", "xor"
"xor":
	eors	r0, r1
        bx      lr
	.size "xor", .-"xor"
