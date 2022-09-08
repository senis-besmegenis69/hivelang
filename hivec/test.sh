
#!/bin/sh

rm -f ./a.txt
make
./bin/linux/debug/hivec ./tests/00-custom.hlang
