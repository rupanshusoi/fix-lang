cmake --build build --parallel 256 \
&& \
/home/rsoi/fix/build/src/tester/stateless-tester tree:3 string:none file:build/test-fix.wasm string:"$1"
