; C EXECUTIVE: CPU-SPECIFIC SYSTEM SUPPORT for ARM
; Copyright hyco 2010. All rights reserved.
; 
; 

	EXPORT setil
	EXPORT intoff
	EXPORT intrst
	EXPORT cpybuf
	EXPORT memcpy_hy
	EXPORT _at_dec
	EXPORT _at_inc
	EXPORT _at_sub
	EXPORT _at_or

;
; Changed FIQ and IRQ mask names to match those in inarm.s
; e.g. IRQMASK changes to _INT_MASK
; This makes more sense in cases where the mask includes the FIQ bit - not just the IRQ bit.
;

_IRQ_MASK   EQU 0x80
_FIQ_MASK   EQU 0x40

_INT_MASK   EQU		(_IRQ_MASK | _FIQ_MASK)


	AREA	|IWV1$$Code|,	CODE,	READONLY

; get current interrupt level, and set to new interrupt level
;
setil
	mrs		a2, cpsr
	bic		a3, a2, #_INT_MASK
	orr		a3, a3, a1
	msr		cpsr_cxsf, a3
	and		a1, a2, #_INT_MASK		; return old psr
	bx		lr


; disable interrupts, and return current level
;
intoff
	mrs		a2, cpsr
	orr		a3, a2, #_INT_MASK
	msr		cpsr_cxsf, a3
	and		a1, a2, #_INT_MASK		; return old psr
	bx		lr


; restore interrupts
;
intrst
	mrs		a2, cpsr
	bic		a3, a2, #_INT_MASK
	orr		a3, a3, a1
	msr		cpsr_cxsf, a3
	bx		lr

;
; A.McCurdy, dec2002: replace above memcpy with this version:
; More optimal memcpy (based on code in ITCL CExec release).
; 该函数存在问题，当copy的size为0x40倍数，且目标地址不为32bit对齐时，拷贝错误 gao
cpybuf
memcpy_hy
    stmfd   sp!,{r4-r7}      
    add     a4,a3,a1

    and     ip,a1,a2            ; src and dst lined up on a word boundary ??
    ands    ip,ip,#3
    bne     cp2
    
cp3 sub     ip,a4,a1
    cmp     ip,#64              ; at least 64 bytes remaining ??
    bmi     cp2
    ldmia   a2!,{r4-r7}
    stmia   a1!,{r4-r7}
    ldmia   a2!,{r4-r7}
    stmia   a1!,{r4-r7}
    ldmia   a2!,{r4-r7}
    stmia   a1!,{r4-r7}
    ldmia   a2!,{r4-r7}
    stmia   a1!,{r4-r7}
    b       cp3    
    
cp2 cmp     a1,a4
	ldrneb  ip,[a2],#1          ; from
	strneb  ip,[a1],#1          ; to
    bne     cp2

cp1	mov     a1,a3               ; return byte count
    ldmfd   sp!,{r4-r7}
	bx      lr




; atomically decrement a short value
;
_at_dec
	mrs		a3,  cpsr
	orr		a4,  a3,  #_INT_MASK
	msr		cpsr_cxsf,  a4
	ldr		a2, [a1, #0]
	sub		a2, a2, #1
	strb	a2, [a1, #0]
	mov		a2, a2, asr #8
	strb	a2, [a1, #1]
	msr		cpsr_cxsf,  a3
	bx		lr

; atomically increment a short value
;
_at_inc
	mrs		a3,  cpsr
	orr		a4,  a3,  #_INT_MASK
	msr		cpsr_cxsf,  a4
	ldr		a2, [a1, #0]
	add		a2, a2, #1
	strb	a2, [a1, #0]
	mov		a2, a2, asr #8
	strb	a2, [a1, #1]
	msr		cpsr_cxsf,  a3
	bx		lr

; atomically subtract a value from a short
;
_at_sub
	mov		a4, a2, lsl #16
	mov		a2, a4, asr #16
	mrs		a3,  cpsr
	orr		a4,  a3,  #_INT_MASK
	msr		cpsr_cxsf,  a4
	ldr		a4, [a1, #0]
	sub		a4, a4, a2
	strb	a4, [a1, #0]
	mov		a4, a4, asr #8
	strb	a4, [a1, #1]
	msr		cpsr_cxsf,  a3
	bx		lr

; atomically bitwise OR a value with an unsigned long
;
_at_or
	mrs		a3,  cpsr
	orr		a4,  a3,  #_INT_MASK
	msr		cpsr_cxsf,  a4
	ldr		a4, [a1, #0]
	orr		a4, a4, a2
	str		a4, [a1, #0]
	msr		cpsr_cxsf,  a3
	bx		lr

	END
