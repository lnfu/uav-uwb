#!/bin/sh
# Does clang-tidy on the host build. Build the tests first.
set -eu
cd "$(dirname "$0")"

# clang does not know these GCC options
gcc_only='fno-malloc-dce|fno-tree-switch-conversion|fstrict-volatile-bitfields'
gcc_only="$gcc_only|fzero-init-padding-bits=all|Wno-old-style-declaration|fanalyzer"

mkdir -p build/clang_tidy
sed -E "s/ -($gcc_only)//g" build/compile_commands.json \
    >build/clang_tidy/compile_commands.json
clang-tidy --quiet -p build/clang_tidy ../*.c main/*.c
