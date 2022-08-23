	@0 // push constant 0
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@LCL // pop local 0
	D=M
	@0
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
(BasicLoop$$LOOP_START)// label LOOP_START
	@ARG // push argument 0
	D=M
	@0
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@LCL // push local 0
	D=M
	@0
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
	@LCL // pop local 0
	D=M
	@0
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
	@ARG // push argument 0
	D=M
	@0
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@1 // push constant 1
	D=A
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
	@ARG // pop argument 0
	D=M
	@0
	D=D+A
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	A=M
	M=D
	@ARG // push argument 0
	D=M
	@0
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@SP// if-goto LOOP_START
	AM=M-1
	D=M
	@BasicLoop$$LOOP_START
	D;JNE
	@LCL // push local 0
	D=M
	@0
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
