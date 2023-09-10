cmake_minimum_required(VERSION 3.27)

project(rude VERSION 0.0.1)

add_executable(rude rude.c)
target_compile_features(rude PRIVATE c_std_99)
