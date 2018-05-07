	.file	"bootloader.c"
	.text
.Ltext0:
#APP
	.code16gcc

	jmp $0, $main;
#NO_APP
	.globl	printCharacter
	.type	printCharacter, @function
printCharacter:
.LFB0:
	.file 1 "bootloader.c"
	.loc 1 17 0
	.cfi_startproc
.LVL0:
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	.loc 1 20 0
	movsbl	8(%ebp), %eax
	orb	$14, %ah
	.loc 1 18 0
#APP
# 18 "bootloader.c" 1
	int $0x10;
# 0 "" 2
	.loc 1 22 0
#NO_APP
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	printCharacter, .-printCharacter
	.globl	printLine
	.type	printLine, @function
printLine:
.LFB1:
	.loc 1 25 0
	.cfi_startproc
.LVL1:
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	8(%ebp), %edx
.LVL2:
.L4:
	.loc 1 26 0
	movsbl	(%edx), %eax
	testb	%al, %al
	je	.L8
	.loc 1 27 0
	pushl	%eax
	incl	%edx
.LVL3:
	call	printCharacter
.LVL4:
	popl	%eax
	jmp	.L4
.L8:
.LVL5:
.LBB9:
.LBB10:
	.loc 1 18 0
	movl	$3594, %eax
#APP
# 18 "bootloader.c" 1
	int $0x10;
# 0 "" 2
.LVL6:
#NO_APP
.LBE10:
.LBE9:
.LBB11:
.LBB12:
	movl	$3597, %eax
#APP
# 18 "bootloader.c" 1
	int $0x10;
# 0 "" 2
.LVL7:
#NO_APP
.LBE12:
.LBE11:
	.loc 1 31 0
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	printLine, .-printLine
	.globl	getCharacter
	.type	getCharacter, @function
getCharacter:
.LFB2:
	.loc 1 34 0
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	.loc 1 36 0
	xorl	%eax, %eax
	.loc 1 34 0
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	.loc 1 36 0
#APP
# 36 "bootloader.c" 1
	int $0x16;
# 0 "" 2
.LVL8:
	.loc 1 42 0
#NO_APP
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	getCharacter, .-getCharacter
	.globl	getLine
	.type	getLine, @function
getLine:
.LFB3:
	.loc 1 45 0
	.cfi_startproc
.LVL9:
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
.LVL10:
	.loc 1 47 0
	movl	8(%ebp), %edx
.LVL11:
.L12:
	movl	12(%ebp), %eax
	leal	-1(%eax), %ecx
	movl	%edx, %eax
	subl	8(%ebp), %eax
	cmpl	%eax, %ecx
	jle	.L13
.LBB13:
.LBB14:
.LBB15:
	.loc 1 36 0
	xorl	%eax, %eax
#APP
# 36 "bootloader.c" 1
	int $0x16;
# 0 "" 2
#NO_APP
.LBE15:
.LBE14:
	.loc 1 50 0
	cmpb	$10, %cl
	je	.L13
	.loc 1 54 0
	movsbl	%cl, %eax
	pushl	%eax
	call	printCharacter
.LVL12:
	.loc 1 55 0
	movb	%cl, (%edx)
	incl	%edx
.LVL13:
	popl	%eax
	jmp	.L12
.L13:
.LBE13:
	.loc 1 58 0
	movb	$0, (%edx)
	.loc 1 59 0
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	getLine, .-getLine
	.globl	rebootSystem
	.type	rebootSystem, @function
rebootSystem:
.LFB4:
	.loc 1 62 0
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	.loc 1 63 0
#APP
# 63 "bootloader.c" 1
	int $0x19;
# 0 "" 2
	.loc 1 64 0
#NO_APP
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	rebootSystem, .-rebootSystem
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Hallo World!"
.LC1:
	.string	""
	.section	.text.startup,"ax",@progbits
	.globl	main
	.type	main, @function
main:
.LFB5:
	.loc 1 67 0
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%esi
	.cfi_offset 6, -12
	call	__x86.get_pc_thunk.si
	addl	$_GLOBAL_OFFSET_TABLE_, %esi
	pushl	%ebx
	subl	$16, %esp
	.cfi_offset 3, -16
	.loc 1 72 0
	leal	-11(%ebp), %ebx
	.loc 1 71 0
	leal	.LC0@GOTOFF(%esi), %eax
	pushl	%eax
	call	printLine
.LVL14:
	.loc 1 72 0
	pushl	$3
	pushl	%ebx
	call	getLine
.LVL15:
	.loc 1 73 0
	leal	.LC1@GOTOFF(%esi), %eax
	pushl	%eax
	call	printLine
.LVL16:
	.loc 1 74 0
	pushl	%ebx
	call	printLine
.LVL17:
	addl	$20, %esp
.L22:
	jmp	.L22
	.cfi_endproc
.LFE5:
	.size	main, .-main
	.section	.text.__x86.get_pc_thunk.si,"axG",@progbits,__x86.get_pc_thunk.si,comdat
	.globl	__x86.get_pc_thunk.si
	.hidden	__x86.get_pc_thunk.si
	.type	__x86.get_pc_thunk.si, @function
__x86.get_pc_thunk.si:
.LFB6:
	.cfi_startproc
	movl	(%esp), %esi
	ret
	.cfi_endproc
.LFE6:
	.text
.Letext0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x1f3
	.value	0x4
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF10
	.byte	0xc
	.long	.LASF11
	.long	.LASF12
	.long	.Ldebug_ranges0+0
	.long	0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x3
	.long	.LASF2
	.byte	0x1
	.byte	0x42
	.long	.LFB5
	.long	.LFE5-.LFB5
	.uleb128 0x1
	.byte	0x9c
	.long	0x74
	.uleb128 0x4
	.long	.LASF6
	.byte	0x1
	.byte	0x45
	.long	0x74
	.uleb128 0x2
	.byte	0x91
	.sleb128 -19
	.uleb128 0x5
	.long	.LVL14
	.long	0x143
	.uleb128 0x5
	.long	.LVL15
	.long	0xa3
	.uleb128 0x5
	.long	.LVL16
	.long	0x143
	.uleb128 0x5
	.long	.LVL17
	.long	0x143
	.byte	0
	.uleb128 0x6
	.long	0x8b
	.long	0x84
	.uleb128 0x7
	.long	0x84
	.byte	0x2
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.byte	0x7
	.long	.LASF0
	.uleb128 0x8
	.byte	0x1
	.byte	0x6
	.long	.LASF1
	.uleb128 0x9
	.long	.LASF13
	.byte	0x1
	.byte	0x3d
	.long	.LFB4
	.long	.LFE4-.LFB4
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x3
	.long	.LASF3
	.byte	0x1
	.byte	0x2c
	.long	.LFB3
	.long	.LFE3-.LFB3
	.uleb128 0x1
	.byte	0x9c
	.long	0x123
	.uleb128 0xa
	.long	.LASF4
	.byte	0x1
	.byte	0x2c
	.long	0x123
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0xa
	.long	.LASF5
	.byte	0x1
	.byte	0x2c
	.long	0x25
	.uleb128 0x2
	.byte	0x91
	.sleb128 4
	.uleb128 0xb
	.long	.LASF7
	.byte	0x1
	.byte	0x2e
	.long	0x25
	.long	.LLST3
	.uleb128 0xc
	.long	.LBB13
	.long	.LBE13-.LBB13
	.uleb128 0xd
	.string	"c"
	.byte	0x1
	.byte	0x31
	.long	0x8b
	.uleb128 0xe
	.long	0x129
	.long	.LBB14
	.long	.LBE14-.LBB14
	.byte	0x1
	.byte	0x31
	.long	0x118
	.uleb128 0xc
	.long	.LBB15
	.long	.LBE15-.LBB15
	.uleb128 0xf
	.long	0x1ee
	.byte	0
	.byte	0
	.uleb128 0x5
	.long	.LVL12
	.long	0x1ab
	.byte	0
	.byte	0
	.uleb128 0x10
	.byte	0x4
	.long	0x8b
	.uleb128 0x11
	.long	.LASF14
	.byte	0x1
	.byte	0x21
	.long	0x8b
	.byte	0x1
	.long	0x143
	.uleb128 0xd
	.string	"c"
	.byte	0x1
	.byte	0x23
	.long	0x8b
	.byte	0
	.uleb128 0x3
	.long	.LASF8
	.byte	0x1
	.byte	0x18
	.long	.LFB1
	.long	.LFE1-.LFB1
	.uleb128 0x1
	.byte	0x9c
	.long	0x1ab
	.uleb128 0x12
	.long	.LASF9
	.byte	0x1
	.byte	0x18
	.long	0x123
	.long	.LLST0
	.uleb128 0xe
	.long	0x1ab
	.long	.LBB9
	.long	.LBE9-.LBB9
	.byte	0x1
	.byte	0x1d
	.long	0x184
	.uleb128 0x13
	.long	0x1b7
	.long	.LLST1
	.byte	0
	.uleb128 0xe
	.long	0x1ab
	.long	.LBB11
	.long	.LBE11-.LBB11
	.byte	0x1
	.byte	0x1e
	.long	0x1a1
	.uleb128 0x13
	.long	0x1b7
	.long	.LLST2
	.byte	0
	.uleb128 0x5
	.long	.LVL4
	.long	0x1ab
	.byte	0
	.uleb128 0x14
	.long	.LASF15
	.byte	0x1
	.byte	0x10
	.byte	0x1
	.long	0x1c3
	.uleb128 0x15
	.long	.LASF9
	.byte	0x1
	.byte	0x10
	.long	0x8b
	.byte	0
	.uleb128 0x16
	.long	0x1ab
	.long	.LFB0
	.long	.LFE0-.LFB0
	.uleb128 0x1
	.byte	0x9c
	.long	0x1df
	.uleb128 0x17
	.long	0x1b7
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0x18
	.long	0x129
	.long	.LFB2
	.long	.LFE2-.LFB2
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x19
	.long	0x139
	.uleb128 0x1
	.byte	0x50
	.byte	0
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST3:
	.long	.LVL10
	.long	.LVL11
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.long	.LVL11
	.long	.LVL12
	.value	0x7
	.byte	0x72
	.sleb128 0
	.byte	0x91
	.sleb128 0
	.byte	0x6
	.byte	0x1c
	.byte	0x9f
	.long	.LVL12
	.long	.LVL13
	.value	0x9
	.byte	0x72
	.sleb128 0
	.byte	0x91
	.sleb128 0
	.byte	0x6
	.byte	0x1c
	.byte	0x23
	.uleb128 0x1
	.byte	0x9f
	.long	.LVL13
	.long	.LFE3
	.value	0x7
	.byte	0x72
	.sleb128 0
	.byte	0x91
	.sleb128 0
	.byte	0x6
	.byte	0x1c
	.byte	0x9f
	.long	0
	.long	0
.LLST0:
	.long	.LVL1
	.long	.LVL2
	.value	0x2
	.byte	0x91
	.sleb128 0
	.long	.LVL2
	.long	.LFE1
	.value	0x1
	.byte	0x52
	.long	0
	.long	0
.LLST1:
	.long	.LVL5
	.long	.LVL6
	.value	0x2
	.byte	0x3a
	.byte	0x9f
	.long	0
	.long	0
.LLST2:
	.long	.LVL6
	.long	.LVL7
	.value	0x2
	.byte	0x3d
	.byte	0x9f
	.long	0
	.long	0
	.section	.debug_aranges,"",@progbits
	.long	0x24
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0
	.value	0
	.value	0
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	.LFB5
	.long	.LFE5-.LFB5
	.long	0
	.long	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.long	.Ltext0
	.long	.Letext0
	.long	.LFB5
	.long	.LFE5
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF0:
	.string	"unsigned int"
.LASF11:
	.string	"bootloader.c"
.LASF10:
	.string	"GNU C11 7.2.0 -m32 -march=i686 -g -Os -ffreestanding"
.LASF14:
	.string	"getCharacter"
.LASF8:
	.string	"printLine"
.LASF7:
	.string	"length"
.LASF9:
	.string	"toPrint"
.LASF13:
	.string	"rebootSystem"
.LASF12:
	.string	"/media/sf_repo/A1"
.LASF4:
	.string	"buffer"
.LASF3:
	.string	"getLine"
.LASF6:
	.string	"input"
.LASF2:
	.string	"main"
.LASF5:
	.string	"bufferLength"
.LASF1:
	.string	"char"
.LASF15:
	.string	"printCharacter"
	.ident	"GCC: (Ubuntu 7.2.0-8ubuntu3.2) 7.2.0"
	.section	.note.GNU-stack,"",@progbits
