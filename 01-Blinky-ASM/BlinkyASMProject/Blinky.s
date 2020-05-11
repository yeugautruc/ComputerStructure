; Defines aus lm4f120h5qr.h:
GPIO_PORTA_DATA_R  EQU 0x400043FC ; Toggle grüne Led in diesem Register
GPIO_PORTA_DEN_R   EQU 0x4000451C ; Digital-Enable-Register
GPIO_PORTA_DIR_R   EQU 0x40004400 ; DIRECTION-Register -> gewünschten Port PA7 (0x80) auf 1 (Ausgang) setzen.

GPIO_PORTF_DATA_R 	EQU 0x400253FC  ; Use this register to use button
GPIO_PORTF_DIR_R	EQU 0x40025400	; DIRECTION-Register
GPIO_PORTF_DEN_R	EQU 0x4002551C  ; Digital-Enable-Register
GPIO_PORTF_PUR_R 	EQU 0x40025510	; 

SYSCTL_RCGC2_R     EQU 0x400FE108 ; Clock auf GPIO-PORTA aktivieren. Clock ist default aus zwecks Strom sparen
SYSCTL_RCGC2_GPIOA EQU 0x00000001 ; Clock Port A
SYSCTL_RCGC2_GPIOF EQU	0x0000020 ; Clock Port F	

; eigene Defines
COUNTER EQU 0xFFFFFF ; Blinky-Delay
PA7     EQU 0x80	; Bitmuster für Port PA7 (grüne LED auf Boosterpack)
PA6		EQU 0x40 	; Bitmuster für Port PA6 (rote LED auf Boosterpack)
PA3		EQU 0x8		; Bitmuster für Port PA3 (blaue LED auf Boosterpack)
PA0		EQU 0xC8	; Bitmuster für Port PA0 (weiße LED auf Boosterpack)
PA2		EQU 0xC0	; Bitmuster für Port PA2 (gelbe LED auf Boosterpack)
PA4		EQU	0x48	; Bitmuster für Port PA4 (magenta LED auf Boosterpack)
PA5		EQU 0x88	; Bitmuster für Port PA5 (cyan LED auf Boosterpack)
	
PF		EQU 0x00	; LOW
P		EQU 0x10 	; HIGH

matrikel_1 EQU 70466827
matrikel_2 EQU 70465018
null	   EQU 0

; Assembler-Direktive -> .text-Bereich
	THUMB
	AREA    |.text|, CODE, READONLY, ALIGN=2
	EXPORT  Blinky
	ENTRY
	
; Funktionen
Blinky 
	; PORT A
	; Als erstes Clock auf Port A aktivieren
	LDR R1, =SYSCTL_RCGC2_R 	;get content of system clock          
	LDR R0, [R1]  				;get content of r1 to r0                  
	ORR R0, R0, #SYSCTL_RCGC2_GPIOA  
	STR R0, [R1]                    

	; Setze Port PA7 (0x80) als Ausgang
	LDR R1, =GPIO_PORTA_DIR_R   ;get portA to r1    
    LDR R0, [R1]                ;get content to r0    
    ORR R0, R0, #PA0           ;get adress PA7 (0x80)    
    STR R0, [R1]     				

	; Setze Port PA7 als Digital-Port
    LDR R1, =GPIO_PORTA_DEN_R   ; point value from portA digital enable( PC-relative addressing )  
    LDR R0, [R1]       			; R0 = value from pointer at R1            
    ORR R0, R0, #PA0		; set 1 bit high to enable digital port
    STR R0, [R1]          		; store register word, store value from R1 to R0
	
	;PORT F
	LDR R1, =SYSCTL_RCGC2_R         
	LDR R0, [R1]                    
	ORR R0, R0, #SYSCTL_RCGC2_GPIOF
	STR R0, [R1]    

	LDR R1, =GPIO_PORTF_DEN_R
	LDR R0, [R1]
	ORR R0, R0, #P				; enable port F
	STR R0, [R1]

	LDR R1, =GPIO_PORTF_PUR_R      
    LDR R0, [R1]                   
    ORR R0, R0, #P  			; set port F HIGH to captrue LOW
    STR R0, [R1] 
	
	LDR R1, =GPIO_PORTF_DIR_R      
    LDR R0, [R1]                   
    ORR R0, R0, #0      
    STR R0, [R1]
	
	MOV32 R8, #COUNTER 
	MOV R4, #0
	
Matrikelnummer
	LDR R5,	=matrikel_1		; save the value of matrikelnummer 1 in R5
	LDR R7, =matrikel_2     ; save the value of matrikelnummer 2 in R7
	LDR R4, =null
	LDR R6, =null
CountUp1
	ADD R4,	#1				; count the value from r4 up
	CMP R4, R5				; compare r4 with the value of matrikelnummer 1
	BEQ CountDown1			; if r4 = matrikelnummer 1, then do CountDown
	B CountUp1				; if not count the value down again
CountDown1
	SUB R4,	#1				; count the value from r4 down
	CMP R4, #null 			; compare r4 with value 0
	BEQ CountUp2			; if r4 = 0, then do CountUp2
	B CountDown1			; if not count the value down again
CountUp2
	ADD R6,	#1				; count the value from r6 up
	CMP R6, R7				; compare r6 with the value of matrikelnummer 2
	BEQ CountDown2			; if r6 = matrikelnummer 2, then do CountDown
	B CountUp2				; if not count the value down again
CountDown2
	SUB R6,	#1				; count the value from r4 down
	CMP R6, #null 			; compare r6 with value 0
	BEQ Button				; if r6 = 0, then do Button
	B CountDown2			; if not count the value down again	
	
Button 
	LDR R1, =GPIO_PORTF_DATA_R  ; Load value from port F to R1
    LDR R0, [R1] 
	CMP R0, #PF					; Compare with LOW
	BEQ Toggle 					; If LOW toggle the LED
	
	B Button						; If not LOW, return to begin from Button
	
Toggle 
	; Toggle the White-LED
	LDR R1, =GPIO_PORTA_DATA_R 
	LDR R0, [R1] 				; load value from port A to R0
	MOV R0, #PA0  				; add value from Bit-Muster to White-LED             
	STR R0, [R1]  				
	BL delay 	
	; Toggle the Red-LED	
	LDR R1, =GPIO_PORTA_DATA_R      
    LDR R0, [R1]                   
    MOV R0,	#0x78      			; add value from Bit-Muster to Red-LED
    STR R0, [R1] 
	BL delay
	; Toggle the Yellow-LED
	LDR R1, =GPIO_PORTA_DATA_R      
    LDR R0, [R1]                   
    MOV R0,	#PA2      			; add value from Bit-Muster to Yellow-LED
    STR R0, [R1] 
	BL delay
	; Toggle the Green-LED
	LDR R1, =GPIO_PORTA_DATA_R      
    LDR R0, [R1]                   
    MOV R0,	#PA7      			; add value from Bit-Muster to Green-LED
    STR R0, [R1] 
	BL delay
	; Toggle the Cyan-LED
	LDR R1, =GPIO_PORTA_DATA_R      
    LDR R0, [R1]                   
    MOV R0,	#PA5     			; add value from Bit-Muster to Cyan-LED
    STR R0, [R1] 
	BL delay
	; Toggle the Blue-LED
	LDR R1, =GPIO_PORTA_DATA_R      
    LDR R0, [R1]                   
    MOV R0,	#PA3     			; add value from Bit-Muster to Blue-LED
    STR R0, [R1] 
	BL delay
	; Toggle the Magenta-LED
	LDR R1, =GPIO_PORTA_DATA_R      
    LDR R0, [R1]                   
    MOV R0,	#PA4     			; add value from Bit-Muster to Magenta-LED
    STR R0, [R1] 
	BL delay
	
	B Toggle 					; call Toggle again to make infinite loop

delay MOV32 R8, #COUNTER 		; move delay value to r8

dl1
	SUBS R8, #1 				; count down to 0 = wait
	BNE dl1
	BX LR 						; return to caller
	
	ALIGN      
	END  
           