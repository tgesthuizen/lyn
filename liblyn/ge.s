	.thumb
	.syntax unified
	.section ".text", "ax"
	.global ">="
	.type ">=", %function
	.func ">=", ">="
">=":
        movs    r3, r0
        lsrs    r2, r1, #31
        asrs    r0, r0, #31
        cmp     r3, r1
        adcs    r0, r0, r2
        lsls    r0, r0, #24
        lsrs    r0, r0, #24
        bx      lr
	.size ">=", .-">="
