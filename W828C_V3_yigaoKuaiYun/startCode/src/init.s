; Copyright hyco 2010. All rights reserved.
;
; Here just initializes stack pointers and interrupts for each mode, and finally branches to the user_entry 
; function.
;
; On reset, the cpu starts up in Supervisor (SVC) mode, in ARM state, 
; with IRQ and FIQ disabled.


	AREA    Init, CODE, READONLY

 
; --- Standard definitions of mode bits and interrupt (I & F) flags in PSRs

Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F ; available on ARM Arch 4 and later

I_Bit           EQU     0x80 ; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40 ; when F bit is set, FIQ is disabled


; --- System memory locations
                                           ; remap first 2K of iSRAM to address 0     
		EXPORT topOfStacks
                                           ; remap first 2K of iSRAM to address 0
top_of_stacks	EQU     0x26000000         ; Points to first word after Cached end of SRAM

; --- Amount of memory (in bytes) allocated for stacks
Len_FIQ_Stack    EQU     1024
Len_IRQ_Stack    EQU     24
Len_ABT_Stack    EQU     24
Len_UND_Stack    EQU     24
Len_SVC_Stack    EQU     1024
Len_USR_Stack    EQU     0



		EXPORT Vector_Init_Block
; init code start
Vector_Init_Block
    ENTRY
    
       LDR PC, Reset_Addr
;       LDR PC, Undefined_Addr
;       LDR PC, SWI_Addr
;       LDR PC, Prefetch_Addr
;       LDR PC, Abort_Addr
;       LDR PC, Reserved_Addr
;       LDR PC, IRQ_Addr
;       LDR PC, FIQ_Addr

RESERVE00		DCD 0x03020100
RESERVE01		DCD 0x07060504
RESERVE02		DCD 0x0b0a0908
RESERVE03		DCD 0x0f0e0d0c
RESERVE04		DCD 0x03020100
RESERVE05		DCD 0x07060504
RESERVE06		DCD 0x0b0a0908
RESERVE07		DCD 0x0f0e0d0c
RESERVE08		DCD 0x03020100
RESERVE09		DCD 0x07060504
RESERVE10		DCD 0x0b0a0908
RESERVE11		DCD 0x0f0e0d0c
RESERVE12		DCD 0x03020100
RESERVE13		DCD 0x07060504
RESERVE14		DCD 0x0b0a0908
RESERVE15		DCD 0x0f0e0d0c
RESERVE16		DCD 0x03020100
RESERVE17		DCD 0x07060504
RESERVE18		DCD 0x0b0a0908
RESERVE19		DCD 0x0f0e0d0c
RESERVE20		DCD 0x03020100
RESERVE21		DCD 0x07060504
RESERVE22		DCD 0x0b0a0908
;RESERVE23		DCD 0x0f0e0d0c
;RESERVE24		DCD 0x03020100
;RESERVE25		DCD 0x07060504
;RESERVE26		DCD 0x0b0a0908
;RESERVE27		DCD 0x0f0e0d0c
;RESERVE28		DCD 0x03020100
;RESERVE29		DCD 0x07060504
;RESERVE30		DCD 0x0b0a0908


Reset_Addr      DCD Reset_Handler
Undefined_Addr  DCD Undefined_Handler
SWI_Addr        DCD SWI_Handler
Prefetch_Addr   DCD Prefetch_Handler
Abort_Addr      DCD Abort_Handler
Reserved_Addr   DCD Reserved_Handler
IRQ_Addr        DCD IRQ_Handler
FIQ_Addr        DCD FIQ_Handler

topOfStacks		DCD     top_of_stacks


                GBLS VERSION
VERSION         SETS "Boot Code by Hyco v1.00 2010/01/01"  


;  Exception Handlers
;
; If an exception occurs during the ROM boot we end up somewhere
; in the next set of forever loops.

Undefined_Handler
  B Undefined_Handler

SWI_Handler
  B SWI_Handler

Prefetch_Handler
  B Prefetch_Handler

Abort_Handler
  B Abort_Handler

IRQ_Handler
  B IRQ_Handler

Reserved_Handler
  B Reserved_Handler

FIQ_Handler
  B FIQ_Handler
  

;;;;TELECHIPS 2010.7.21
MasterID
	; Lower address has higher priority
	; 0 = LCD, 1 = (EHI>CIF>DMA), 2/3 = VideoHW, 4 = G2D, 5 = SCore I, 6 = SCore D, 7 = MSC, 8 = MCore D, 9 = MCore I
	DCB 0x00, 0x01, 0x06, 0x05, 0x03, 0x02, 0x09, 0x08, 0x07, 0x04 ,0x00, 0x00
	  

	EXPORT _reset_handler
_reset_handler
Reset_Handler

;;TELECHIPS 2010.7.21-----start
		nop
00
		; Arbitration Priority Setting (to be compatible with 78X)
		ldr	r0, =0xF3005300
		ldr	r1, =MasterID
		add	r2, r1, #10
01
		ldrb	r3, [r1], #1
		str	r3, [r0], #4
		cmp	r1, r2
		bne	%b01
		
		ldr	r0, =0xF3005100
		mov	r1, #10
		str	r1, [r0, #0x08]
		mov	r1, #0
		str	r1, [r0]
;;;;TELECHIPS 2010.7.21-------end

; --- Initialize stack pointer registers

; Enter each mode in turn and set up the stack pointer

		LDR		r0, =top_of_stacks

		MSR		CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit ; No interrupts
		SUB		r0, r0, #4
		MOV		sp, r0								; 堆栈为减地址方式
		SUB		r0, r0, #Len_FIQ_Stack

		MSR		CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit ; No interrupts
		SUB		r0, r0, #Len_IRQ_Stack				; 堆栈为增地址方式
		MOV		sp, r0

		SUB		r0, r0, #4							; 以下堆栈为减地址方式
		MSR		CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit ; No interrupts
		MOV		sp, r0								; 堆栈为减地址方式
		SUB		r0, r0, #Len_ABT_Stack

		MSR		CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit ; No interrupts
		MOV		sp, r0								; 堆栈为减地址方式
		SUB		r0, r0, #Len_UND_Stack

		MSR		CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit ; No interrupts
		MOV		sp, r0								; 堆栈为减地址方式
		;SUB		r0, r0, #Len_SVC_Stack
        
;******************************************************************************
; MMU initialize and enable
; Build Execution Regions for C code
;*****************************************************************************
		
		IMPORT hyhwArm926e_writeBackDcache
		IMPORT hyhwArm926ejs_drainWriteBuffer
		IMPORT hyhwArm926ejs_invalidIcacheDcache
		
		IMPORT MMUSectionTable
		IMPORT hyhwArm926ejs_mmuDisable
		IMPORT hyhwArm926ejs_mmuConfig
		IMPORT hyhwArm926ejs_mmuEnable
		IMPORT hyhwArm926ejs_enableDcache
		IMPORT hyhwArm926ejs_enableIcache
		
		BL	hyhwArm926e_writeBackDcache
		BL	hyhwArm926ejs_drainWriteBuffer
		BL	hyhwArm926ejs_invalidIcacheDcache
		
		BL	hyhwArm926ejs_mmuDisable
		LDR	r0,	=MMUSectionTable
		BL	hyhwArm926ejs_mmuConfig
		BL	hyhwArm926ejs_mmuEnable
	
		BL	hyhwArm926ejs_enableDcache
		BL	hyhwArm926ejs_enableIcache
		

; - For Embedded systems we have to do the copying of ROM to RAM
 		IMPORT InitRegions
        BL     InitRegions ; in regioninit.s. The function is configured to use the scatter file
						   ; defined execution regions
						   
	
		IMPORT hyhwScanSdramCapability
		BL	hyhwScanSdramCapability

; unmask interruptions in SVC mode 
        MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit ; No interrupts
        
        IMPORT  hyhwInitialSysClk
        bl      hyhwInitialSysClk 
        
; --- Now enter the C code
		IMPORT	user_entry
		b user_entry


    EXPORT Interrupts_Disable
    EXPORT Interrupts_Enable

Interrupts_Enable
	MSR     CPSR_c, #Mode_SVC
    MOV     pc, lr

Interrupts_Disable
	MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit ; No interrupts
	MOV     pc, lr

; This section contains the heap placeholder to ensure resolving
; the Image$$ISRAM$$ZI$$Limit linker symbol

    EXPORT scatter_heap

    AREA scatter_heap, DATA, READWRITE

bottom_of_heap SPACE 1

    END

