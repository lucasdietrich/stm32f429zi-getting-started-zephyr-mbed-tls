cmake_minimum_required(VERSION 3.13.1)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(stm32f429zi-getting-started-zephyr-mbed-tls)

FILE(GLOB app_sources ../src/*.c*)
target_sources(app PRIVATE ${app_sources})

# content.html
# set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)

# generate_inc_file_for_target(app src/content.html ${gen_dir}/content.html.inc)