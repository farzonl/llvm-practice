./llvmJit.sh

libextension=""
if [ "$(uname -s)" = "Darwin" ]; then
    libextension="dylib"
elif [ "$(uname -s)" = "Linux" ]; then
    libextension="so"
else
    echo "OS $(uname -s) is unsupported."
fi

echo "Instruction Count Pass:"
opt -load-pass-plugin "build/src/libInstructionCountPass.$libextension" -passes="print<inst-count-pass>"  -disable-output build/test/test1.bc 
echo "Basic Block Count Pass:"
opt -load-pass-plugin "build/src/libBasicBlockCounterPass.$libextension" -passes="print<bb-count-pass>"  -disable-output build/test/test1.bc
echo "CFG Edge Count Pass:"
opt -load-pass-plugin "build/src/libCFGEdgeCounterPass.$libextension" -passes="print<cfg-edge-count-pass>"  -disable-output build/test/test1.bc
echo "Back Edge Count Pass:"
opt -load-pass-plugin "build/src/libBackEdgeDetectorPass.$libextension" -passes="print<bed-count-pass>"  -disable-output build/test/test1.bc