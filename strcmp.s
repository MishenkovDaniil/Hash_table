;=============================================
;=============================================
section .text

global m_strcmp

; extern printf 
extern strlen 

;=============================================
;int strcmp (const char *str1, const char *str2)                               
;--------------------------------------------
;Entry: rdi = addr: str1, rsi = addr: str2 
;Exit: eax = result
;Destroys: ebx, rcx, rdi, rsi 
;--------------------------------------------
m_strcmp: 
            ; pop qword [ret_addr]

            xor rax, rax

            mov rcx, rdi
            call strlen 

            xor rcx, rcx
            mov ecx, eax
            inc ecx  

            xor eax, eax 
            xor ebx, ebx 

.next:      
            mov al, byte [rsi]
            cmp al, byte [rdi]
            jne .end 

            inc rsi 
            inc rdi

            dec ecx 
            test ecx, ecx 
            jne .next 
            ; loop .next 

            dec rsi 
            dec rdi 
.end:
            mov bl, byte [rdi]
            
            sub eax, ebx 

            ; push qword [ret_addr]

            ret 
;=============================================
;=============================================


section .data

ret_addr        dq 0
printing_str    db "hello %s %s", 0
rsi_            dq 0 
