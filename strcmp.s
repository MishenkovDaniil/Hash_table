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
            ; pop qword [ret_addr]

            mov ecx, 8
            
            mov eax, dword [rsi]
            cmp eax, dword [rdi]
            jne .end 

            add rsi, 4
            add rdi, 4

            mov eax, dword [rsi]
            cmp eax, dword [rdi]
            jne .end 

            add rsi, 4
            add rdi, 4

            mov eax, dword [rsi]
            cmp eax, dword [rdi]
            jne .end
            
            add rsi, 4
            add rdi, 4

            mov eax, dword [rsi]
            cmp eax, dword [rdi]
            jne .end

            add rsi, 4
            add rdi, 4

            mov eax, dword [rsi]
            cmp eax, dword [rdi]
            jne .end

            add rsi, 4
            add rdi, 4

            mov eax, dword [rsi]
            cmp eax, dword [rdi]
            jne .end

            add rsi, 4
            add rdi, 4

            mov eax, dword [rsi]
            cmp eax, dword [rdi]
            jne .end
            
            add rsi, 4
            add rdi, 4

            mov eax, dword [rsi]
            cmp eax, dword [rdi]
            jne .end

.end:
            mov ebx, dword [rdi]
            
            sub eax, ebx 
            ; push qword [ret_addr]

            ret 
;=============================================
;=============================================


section .data

ret_addr        dq 0
printing_str    db "hello %s %s", 0
rsi_            dq 0
rdi_            dq 0
rax_            dq 0
rcx_            dq 0
msg             db "%s %s", 0
