
## Windows Build Instructions
- First build llvm
```bat
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64  

cmake -G Ninja -B llvm_win_clang_build -S llvm-project\llvm -DLLVM_ENABLE_PROJECTS=llvm;clang;lld -DCMAKE_C_COMPILER=clang.exe -DCMAKE_CXX_COMPILER=clang++.exe -DCMAKE_BUILD_TYPE=Release -DLLVM_HOST_TRIPLE=x86_64 -DCMAKE_LINKER=lld.exe -DCMAKE_INSTALL_PREFIX="D:/projects/llvm_install"
-DLLVM_EXPORT_SYMBOLS_FOR_PLUGINS=TRUE

ninja -C llvm_win_clang_build
```
- Then install llvm
```bat
ninja -C llvm_win_clang_build install
```

- Then build llvm-practice
```bat
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64
cmake -G Ninja -B build -DLLVM_DIR="D:/projects/llvm-project/llvm/cmake/modules" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang.exe -DCMAKE_CXX_COMPILER=clang.exe -DCMAKE_LINKER=lld.exe -S .

ninja -C build
```

- Then test the plugin
```
```

## Docker Build & Run 
- Build: `docker build -t llvm-practice:latest .`
- Run: `docker run  --name lllvm-practice_vm -it llvm-practice:latest`