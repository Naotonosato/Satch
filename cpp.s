	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 15	sdk_version 10, 15
	.globl	__Z8functionee          ## -- Begin function _Z8functionee
	.p2align	4, 0x90
__Z8functionee:                         ## @_Z8functionee
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	fldt	32(%rbp)
	fldt	16(%rbp)
	faddp	%st(1)
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function

.subsections_via_symbols
