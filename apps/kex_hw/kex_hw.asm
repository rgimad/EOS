; Hello world KEX
; header:
use32              
        org     0
        db      'MENUET01'  ; magic
        dd      1           ; header version
        dd      START       ; entry point
        dd      I_END       ; program size
        dd      MEM         ; memory size
        dd      STACKTOP    ; stack top addr
        dd      0           ; buf for args
        dd      0           ; reversed

; code:

; edx - string
debug_outstr:
        mov  eax, 63
        mov  ebx, 1
@@:
        mov  cl, [edx]
        test cl, cl
        jz   @f
        int  0x40
        inc  edx
        jmp  @b
@@:
        ret

START:
        mov     edx, str1
        call    debug_outstr
        ret

 
; data:
str1             db 'Hello world', 0
 
; labels:
I_END:
  rb 4096               ; for stack
 
align 16
STACKTOP:               ; stack top label, stack grows downwards
                       
MEM:                    ; end