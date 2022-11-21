	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "land"
	.type "land", %function
	.func "land", "land"
"land":
	ands    r0, r1
        bx      lr
	.size "land", .-"land"
