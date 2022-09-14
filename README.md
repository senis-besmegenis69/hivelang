
# Project hivelang


## The hiveLang toolkit

HLang compiler - [hivec](./hivec/README.md)<br>
HLang standard library - [hivestd](#)<br>
HLang package manager - [hivepm](#)<br>


## The hiveLang syntax

The hivelang will have the following keywords:

 - `import`<br>usage:
	```
	import "std.io.hlang"
	```
 - `if`<br>usage:
	```
	1 1 == if then
		// ...
	end
	```
 - `then`<br>usage:
	```
	1 1 == if then
		// ...
	end
	```
 - `else`<br>usage:
	```
	1 1 == if then
		// ...
	else
		// ...
	end
	```
 - `while`<br>usage:
	```
	10 while dup 0 > then
		// ...
	end
	```
 - `end`<br>usage:
	```
	1 1 == if then
		// ...
	end
	```

Default functions:
 - `clone`<br>usage:
	```
	5 clone // puts 2 5's in the stack
	```
 - `drop`<br>usage:
	```
	7 5 drop // pops 5 from the stack
	```
