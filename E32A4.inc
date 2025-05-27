format binary
use32
include "TOOLS\PreprocFunc.inc"

FLAGS equ t s z o c p i u m
	t = 0
	s = 1
	z = 2
	o = 3
	c = 4
	p = 5
	i = 6
	u = 7
	m = 8

rept 32 cntr:0{
	forward
	_r#cntr = cntr
	r#cntr equ _r#cntr
	REGS equ _r#cntr
	common
	irpv regs, REGS\{
		\common
		REGS equ regs
	\}
}

macro %is_reg result, arg{
	result equ 0
	match val, arg\{
		match any1 =val any2, [: REGS :] \\{
			result equ 1
		\\}
	\}
}

; %is_reg result, r3
; display result

; macro %is_flag result, arg{
; 	result equ 0
; 	match val, arg\{
; 		match any1 =arg any2, [: FLAGS :]\\{
; 			result equ 1
; 		\\}
; 	\}
; }

; %is_flag result, Z
; display result

; NF

macro %alu_ins result, opcode, nfmask, arg1, arg2, arg3{
	local not_flag, check_flag, _arg1
	check_flag = t
	not_flag = 0
	_arg1 equ arg1
	match ~flag dest, arg1\{
		not_flag = 1
		check_flag = flag
		_arg1 equ dest
	rept 0\{\} match flag dest, arg1\{
		check_flag = flag
		_arg1 equ dest
	\}
	local _result
	%is_reg _result, arg3
	match =1, _result\{
		result = ((not_flag shl 4) or (check_flag shl 5) or (arg3 shl 11) or (arg2 shl 16) or (_arg1 shl 22) or (opcode shl 26)) or nfmask
	rept 0\{\} rept 1\{
		if nfmask
			\Can't use no flags prefix with imm operand
		end if 
		if check_flag <> t | not_flag
			\Invalid flags postfix use for imm operand mode
		end if 
		result = arg3 or (arg2 shl 16) or (_arg1 shl 22) or (opcode shl 26) or 0x80000000
	\}
}

macro %generate_alu [name, opcode]{
	macro name args&\{
		\local _opcode
		%alu_ins _opcode, opcode, 0, args
		; %display_num _opcode, 16
		dd _opcode
	\}
	macro !#name args&\{
		\local _opcode
		%alu_ins _opcode, opcode, 1024, args
		; %display_num _opcode, 16
		dd _opcode
	\}
}

%generate_alu \
	add, 0, \
	adc, 1, \
	sub, 2, \
	sbb, 3, \
	umul, 4, \
	mul, 5, \
	nor, 6, \
	and, 7, \
	or, 8, \
	xor, 9, \
	nand, 10, \
	shl, 11, \
	shr, 12, \
	rol, 13, \
	ror, 14, \
	sar, 15, \
	sld, 16, \
	srd, 17

