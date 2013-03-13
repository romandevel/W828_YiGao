;-------------------------------------------------------------------------------
; �ļ�: overlay.s                                         
; ����: overlay Manager                                   
; ˵��:	���ļ���������صĺ�ͺ���������֧����SRAM�е�overlay�����������ڳ���
;		����ʱ��Ҫ��ʼ������ĳ���������� 
;		RO �����RW���������һ�������Ǵ�һ��load��copy��image������RO code ��/��
;		RW data ��ROM�и��Ƶ�RAM�У� 
;		ͬʱ����ZI data������. ע�⣬����ĳ����������RO��RW��ZI������ܽ�������
;		һ�ֻ�����
; -------------------------------------------------------------------------------


        AREA AppOverlayMan, CODE, READONLY

; ����ĺ���
				
		
		EXPORT IE_InitToSdram

		EXPORT Gsm_InitToSdram
		
		EXPORT Wifi_InitToSdram
		
		EXPORT Hyc_SysSleepOverlay

		EXPORT TcpIp_InitMemory

		EXPORT PPP_InitMemory
                
		EXPORT USB_InitMemory
        
        EXPORT Nand_InitToSdram
        
		EXPORT HandWriting_InitMemory
		
; -------------------------------------------------------------------------------
; ��  : AppMacro_RWZI_Init
; ����: 1)	�� RO code ��/�� RW data �� ROM (��ʼ��ַLoad$$area$$Base) ���Ƶ�
; 			RAM (��ʼ��ַImage$$area$$Base)������ΪImage$$area$$Length bytes.
; 		2)	�� RAM �е� ZI data ���㣬��ʼ��ַΪImage$$area$$ZI$$Base, 
;			����Ϊ  Image$$area$$ZI$$Length bytes.
; ����: $areaname	-- �����������֣���scatter�ļ��ж���  
; -------------------------------------------------------------------------------
        MACRO
        AppMacro_RWZI_Init $areaname

;	LCLS ���������ֲ����ַ�������
        LCLS   namecp
        LCLS   copyloadsym
        LCLS   copybasesym
        LCLS   copylensym
        LCLS   zibasesym
        LCLS   zilensym

;	����Щ�ֲ�������ֵ
namecp		SETS	"$areaname"

copyloadsym SETS	"|Load$$$$":CC:namecp:CC:"$$$$Base|"
copybasesym SETS	"|Image$$$$":CC:namecp:CC:"$$$$Base|"
copylensym  SETS	"|Image$$$$":CC:namecp:CC:"$$$$Length|"
zibasesym   SETS	"|Image$$$$":CC:namecp:CC:"$$$$ZI$$$$Base|"
zilensym    SETS	"|Image$$$$":CC:namecp:CC:"$$$$ZI$$$$Length|"


;	����ķ�������linker�����ġ��Ǹ���scatter�ļ�������������ɵģ���һ��
;	ÿ�����Ŷ��������ˣ�����ĳ��������ֻ��RO����RW��ZI�ķ��žͲ������ɡ�
;	ʹ�ùؼ���WEAK�� ��ʾ���û�ж���ֵ����ȡֵΪ0�� 

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

		;	����RO ��/�� RW����
		LDR     r0, =$copyloadsym       ; load address of region
		LDR     r1, =$copybasesym       ; execution address of region
		MOV     r2, r1                  ; copy execution address into r2
		LDR     r4, =$copylensym
		ADD     r2, r2, r4              ; add region length to execution address to...
		                                ; ...calculate address of word beyond end...
		                                ; ... of execution region
		CMP     r0,r1
		BLNE    AppCopy


		;	����ZI����       
		LDR     r2, =$zilensym          ; get length of ZI region
		LDR     r0, =$zibasesym         ; load base address of ZI region
		MOV     r1, r0                  ; copy base address of ZI region into r1
		ADD     r1, r1, r2              ; add region length to base address to...
		                                ; ...calculate address of word beyond end...
		                                ; ... of ZI region
		BL      AppZiInit

        BL	hyhwArm926e_writeBackDcache
        BL	hyhwArm926ejs_drainWriteBuffer
        BL	hyhwArm926ejs_invalidIcacheDcache

		LDMFD   r13!,{r0-r4}

		MEND
;	��Ľ���        
        


; ------------------------------------------------------------------------------
; ����: AppCopy
; ����: ����һ����������ݵ���һ����
; ����: r0	--	Դ����ʼ��ַ
;		r1	--	Ŀ�����ʼ��ַ
;		r2	--	Ŀ�����ֹ��ַ����һ��ַ
;		r3	--	���ȣ���wordΪ��λ
; ����: ��
; ------------------------------------------------------------------------------ 
AppCopy
		CMP     r1, r2          ; loop whilst r1 < r2
		LDRLO   r3, [r0], #4
		STRLO   r3, [r1], #4
		BLO     AppCopy
		MOV     pc, lr          ; return from subroutine copy

; ------------------------------------------------------------------------------
; ����: AppZiInit
; ����: ����һ������
; ����: r0	--	��ʼ��ַ
;		r1	--	��ֹ��ַ����һ��ַ
; ����: ��
; ------------------------------------------------------------------------------ 
AppZiInit
		MOV     r2, #0
		CMP     r0, r1          ; loop whilst r0 < r1
		STRLO   r2, [r0], #4
		BLO     AppZiInit 
		MOV     pc, lr          ; return from subroutine zi_init



; ����Ϊ����ĺ���

; ------------------------------------------------------------------------------
; ����: 
; ����: ��ʼ��record���������������ֱ����scatter�ļ��ж����һ��
; ����: ��
; ����: ��
; ------------------------------------------------------------------------------ 
IE_InitToSdram
        STMFD sp!,{lr}
        AppMacro_RWZI_Init SDRAM_IE_MEMORYPOOL
        LDMFD sp!,{pc}               
        
Gsm_InitToSdram
        STMFD sp!,{lr}
        AppMacro_RWZI_Init GSM_OVERLAY_SDRAM
        LDMFD sp!,{pc}  

Wifi_InitToSdram
        STMFD sp!,{lr}
        AppMacro_RWZI_Init WIFI_OVERLAY_SDRAM
        LDMFD sp!,{pc}  
        
Hyc_SysSleepOverlay
        STMFD sp!,{lr}
        AppMacro_RWZI_Init SYS_SLEEP
        LDMFD sp!,{pc}

TcpIp_InitMemory
        STMFD sp!,{lr}
        AppMacro_RWZI_Init SDRAM_TCPIP_MEMORY
        LDMFD sp!,{pc} 

PPP_InitMemory              
        STMFD sp!,{lr}
        AppMacro_RWZI_Init SDRAM_PPP_MEMORY
        LDMFD sp!,{pc}             
                
USB_InitMemory
        STMFD sp!,{lr}
        AppMacro_RWZI_Init SRAM_USB_MEMORY
        LDMFD sp!,{pc}
        
Nand_InitToSdram
     	STMFD sp!,{lr}
        AppMacro_RWZI_Init NAND_OVERLAY_SDRAM
        LDMFD sp!,{pc}
        
HandWriting_InitMemory
		STMFD sp!,{lr}
        AppMacro_RWZI_Init HYHAND_OVERLAY_SDRAM
        LDMFD sp!,{pc}  
        		        
        END        