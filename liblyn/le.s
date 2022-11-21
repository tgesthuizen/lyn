	.thumb
	.syntax unified
	.section ".text", "ax"
	.global "<="
	.type "<=", %function
	.func "<=", "<="
"<=":
	movs    r3, r0
        asrs    r2, r1, #31
        lsrs    r0, r0, #31
        cmp     r1, r3
        adcs    r0, r0, r2
        lsls    r0, r0, #24
        lsrs    r0, r0, #24
        bx      lr
	.size "<=", .-"<="
