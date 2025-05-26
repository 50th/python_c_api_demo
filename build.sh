gcc example.c -I/usr/include/python3.9 -lpython3.9 -o example
gcc example_threading.c -I/usr/include/python3.9 -lpython3.9 -lpthread -o example_threading

aarch64-linux-gnu-gcc example_threading.c -o example_threading_arm64 -I./arm64-python3.12/include/python3.12 -L./arm64-python3.12/lib -lpython3.12 -lpthread -lm -lutil -ldl -Wl,-rpath,.
