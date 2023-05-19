cmake --build build --parallel 256 &&
/home/rsoi/fix/build/src/tester/stateless-tester \
tree:7 string:none file:build/test-fix.wasm uint32:1 uint32:1 string:"+" \
  thunk: tree:5 string:none file:build/test-fix.wasm uint32:1 uint32:0 string:"988" \
  thunk: tree:5 string:none file:build/test-fix.wasm uint32:1 uint32:0 string:"1"
