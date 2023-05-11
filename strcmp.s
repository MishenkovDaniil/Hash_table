;=============================================
;=============================================
section .text

global m_strcmp

; extern printf 
; extern strlen 

;=============================================
;int strcmp (const char *str1, const char *str2)                               
;--------------------------------------------
;Entry: rdi = addr: str1, rsi = addr: str2 
;Exit: eax = result
;Destroys: rcx, rdi, rsi 
;--------------------------------------------
m_strcmp: 
            ; pop qword [ret_addr]

            xor rax, rax
            ;mov rcx, rdi
            ; mov r9, rdi  
            call m_strlen
            ; mov rdi, r9 
            sub rdi, rax 

            xor rcx, rcx  
            mov ecx, eax 
            inc ecx  

            repe cmpsb 
            
            xor eax, eax 
            xor ebx, ebx 

            dec rdi 
            dec rsi 
                
            mov al, byte [rdi]
            mov bl, byte [rsi]
            
            sub eax, ebx 

            ; push qword [ret_addr]

            ret
;=============================================


;=============================================
;int strlen (const char *str1)                               
;--------------------------------------------
;Entry: rdi = addr;
;Exit: eax = result
;Destroys: rdi, rcx 
;=============================================
m_strlen:   
            mov al, 0
            ; mov rdi, rcx 
            mov rcx, 0xff 

            repne scasb 
            
            mov eax, 0xff 
            sub eax, ecx 

            ret 
;=============================================


section .data

ret_addr        dq 0
printing_str    db "hello %s %s", 0
rsi_            dq 0 
