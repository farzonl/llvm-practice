./llvmJit.sh
echo "Instruction Count Pass:"
opt -load-pass-plugin build/src/libInstructionCountPass.dylib -passes="print<inst-count-pass>"  -disable-output build/test/test1.bc 
echo "Basic Block Count Pass:"
opt -load-pass-plugin build/src/libBasicBlockCounterPass.dylib -passes="print<bb-count-pass>"  -disable-output build/test/test1.bc
echo "CFG Edge Count Pass:"
opt -load-pass-plugin build/src/libCFGEdgeCounterPass.dylib -passes="print<cfg-edge-count-pass>"  -disable-output build/test/test1.bc