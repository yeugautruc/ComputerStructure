; Stack-Define
Stack   EQU     0x00000100
        AREA    STACK, NOINIT, READWRITE, ALIGN=3
StackMem
        SPACE   Stack
__initial_sp

; Reset-Bereich + Vektoren definieren
		AREA    RESET, CODE, READONLY
        THUMB
        EXPORT  __Vectors
__Vectors
        DCD     StackMem + Stack       
        DCD     Reset_Handler              

		EXPORT  Reset_Handler
Reset_Handler
        IMPORT  Blinky
        B       Blinky     ;Branch / Springe zu Blinky.s
        END
