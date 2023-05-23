;=============================================
;=============================================
section .text

global m_strcmp

extern printf 
extern strlen 

;=============================================
;long strcmp (const char *str1, const char *str2)                               
;--------------------------------------------
;Entry: rdi = addr: str1, rsi = addr: str2 
;Exit: rax = result
;Destroys: rdi, rsi 
;--------------------------------------------
m_strcmp: 
            mov rax, qword [rsi]
            sub rax, qword [rdi]
            jne .end 

            mov rax, qword [rsi + 8]
            sub rax, qword [rdi + 8]
            jne .end 

            mov rax, qword [rsi + 16]
            sub rax, qword [rdi + 16]
            jne .end
            
            mov rax, qword [rsi + 24]
            sub rax, qword [rdi + 24]

.end:

            ret 
;=============================================
;=============================================
