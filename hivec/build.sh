
#!/bin/sh

set -xe

nasm -felf64 main.asm
ld -o main.out main.o
