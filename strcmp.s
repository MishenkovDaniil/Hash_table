;=============================================
;=============================================
section .text

global m_strcmp

extern printf 
extern strlen 

;=============================================
;int strcmp (const char *str1, const char *str2)                               
;--------------------------------------------
;Entry: rdi = addr: str1, rsi = addr: str2 
;Exit: eax = result
;Destroys: rcx, rdi, rsi 
;--------------------------------------------
m_strcmp: 
            pop qword [ret_addr]
    
            mov rcx, rdi
            call strlen

            xor rcx, rcx  
            mov ecx, eax 
            inc ecx  

            repe cmpsb 
            
            xor eax, eax 
            xor ebx, ebx 

            dec rdi 
            dec rsi 
                
            mov al, byte [rdi]
            ;sub al, byte [rsi]
            mov bl, byte [rsi]
            
            sub eax, ebx 

            ;call m_strcmp_main

            push qword [ret_addr]

            ret
;=============================================


;=============================================
;=============================================
section .data

ret_addr    dq 0
printing_str    db "hello %s %s", 0
rsi_            dq 0 
