	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "or"
	.type "or", %function
	.func "or", "or"
"or":
	orrs	r0, r1
        bx      lr
	.size "or", .-"or"
