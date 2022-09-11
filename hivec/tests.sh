
#!/bin/sh

TESTS_DIR=$PWD/tests

for FILE in $TESTS_DIR/*.hlang; do
	echo "$FILE:"
	./bin/linux/debug/hivec $FILE
	echo ""
done
