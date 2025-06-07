format binary
use32
include "TOOLS\PreprocFunc.inc"

FLAGS equ t s z o c p i u m
	virtual at 0
		t rb 1
		s rb 1
		z rb 1
		o rb 1
		c rb 1
		p rb 1
		i rb 1
		u rb 1
		m rb 1
	end virtual

virtual at 0
rept 32 cntr:0{
	forward
	_r#cntr rb 1
	r#cntr equ _r#cntr
	REGS equ _r#cntr
	common
	irpv regs, REGS\{
		\common
		REGS equ regs
	\}
}
end virtual

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
		; display 0xA
		dd _opcode
	\}
	macro !#name args&\{
		\local _opcode
		%alu_ins _opcode, opcode, 1024, args
		; %display_num _opcode, 16
		; display 0xA
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

macro %mem_ins result, opcode, is_load, arg1, arg2, arg3{
	if (arg3 >= 1 shl 10) | (arg3 <= -1 shl 10)
		\Offset is too big
	end if
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
	result = (arg3 and 11111b) or (is_load shl 5) or (not_flag shl 6) or (check_flag shl 7) or (arg3 shr 5 shl 10) or (arg1 shl 16) or (arg2 shl 21) or (opcode shl 26)
}

macro memb arg1, arg2, arg3{
	local result
	match =load any, arg1\{
		%mem_ins result, 18, 1, any, arg2, arg3
	\}
	match =store any, arg1\{
		%mem_ins result, 18, 0, any, arg2, arg3
	\}
	dd result
}

macro memh arg1, arg2, arg3{
	local result
	match =load any, arg1\{
		%mem_ins result, 19, 1, any, arg2, arg3
	\}
	match =store any, arg1\{
		%mem_ins result, 19, 0, any, arg2, arg3
	\}
	dd result
}

macro mem arg1, arg2, arg3{
	local result
	match =load any, arg1\{
		%mem_ins result, 20, 1, any, arg2, arg3
	\}
	match =store any, arg1\{
		%mem_ins result, 20, 0, any, arg2, arg3
	\}
	dd result
}