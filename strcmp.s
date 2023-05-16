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
            ; xor rax, rax
             
            ; cld 
            
            ; mov rcx, rsi
            ; call strlen 

            ; xor rcx, rcx
            ; mov ecx, eax
            ; inc ecx  

            mov ecx, 8
            ; xor eax, eax 
            ; xor ebx, ebx 

; .next:      
            ; mov al, byte [rsi]
            ; cmp al, byte [rdi]
            mov eax, dword [rsi]
            cmp eax, dword [rdi]
            jne .end 

            ; inc rsi 
            ; inc rdi
            ; mov qword [rsi_], rsi 
            ; mov qword [rdi_], rdi 
            ; mov qword [rax_], rax 
            ; mov qword [rcx_], rcx 

            ; mov rdx, rsi 
            ; mov rsi, rdi 
            ; mov rdi, msg 

            ; call printf

            ; mov rcx, qword [rcx_] 
            ; mov rax, qword [rax_] 
            ; mov rsi, qword [rsi_] 
            ; mov rdi, qword [rdi_] 

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

            ; dec ecx 
            ; test ecx, ecx 
            ; jne .next 
            ; loop .next 

            ; dec rsi 
            ; dec rdi 
            ; sub rsi, 4
            ; sub rdi, 4

.end:
            ; mov bl, byte [rdi]
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
