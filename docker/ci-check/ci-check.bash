#!/bin/bash

find src -type f \( -name '*.cpp' -o -name '*.hpp' \) -print0 \
            | xargs -0 clang-format --dry-run --Werror
cppcheck --enable=warning,performance,style --suppress=missingOverride --inline-suppr --error-exitcode=1 src