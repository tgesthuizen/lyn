	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "neg"
	.type "neg", %function
	.func "neg", "neg"
"neg":
	rsbs	r0, r0, #0
        bx      lr
	.size "neg", .-"neg"
