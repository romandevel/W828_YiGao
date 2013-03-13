; Copyright hyco 2010. All rights reserved.
;

; exported globals
;
    EXPORT  _enaio
    EXPORT  pulink
    EXPORT  user_entry
    EXPORT  _irq_int
    EXPORT	_fiq_int

    IMPORT  _iclock
    IMPORT  _idevs
    IMPORT  _iinit
    IMPORT  _itask
    IMPORT  _ikernel
    IMPORT  _kilev
    IMPORT  _noints
    IMPORT  _ioff
    IMPORT  _sched
    IMPORT  cpyvec
    IMPORT  ulink
    IMPORT  user_fiq_handler
    IMPORT  user_irq_handler
    IMPORT  comint
    IMPORT  _abort_code
	IMPORT  _iuser

    IMPORT _reset_handler      ; Added BELVAMO ( go to that address in response to a reset exception)

;
; SYSTEM CONFIGURATION PARAMETERS
;

; The following definitions enable (1) or disable (0)
;
ROMRAM      EQU        0        ; copy data from ROM to RAM at start-up time
CPYVEC      EQU        0        ; copy interrupt vectors
CEVIEW      EQU        0        ; enable CE-VIEW


_IRQ_MASK   EQU        0x80
_FIQ_MASK   EQU        0x40

_INT_MASK   EQU        (_IRQ_MASK | _FIQ_MASK)



    AREA    |C$$data|, DATA

    ALIGN

; The following variable should not be changed if 
; C EXECUTIVE and user tasks are linked into a single image.
; If user tasks are linked independently of C EXECUTIVE,
; the "ulink" parameter should be replaced with the offset
; of the start of the user text area.

pulink  DCD     ulink       ; link to user tasks

; SYSTEM START-UP VARIABLES
; These variables are added to simplify the initial test procedure.

_enaclk DCB     1           ; change to 1 to enable the clock
_enaio  DCB     1           ; change to 1 to activate I/O drivers
_enaint DCB     1           ; change to 1 to enable interrupts
        DCB     0           ; padding

; END OF CONFIGURATION PARAMETERS


    AREA    |C$$Code|,    CODE,    READONLY
    
	
user_entry

 IF CEVIEW=1
    EXPORT    _restrt
_restrt
 ENDIF

    mrs        a1,cpsr
    orr        a1,a1,#_INT_MASK        ; Disable interrupts!
    msr        cpsr_cxsf,a1


;
; CExecutive now has control of IRQs only
;
    mov        a1, #_INT_MASK       
    ldr        ip, =_ioff           ; initialise _ioff
    str        a1, [ip]

    bl        _ikernel
    cmp        a1, #0
    bne        abort                ; if out of heap, abort

    ldr        ip, =_enaint
    ldrb       a1, [ip]
    cmp        a1, #0
    beq        no_ints1
    bl        _iinit

; initialize context
;
    bl 		   _iuser               ; perform user initialisation function
    cmp        a1, #0               ;
    bne        abort                ; if unsuccesfull, abort

; initialize clock
;
no_ints1
    ldr        ip, =_enaclk
    ldrb       a1, [ip]
    cmp        a1, #0
    beq        no_clk
    bl        _iclock
    
no_clk
; initialize C EXECUTIVE
    bl        _idevs                ; allocate I/O buffers, init device tables
    cmp        a1, #0
    bne        abort                ; if out of heap, abort

    bl        _itask                ; allocate init tables, user stacks
    cmp        a1, #0
    bne        abort                ; if out of heap, abort

; BEGIN CE-VIEW SUPPORT CODE
 IF CEVIEW=1
    IMPORT    cvoff
    bl        cvoff
 ENDIF
; END CE-VIEW SUPPORT CODE

; enable processor interrupts
;

    ldr        ip, =_enaint
    ldrb       a1, [ip]
    cmp        a1, #0
    beq        no_ints

;	-----------------------------------
;	copy interrupt vector to address 0x0
    ldr        r0, =vect_start  ; source start  address
    mov        r1, #0			; destination start address
    ldr		   r2, =vect_end	; source end, also it is the limit

loopCopyVector  
	CMP     r0, r2          ; loop whilst r1 < r2
	LDRLO   r3, [r0], #4
	STRLO   r3, [r1], #4
	BLO     loopCopyVector
	
;  copy interrupt vector finished, now there is vectors resides in 0x0 
;  ------------------------------------------------------

    mrs        a1, cpsr

    ldr        ip, =_kilev
    str        a1, [ip]         ; initialize kernel interrupt level to noints
    ldr        ip, =_noints
    str        a1, [ip]         ; initialize interrupt level for noints
    
    bic        a1, a1, #0xC0    ; enable FIQ and IRQ interrupts!
    
    msr        cpsr_cxsf, a1  

	
no_ints
    bl        _sched            ; 开始任务调度，不返回  start scheduler, never returns

;
; We end up here if something bad happens (e.g. no heap available during init).
;
abort
    ldr     ip, =_abort_code
    strb    a1, [ip]
    b       abort


 IF CEVIEW=1
    EXPORT    _cvidevs

_cvidevs
    stmfd      sp!, {lr}
    ldr        ip, =_enaint
    ldrb       a1, [ip]
    cmp        a1, #0
    beq        cvnoints

cvnoints
    ldr        ip,    =_enaclk
    ldrb        a1, [ip]
    cmp        a1, #0
    beq        cvnoclk
    bl        _iclock
cvnoclk
    bl        _idevs                ; allocate I/O buffers, init device tables
    cmp        a1, #0
    bne        abort                ; if out of heap, abort
    ldmfd        sp!, {lr}
    mov        pc,lr

 ENDIF

; 启动代码到此结束


	IMPORT	DisplayInfo_DataAbort

; 下面的代码是和中断处理函数相关的代码，不属于启动代码，所有的interrupt相关代码应不超过
; 0x100 字节，以便可以放在sram的0x0开始的 0x100 = 256 字节中
;
; exception vectors  中断向量
;
; Code between 'vect_start' and 'vect_end' is copied to vector location 即0x0处.
; everything else (ie IRQ and FIQ handlers) are run from their link locations.
; 共0x40 字节 

vect_start

Reset_Handler           ldr     pc,_Reset_Handler       ; 0x00 == reset          (svc   mode)
Undefined_Handler       ldr     pc,_Undefined_Handler       ; 0x04 == undef          (undef mode)
SWI_Handler             ldr     pc,_SWI_Handler             ; 0x08 == SWI            (svc   mode)
PrefetchAbort_Handler   ldr     pc,_PrefetchAbort_Handler          ; 0x0c == prefetch abort (abort mode)
DataAbort_Handler       ldr     pc,_DataAbort_Handler       ; 0x10 == data abort     (abort mode)
Reserved_Handler        b       Reserved_Handler        ; 0x14 == reserved
IRQ_Handler             ldr     pc, _IRQ_Handler        ; 0x18 == IRQ            (irq   mode)
FIQ_Handler             ldr     pc, _FIQ_Handler        ; 0x1c == FIQ            (fiq   mode)

_Reset_Handler  dcd     _reset_handler					; 0x20 DCD
_IRQ_Handler    dcd     _irq_int						; 0x24 DCD  _irq_int的地址是编译器产生的
_FIQ_Handler    dcd     _fiq_int						; 0x28 DCD

_Undefined_Handler			dcd     _undefined_handler		; 0x2C DCD
_SWI_Handler				dcd		_swi_handler			; 0x30 DCD
_PrefetchAbort_Handler		dcd     _prefetchabort_handler	; 0x34 DCD
_DataAbort_Handler			dcd     _dataAbort_handler		; 0x38 DCD

vect_end



_swi_handler
	;step1, disable FIQ and IRQ interrupt via CPSR
;	SUB 	lr,lr,#8		;adjust lr	
	STMFD	r13!,{r0-r3,r12,lr} 	;r4-r11 preserved by ARMCC compiler

	;step2,call
	IMPORT	DisplayInfo_SWI
	bl	DisplayInfo_SWI
	;step3,before return, pop SPSR to CPSR, and clear interrupt label 
	LDMFD	r13!,{r0-r3,r12,pc}^	;^ means SPSR ---> CPSR while pc is loaded at same time

_undefined_handler
	;step1, disable FIQ and IRQ interrupt via CPSR
	SUB 	lr,lr,#4		;adjust lr	
	STMFD	r13!,{r0-r3,r12,lr} 	;r4-r11 preserved by ARMCC compiler

	;step2,call
	IMPORT	DisplayInfo_Undefined
	bl	DisplayInfo_Undefined
	;step3,before return, pop SPSR to CPSR, and clear interrupt label 
	LDMFD	r13!,{r0-r3,r12,pc}^	;^ means SPSR ---> CPSR while pc is loaded at same time

_prefetchabort_handler
	;step1, disable FIQ and IRQ interrupt via CPSR
	SUB 	lr,lr,#4		;adjust lr	
	STMFD	r13!,{r0-r3,r12,lr} 	;r4-r11 preserved by ARMCC compiler

	;step2,call
	IMPORT	DisplayInfo_PrefetchAbort
	bl	DisplayInfo_PrefetchAbort
	;step3,before return, pop SPSR to CPSR, and clear interrupt label 
	LDMFD	r13!,{r0-r3,r12,pc}^	;^ means SPSR ---> CPSR while pc is loaded at same time

_dataAbort_handler
	;step1, disable FIQ and IRQ interrupt via CPSR
	SUB 	lr,lr,#8		;adjust lr	
	STMFD	r13!,{r0-r3,r12,lr} 	;r4-r11 preserved by ARMCC compiler

	;step2,call
	IMPORT	DisplayInfo_DataAbort
	bl	DisplayInfo_DataAbort
	;step3,before return, pop SPSR to CPSR, and clear interrupt label 
	LDMFD	r13!,{r0-r3,r12,pc}^	;^ means SPSR ---> CPSR while pc is loaded at same time


; 单独定义这个code 区，是为了将这段代码放在sram中，加快处理速度
    AREA    |IntHandler$$Code|,    CODE,    READONLY

	EXPORT	 armInterruptResume
	EXPORT	 armInterruptPause

;
; irq handler  0x50 字节
;
; NOTE: When an irq interrupt is received, there is a window before
; interrupts are disabled that an FIQ interrupt can be received.
; irq模式的栈是从低到高，从irq的r13 开始，从低地址到高地址，存放了5个数据，依次是
;		r0, spsr, spsr+SVC+ARM mode, 用户interrupt handler，lr


_irq_int
    str		r0, [r13,#0]			; 将 r0 推入到irq的栈中，在下面这段代码中
    								; 仅使用了r0进行计算，所以只保护r0即可

    mrs     r0, spsr				; 在禁止中断的指令的执行过程中，ARM允许中断！
    tst     r0, #_IRQ_MASK          ; 如果是这条指令，则立即返回以便完成这个指令段
    ldrne   r0, [r13,#0]			; 从栈中推出 r0
    subnes  pc, lr, #4              ; 返回


    str     r0, [r13, #4]			; r0 存放spsr， 推入到栈中
    orr     r0, r0, #0x13           ; spsr, 设为Supervisor mode
    bic     r0, r0, #0x20           ; 设为ARM mode，中断处理程序要求一定是ARM模式
    str     r0, [r13, #8]			; r0 存放spsr+SVC+ARM mode, ， 推入到栈中
    ;orr     r0, r0, #0xc0           ; 禁止 fiq和 irq 中断
    orr     r0, r0, #_IRQ_MASK      ; 禁止 irq 中断
    msr     spsr_c, r0				; 将r0 回存到spsr中，c表示更改控制位
    
    ldr     r0, [pc, #irq_handler-.-8]
    								; 这条指令表示从使用相对pc偏移量(快？）， 找到irq_handler
    								; 的地址中的内容，赋给r0，-8 是由于arm9 流水线的缘故
    								; r0 存放的是函数user_irq_handler入口地址
    str     r0, [r13, #0xc]			; 推入到irq栈中: interrupt handler
    
    sub     lr, lr, #4              ; 在FIQ/IRQ中断情况，是这样的情况：
    								; ARM 在指令完成后，处理器需要检查FIQ/IRQ pin 是否为低(产生中断）
    								; 同时检查cpsr是否允许相应的中断，所以，IRQ/FIQ 在pc 更新后才产生，
    								; 这样pc值为为当前指令地址值 +8 （流水线）+ 4 （更新为下一个指针）
    								; arm内核规定，lr 总是存（pc-4）的值，
    								; 这样，如果需要lr指向下一条指令，则需要lr再减4
    
    str     lr, [r13, #0x10]		; 推入到irq栈中: lr
    mov     r0, r13					; 将堆栈指针stack pointer存到 r0 中
    
    ldr     lr, [pc, #_comint-.-8] 	; 使用相对偏移量，将lr赋为comint的入口地址
    subs    pc, lr, #0              ; 跳到comint, 转为 Supv mode， 退出IRQ中断
    								; 如果计算指令中有s标记，且r15(pc)作为存放结果的
    								; 寄存器，则一是程序会跳转到计算的值，二是当前模
    								; 式的SPSR值被复制到CPSR中，这点通常被用来从中断
    								; 模式中返回。当然，由于模式改变，r13的值也将改变


irq_handler     DCD user_irq_handler        ; Handler entry point in driver
_comint         DCD comint          ; Common Handler entry point



;
; fiq handler  0x38 字节
; This code saves critical information and calls common interrupt (comint)
; handling code. Info is saved on fiq_stack.
; fiq模式的栈是从低到高，从fiq的r13 开始，从低地址到高地址（注意程序使用了stmea指令，其中 e=Empty 栈指针指向
; 下一个空的位置， a= ascending 升序, 即从低到高），存放了5个数据，依次是
;		r0, spsr, spsr+SVC+ARM mode, 用户interrupt handler，lr

;_fiq_int
;	mrs		r8, spsr
;	tst		r8, #_FIQ_MASK			; ARM allows interrupt during an instruction
;	subnes	pc, lr, #4				; which disables interrupts!, return immediately
;									; to complete the section of code.
;
;	orr		r9, r8, #0x13			; spsr, with Supervisor mode
;	bic		r9, r9, #0x20			; set mode to ARM
;	ldr		r10, [pc, #fiq_handler-.-8]
;	sub		lr, lr, #4				; correct pc for RTI sequence
;	stmea	sp, {r0,r8-r10,lr}		; fiq_stack: r0, &handler, spsr, lr
;	mov		r0, sp					; save stack pointer in r0
;	orr		r9, r9, #0xc0			; spsr, with Supervisor mode, ints disabled
;	msr		spsr_c, r9
;	ldr		lr, [pc, #_comint-.-8]
;	subs	pc, lr, #0				; branch to comint, change to Supv mode
;
;
;fiq_handler     DCD user_fiq_handler    ; Handler entry point in driver

_fiq_int
	;step1, disable FIQ and IRQ interrupt via CPSR
	SUB 	lr,lr,#4				;adjust lr
	STMFD	r13!,{r0-r3,r12,lr} 	;r4-r11 preserved by ARMCC compiler

	;step2,call
	IMPORT	user_fiq_handler
	bl		user_fiq_handler		;20110504, 修改fiq的handler在FIQ中断中调用，
									;修改后，FIQ中断堆栈为减地址方式，该修改在init.s中line 174
	;step3,before return, pop SPSR to CPSR, and clear interrupt label 
	LDMFD	r13!,{r0-r3,r12,pc}^	;^ means SPSR ---> CPSR while pc is loaded at same time

;fiq_handler     DCD user_fiq_handler    ; Handler entry point in driver

;	下面两个函数为 0x34 字节
;-----------------------------------------------------------------------------
; 函数:	C语言原形 void armInterruptResume( UInt8 interrupt_level)
; 功能:	Enable the interrupts using the CPSR flags 
; 参数:	interrupt_level	-- arm 的interrupt 的标记,输入参数为上一次的中断使能标志
; 返回:	none
;----------------------------------------------------------------------------*/
armInterruptResume
	MRS      r1,CPSR
	AND      r0,r0,#3
	MOV      r0,r0,LSL #6
	BIC      r1,r1,r0
	MSR      CPSR_c,r1
	BX       r14

;-----------------------------------------------------------------------------
; 函数:	C 语言原形  UInt8 armInterruptPause( void )
; 功能:	Disable the interrupts using the CPSR flags 
; 参数:	none
; 返回:	interrupt_level	-- arm 的interrupt 的标记, 返回原来的中断使能标志
;----------------------------------------------------------------------------*/
armInterruptPause
	MRS      r1,CPSR
	MVN      r0,r1,LSR #6
	AND      r0,r0,#3
	ORR      r1,r1,#0xc0
	MSR      CPSR_c,r1
	BX       r14

  END