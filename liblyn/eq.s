	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "="
	.type "=", %function
	.func "=", "="
"=":	
	subs    r0, r0, r1
        negs    r3, r0
        adcs    r0, r3
        bx      lr
	str     r0, [sp, #-8]
	.size "=", .-"="
