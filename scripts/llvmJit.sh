mkdir build/test
clang -emit-llvm test/test1.c -c -o build/test/test1.bc
#lli build/test/test1.bc
clang -emit-llvm test/test2.c  -c -o build/test/test2.bc
#lli build/test/test2.bc
clang -emit-llvm test/test3.c  -c -o build/test/test3.bc
#lli build/test/test3.bc
