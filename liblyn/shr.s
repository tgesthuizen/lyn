	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "shr"
	.type "shr", %function
	.func "shr", "shr"
"shr":
	asrs    r0, r0, r1
        bx      lr
	.size "shr", .-"shr"
