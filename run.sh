cmake --build build --parallel 256 \
&& \
/home/rsoi/fix/build/src/tester/stateless-tester tree:4 string:none file:build/test-fix.wasm uint32:0 string:"$1"
