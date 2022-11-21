	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "!="
	.type "!=", %function
	.func "!=", "!="
"!=":
	subs	r0, r0, r1
	rsbs    r3, r0, #1
	sbcs    r0, r0, r3
        bx      lr
	.size "!=", .-"!="
