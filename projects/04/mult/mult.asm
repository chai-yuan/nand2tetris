// 将寄存器R0和R1相乘，结果放入R2
    M[2]=0      	// 初始化
    
    D = M[0]    	// 判断第一个数是否小于0
    @END
    D;JLE
(LOOP)
    MD = M[1] - 1   // 循环变量--
    @END
    D;JLT   		// 如果循环到小于0，跳出

    D = M[0]            
    M[2] = M + D   	// 循环体内容，将R2中的值每次增加

    // @LOOP   		// 循环
    // 0;JMP
    goto LOOP
(END)       