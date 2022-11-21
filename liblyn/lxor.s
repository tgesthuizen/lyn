	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "lxor"
	.type "lxor", %function
	.func "lxor", "lxor"
"lxor":
	eors    r0, r1
        bx      lr
	.size "lxor", .-"lxor"
