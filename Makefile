CC = g++
Flags = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

compile: main.cpp hash_table.cpp list/list.cpp  parser/parser.cpp hash_functions/hash_functions.cpp root_mean_square/rms.cpp strcmp.s
	$(CC) -O2 -c -mavx2 main.cpp hash_table.cpp list/list.cpp parser/parser.cpp root_mean_square/rms.cpp hash_functions/hash_functions.cpp
	nasm -f elf64 strcmp.s
	g++ -no-pie main.o hash_table.o parser.o list.o hash_functions.o rms.o strcmp.o -o hash 
 
all_C:	main.cpp hash_table.cpp list/list.cpp  parser/parser.cpp hash_functions/hash_functions.cpp strcmp.s 
	$(CC) -O2 -S -mavx2 hash_table.cpp list/list.cpp parser/parser.cpp hash_functions/hash_functions.cpp strcmp.s

run: 
	./hash 

C_files: main.cpp hash_table.cpp list/list.cpp  parser/parser.cpp hash_functions/hash_functions.cpp
	$(CC) -O0 main.cpp hash_table.cpp list/list.cpp parser/parser.cpp hash_functions/hash_functions.cpp -o hash 

debug: 
	valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes ./hash 
