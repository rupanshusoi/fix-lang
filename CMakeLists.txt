cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0115 NEW)

set(WASI_SDK $ENV{HOME}/wasm-toolchain/sysroot)
set(WASIX_DIR $ENV{HOME}/wasm-toolchain/sysroot/wasix)

option(BUILD_SYS_DRIVER "Build local machine version" ON)

# Use compiler from wasm-toolchain if building to wasm
set(CMAKE_C_COMPILER ${WASI_SDK}/bin/clang)
set(CMAKE_CXX_COMPILER ${WASI_SDK}/bin/clang++)
set(CMAKE_AR ${WASI_SDK}/bin/llvm-ar)
set(CMAKE_RANLIB ${WASI_SDK}/bin/llvm-ranlib)
LIST(APPEND CMAKE_PROGRAM_PATH ${WASI_SDK}/bin)
set(CMAKE_C_FLAGS "-I${WASIX_DIR}/include -I${WASI_SDK}/include -D_WASI_EMULATED_SIGNAL -D_WASI_EMULATED_MMAN -L${WASIX_DIR}/lib -lwasix -L${WASI_SDK}/lib -L${WASI_SDK}/lib/wasm32-wasi -lwasi-emulated-signal -lwasi-emulated-mman -z stack-size=1048576")
set(CMAKE_CXX_FLAGS "-I${WASIX_DIR}/include -I${WASI_SDK}/include -D_WASI_EMULATED_SIGNAL -D_WASI_EMULATED_MMAN -D_LIBCPP_NO_EXCEPTIONS -D_LIBCXXABI_NO_EXCEPTIONS -mreference-types -Os")
add_link_options(-L${WASIX_DIR}/lib -lwasix -L${WASI_SDK}/lib -L${WASI_SDK}/lib/wasm32-wasi -lwasi-emulated-signal -lwasi-emulated-mman -z stack-size=1048576)

project(fixtmp)

include_directories(cereal/include)

add_link_options(-nostdlib -Wl,--no-entry -Wl,--allow-undefined
                 $ENV{HOME}/wasm-toolchain/sysroot/lib/wasm32-wasi/libc++.a
                 $ENV{HOME}/wasm-toolchain/sysroot/lib/wasm32-wasi/libc++abi.a
                 $ENV{HOME}/wasm-toolchain/sysroot/lib/wasm32-wasi/libc.a
                 $ENV{HOME}/wasm-toolchain/sysroot/lib/clang/16.0.0/lib/wasi/libclang_rt.builtins-wasm32.a)

add_library(stub-obj OBJECT "ignore.cc")
add_executable(test "interpreter.cc")
target_link_libraries(test stub-obj)

add_custom_command(
  OUTPUT "linked_with_asm_no_apply.wasm"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/linked_with_asm_flatware.wasm
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/export-audit/export-audit
          --enable-multi-memory
          --enable-exceptions
          -r _fixpoint_apply
          ${CMAKE_CURRENT_SOURCE_DIR}/linked_with_asm_flatware.wasm
          linked_with_asm_no_apply.wasm
)

add_custom_command(
  OUTPUT "linked_with_asm_no_memory.wasm"
  DEPENDS linked_with_asm_no_apply.wasm
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/export-audit/export-audit
          --enable-multi-memory
          --enable-exceptions
          -r memory
          linked_with_asm_no_apply.wasm
          linked_with_asm_no_memory.wasm
)

add_custom_command(
  OUTPUT "test-fix-no-storage.wasm"
  DEPENDS linked_with_asm_no_memory.wasm
          test
          interpreter.cc
          interpreter.hh
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --debug-names
          --enable-multi-memory
          --enable-exceptions
          test
          linked_with_asm_no_memory.wasm
          -m wasi_command
          -n wasi_snapshot_preview1
          -o "test-fix-no-storage.wasm"
)

add_custom_command(
  OUTPUT "test-fix.wasm"
  DEPENDS test-fix-no-storage.wasm
          test
          interpreter.cc
          ${CMAKE_CURRENT_SOURCE_DIR}/fixpoint_storage.wasm
  COMMAND $ENV{HOME}/wasm-toolchain/wasm-tools/build/src/module-combiner/wasmlink
          --debug-names
          --enable-multi-memory
          --enable-exceptions
          test-fix-no-storage.wasm
          ${CMAKE_CURRENT_SOURCE_DIR}/fixpoint_storage.wasm
          -m wasi_command
          -n fixpoint_storage
          -o "test-fix.wasm"
)

add_custom_target(test-fix ALL DEPENDS test-fix.wasm)
