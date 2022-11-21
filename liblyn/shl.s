	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "shl"
	.type "shl", %function
	.func "shl", "shl"
"shl":
	lsls    r0, r0, r1
        bx      lr
	.size "shl", .-"shl"
