set(MY_CXX_COMPILLER_FLAGS
    -fdiagnostics-color=always
    -fstack-protector-strong
    
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wfloat-equal
    -Wconversion
    -Wuninitialized
)
set(MY_CXX_LINKER_FLAGS "")

if(BUILD_WITH_SANITIZERS)
    list(APPEND MY_CXX_COMPILLER_FLAGS
        -Og
        -fno-omit-frame-pointer
        -ggdb
        -g3
        -fsanitize=address,undefined,leak,signed-integer-overflow,pointer-subtract,pointer-compare
        -fsanitize-address-use-after-scope
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls        
    )
    list(APPEND MY_CXX_LINKER_FLAGS
        -fsanitize=address,undefined,leak,signed-integer-overflow,pointer-subtract,pointer-compare
        -fsanitize-address-use-after-scope        
    )
endif()
set(test_list
    cxx17opts.cxx
    cxx20opts.cxx
    )
set(lib_link_list
    cxx20opts
    )
foreach(i ${test_list})
    add_executable(${i}.test ${i})
    target_link_libraries(${i}.test PUBLIC ${lib_link_list})
    target_link_options(${i}.test PUBLIC ${MY_CXX_LINKER_FLAGS})
    target_compile_options(${i}.test PUBLIC ${MY_CXX_COMPILLER_FLAGS})
    add_test(${i} ${i}.test) # compile && run tests.
endforeach()

target_compile_features(cxx20opts.cxx.test PUBLIC cxx_std_20)
target_compile_features(cxx17opts.cxx.test PUBLIC cxx_std_17) # cxx_std_17
