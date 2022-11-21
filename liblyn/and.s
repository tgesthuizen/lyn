	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "and"
	.type "and", %function
	.func "and", "and"
"and":
	ands	r0, r1
        bx      lr
	.size "and", .-"and"
