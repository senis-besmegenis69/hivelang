
# Project hivelang (HLang)


## HLang toolkit

HLang compiler - [hivec](./hivec/README.md)<br>
HLang standard library - [hivestd](#)<br>
HLang  package manager - [hivepm](#)<br>


## HLang syntax

The hivelang will have the following keywords:

 - `import`<br>usage:
	```
	import "std.io.hlang";
	```

 - `export`<br>usage:
	```
	export proc myFunc() -> void:
		// The code...
	end
	```

 - `module`<br>usage:
	```
	export module math:
		// Functions and everything else...
	end
	```

 - `sizeOf`<br>usage:
	```
	u64 size1 = sizeOf(f32);
	u64 size2 = sizeOf("Hello!\n"); // With the \0
	```

 - `typeOf`<br>usage:
	```
	i32 val = 0;

	if typeOf(val) == i32:
		// The code...
	end
	```

 - `inline`<br>usage:
	```
	inline proc myFunc() -> void:
		// The code...
	end
	```

 - `proc`<br>usage:
	```
	proc myFunc() -> void:
		// The code...
	end
	```

 - `constproc`<br>usage:
	```
	constproc myFunc() -> void:
		// The code...
	end
	```

 - `const`<br>usage:
	```
	const i32 val = 4;
	```

 - `return`<br>usage:
	```
	proc myFunc(i32 val) -> i32:
		return val + 5;
	end
	```

 - `if`<br>usage:
	```
	if a == b:
		// The code...
	end
	```

 - `switch`<br>usage:
	```
	switch a:
		case 0:
		end

		case 1:
		case 2:
		end

		case 3:
		end

		default:
		end
	end
	```

 - `for`<br>usage:
	```
	for i32 i = 0; i < 10; i++:
		// The code...
	end
	```

 - `while`<br>usage:
	```
	while a != b:
		// The code...
	end
	```

 - `use`<br>usage:
	```
	i32 a = 0, b = 1;

	i32 c = use a, b -> i32:
		return a + b;
	end
	```

 - `end`<br>usage:
	```
	if a == b:
		// The code...
	end
	```

 - `alias`<br>usage:
	```
	alias id_t = i32;
	```

 - `struct`<br>usage:
	```
	struct Vec2:
		f32 x;
		f32 y;
	end
	```

 - `enum`<br>usage:
	```
	enum Flag:
		One = 0,
		Two
	end
	```

 - `void`<br>usage:
	```
	proc myFunc() -> void:
	end
	```

 - `null`<br>usage:
	```
	char* p = null;
	```

 - `bool`<br>usage:
	```
	bool a = true;
	bool b = false;
	```

 - `true`<br>usage:
	```
	bool a = true;
	```

 - `false`<br>usage:
	```
	bool a = false;
	```

 - `char`<br>usage:
	```
	char c1 = 'k';
	char c2 = 75;
	```

 - `i8`<br>usage:
	```
	i8 v = 2;
	```

 - `i16`<br>usage:
	```
	i16 v = 2;
	```

 - `i32`<br>usage:
	```
	i32 v = 2;
	```

 - `i64`<br>usage:
	```
	i64 v = 2;
	```

 - `u8`<br>usage:
	```
	u8 v = 2;
	```

 - `u16`<br>usage:
	```
	u16 v = 2;
	```

 - `u32`<br>usage:
	```
	u32 v = 2;
	```

 - `u64`<br>usage:
	```
	u64 v = 2;
	```

 - `f32`<br>usage:
	```
	f32 v = 2;
	```

 - `f64`<br>usage:
	```
	f64 v = 2;
	```

 - `string`<br>usage:
	```
	string str = "Hello!\n";
	u64 length = str.length();
	```


An example program:

```hlang
import "std.io.hlang"


proc linearSum(i32 count, i32[] values) -> i64:
	i64 sum = 0i;

	for i32 i = 0; i < count; i++:
		sum += values[i];
	end

	return sum;
end


proc main(const i32 argc, const string[] args) -> i32:
	if argc > 1:
		for i32 i = 1; i < argc; i++:
			std::print(f"{i}-th argument is {args[i]}\n");
		end

		return argc;
	end

	if argc < 33:
		i32[] values = i32[32];

		for i32 i = 1; i < argc; i++:
			values[i] = args[i].length();
		end

		return linearSum(argc - 1, values);
	end

	return 0u;
end

// Parentheses can be used inside if, for, and other statements / expressions.
```
