# Add an executable for the test
add_executable(testByMinifb testByMinifb.c)

# Link the test executable with the library
target_link_libraries(testByMinifb qrcode)



