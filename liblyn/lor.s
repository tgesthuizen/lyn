	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "lor"
	.type "lor", %function
	.func "lor", "lor"
"lor":
	orrs    r0, r1
        bx      lr
	.size "lor", .-"lor"
