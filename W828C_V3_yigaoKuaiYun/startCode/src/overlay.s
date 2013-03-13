;-------------------------------------------------------------------------------
; 文件: overlay.s                                         
; 功能: overlay Manager                                   
; 说明:	本文件包括了相关的宏和函数，用来支持在SRAM中的overlay的运行区域，在程序
;		运行时需要初始化对于某个运行区域。 
;		RO 区域和RW区域的性质一样，都是从一个load区copy到image区，将RO code 和/或
;		RW data 从ROM中复制到RAM中， 
;		同时，对ZI data区清零. 注意，对于某个运行区域，RO，RW，ZI区域可能仅有其中
;		一种或两种
; -------------------------------------------------------------------------------


        AREA overlayMan, CODE, READONLY

; 输出的函数
		
; -------------------------------------------------------------------------------
; 宏  : macro_RWZI_Init
; 功能: 1)	将 RO code 和/或 RW data 从 ROM (起始地址Load$$area$$Base) 复制到
; 			RAM (起始地址Image$$area$$Base)，长度为Image$$area$$Length bytes.
; 		2)	将 RAM 中的 ZI data 清零，起始地址为Image$$area$$ZI$$Base, 
;			长度为  Image$$area$$ZI$$Length bytes.
; 参数: $areaname	-- 运行区的名字，由scatter文件中定义  
; -------------------------------------------------------------------------------
        MACRO
        macro_RWZI_Init $areaname

;	LCLS 用来声明局部的字符串变量
        LCLS   namecp
        LCLS   copyloadsym
        LCLS   copybasesym
        LCLS   copylensym
        LCLS   zibasesym
        LCLS   zilensym

;	给这些局部变量赋值
namecp		SETS	"$areaname"

copyloadsym SETS	"|Load$$$$":CC:namecp:CC:"$$$$Base|"
copybasesym SETS	"|Image$$$$":CC:namecp:CC:"$$$$Base|"
copylensym  SETS	"|Image$$$$":CC:namecp:CC:"$$$$Length|"
zibasesym   SETS	"|Image$$$$":CC:namecp:CC:"$$$$ZI$$$$Base|"
zilensym    SETS	"|Image$$$$":CC:namecp:CC:"$$$$ZI$$$$Length|"


;	下面的符号是由linker产生的。是根据scatter文件定义的区域生成的，不一定
;	每个符号都被定义了，例如某个运行区只有RO，则RW，ZI的符号就不会生成。
;	使用关键字WEAK， 表示如果没有定义值，则取值为0。 

        IMPORT $copyloadsym, WEAK
        IMPORT $copybasesym, WEAK
        IMPORT $copylensym, WEAK
        IMPORT $zibasesym, WEAK
        IMPORT $zilensym, WEAK


        STMFD   r13!,{r0-r4}
        
	;	IMPORT CacheDataWriteBack
	;	IMPORT Drain_WriteBuffer
	;	IMPORT CacheInstructDataInvalidate
	;	IMPORT CacheInstructInvalidate
	
		IMPORT hyhwArm926e_writeBackDcache
		IMPORT hyhwArm926ejs_drainWriteBuffer
		IMPORT hyhwArm926ejs_invalidIcacheDcache
		IMPORT hyhwArm926ejs_invalidateIcache
		
    ;    BL				CacheDataWriteBack
    ;    BL				Drain_WriteBuffer
    ;    BL				CacheInstructDataInvalidate
        
        BL	hyhwArm926e_writeBackDcache
        BL	hyhwArm926ejs_drainWriteBuffer
        BL	hyhwArm926ejs_invalidIcacheDcache

;	复制RO 和/或 RW区域
        LDR     r0, =$copyloadsym       ; load address of region
        LDR     r1, =$copybasesym       ; execution address of region
        MOV     r2, r1                  ; copy execution address into r2
        LDR     r4, =$copylensym
        ADD     r2, r2, r4              ; add region length to execution address to...
                                        ; ...calculate address of word beyond end...
                                        ; ... of execution region
        CMP     r0,r1
        BLNE    copy
        
       
;	清零ZI区域       
        LDR     r2, =$zilensym          ; get length of ZI region
        LDR     r0, =$zibasesym         ; load base address of ZI region
        MOV     r1, r0                  ; copy base address of ZI region into r1
        ADD     r1, r1, r2              ; add region length to base address to...
                                        ; ...calculate address of word beyond end...
                                        ; ... of ZI region
        BL      zi_init

    ;    BL				CacheDataWriteBack
    ;    BL				Drain_WriteBuffer
    ;    BL				CacheInstructDataInvalidate
        
        BL	hyhwArm926e_writeBackDcache
        BL	hyhwArm926ejs_drainWriteBuffer
        BL	hyhwArm926ejs_invalidIcacheDcache
   
   ;     BL				CacheInstructInvalidate

        LDMFD   r13!,{r0-r4}

        MEND
;	宏的结束        
        


; ------------------------------------------------------------------------------
; 函数: copy
; 功能: 复制一块区域的内容到另一区域
; 参数: r0	--	源的起始地址
;		r1	--	目标的起始地址
;		r2	--	目标的终止地址的下一地址
;		r3	--	长度，以word为单位
; 返回: 无
; ------------------------------------------------------------------------------ 
copy
        CMP     r1, r2          ; loop whilst r1 < r2
        LDRLO   r3, [r0], #4
        STRLO   r3, [r1], #4
        BLO     copy
        MOV     pc, lr          ; return from subroutine copy

; ------------------------------------------------------------------------------
; 函数: zi_init
; 功能: 清零一块区域
; 参数: r0	--	起始地址
;		r1	--	终止地址的下一地址
; 返回: 无
; ------------------------------------------------------------------------------ 
zi_init
        MOV     r2, #0
        CMP     r0, r1          ; loop whilst r0 < r1
        STRLO   r2, [r0], #4
        BLO     zi_init 
        MOV     pc, lr          ; return from subroutine zi_init



; 以下为输出的函数

; ------------------------------------------------------------------------------
; 函数: Record_InitExeRam
; 功能: 初始化record运行区域，区域名字必须和scatter文件中定义的一致
; 参数: 无
; 返回: 无
; ------------------------------------------------------------------------------ 
       
        
        END
