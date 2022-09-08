
iota_counter = 0


def iota(reset=False):
	global iota_counter
	iota_counter += 1

	if reset:
		iota_counter = 0
	
	return iota_counter


OP_PUSH = iota(True)
OP_PLUS = iota()
OP_MINUS = iota()
OP_DUMP = iota()


def push(value):
	return (OP_PUSH, value)


def plus():
	return (OP_PLUS, )


def minus():
	return (OP_MINUS, )


def dump():
	return (OP_DUMP, )


def simulate(program):
	stack = []

	for op in program:
		if op[0] == OP_PUSH:
			stack.append(op[1])
		elif op[0] == OP_PLUS:
			a = stack.pop()
			b = stack.pop()
			stack.append(a + b)
		elif op[0] == OP_MINUS:
			a = stack.pop()
			b = stack.pop()
			stack.append(b - a)
		elif op[0] == OP_DUMP:
			print(stack.pop())
		else:
			assert False, "Unknown operation encountered!"


def compile(program):
	pass


if __name__ == '__main__':
	if sys.argv < 2:
		print("Usage: hivec [Options] <commands>")


program = [
	push(34),
	push(35),
	minus(),
	dump(),
	push(420),
	dump()
]

simulate(program)
