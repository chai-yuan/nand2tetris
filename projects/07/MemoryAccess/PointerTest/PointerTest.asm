	@3030 // push constant 3030
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@SP // pop pointer 0
	AM=M-1
	D=M
	@R3
	M=D
	@3040 // push constant 3040
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@SP // pop pointer 1
	AM=M-1
	D=M
	@R4
	M=D
	@32 // push constant 32
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THIS // pop this 2
	D=M
	@2
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
	@46 // push constant 46
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THAT // pop that 6
	D=M
	@6
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
	@R3 // push pointer 0
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@R4 // push pointer 1
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@SP // add
	AM=M-1
	D=M
	A=A-1
	M=D+M
	@THIS // push this 2
	D=M
	@2
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@SP // sub
	AM=M-1
	D=M
	A=A-1
	M=M-D
	@THAT // push that 6
	D=M
	@6
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@SP // add
	AM=M-1
	D=M
	A=A-1
	M=D+M
