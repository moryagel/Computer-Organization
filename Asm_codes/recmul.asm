	limm $sp, $zero, 1		# 0: $sp = 1
	limm $t1, $zero, 10		# 1: $t1 = 10
	noimm $sp, $t1, sll		# 2: $sp = 1 << 10 = 1024
	limm $t0, $zero, 1		# 3: t0 = 1
	limm $t1, $zero, 8		# 4: t1 = 8
	noimm $t0, $t1, sll		# 5: t0 = 1 << 8 = 256 = 0x100
	lw $a0, $t0, 0			# 6: load n from address 256
	limm $t0, $zero, 1		# 8: t0 = 1
	limm $t1, $zero, 8		# 9: t1 = 8
    noimm $t0, $t1, sll		# a: t0 = 1 << 8 = 256
	lw $a1, $t0, 1			# 6: load k from address 257=0x101
	jal $ra, $zero, recmul	# 7: calc $v0 = recmul(n,k)
	limm $t0, $zero, 1		# 8: t0 = 1
	limm $t1, $zero, 8		# 9: t1 = 8
    noimm $t0, $t1, sll		# a: t0 = 1 << 8 = 256
	sw $v0, $t0, 2			# b: store recmul(n,k) in address 258=0x102 
	halt $zero, $zero, 0	# c: halt
recmul:
	limm $t0, $zero, 4		# d: $t0 = 4
	noimm $sp, $t0, sub		# e: adjust stack for 4 items
	sw $s0, $sp, 3			# f: save $s0
	sw $ra, $sp, 2			# 10: save return address
	sw $a1, $sp, 1			# 11: save argument
	sw $a0, $sp, 0			# 11: save argument
	limm $t1, $zero, 1		# 12: $t1 = 1
	noimm $a0, $t1, sub		# 13: $a0 = x - 1
	limm $t2, $zero, 1		# d: $t2 = 1
	noimm $t2, $a0, add		# 13: $t2 = x 
	bgtz $t2, $zero, else	# 14: jump to else if x > 0
	limm $v0, $zero, 0		# 16: $v0 = 0 else return 0
	noimm $sp, $t0, add		# 18: pop 4 items from stack
	noimm $ra, $zero, jr	# 19: and return
else:
	jal $ra, $zero, recmul	# 1a: calc $v0=recmul(n-1,k)
	limm $s0, $zero, 0		# 1b: $s0 = 0
	noimm $s0, $a1, add		# 1c: $s0 = k
	noimm $v0, $s0, add		# 1c: $v0 = recmul(n-1,k)+k
	lw $a0, $sp, 0			# 22: restore $a0
	lw $a1, $sp, 1			# 22: restore $a1
	lw $ra, $sp, 2			# 23: restore $ra
	lw $s0, $sp, 3			# 24: restore $s0
	limm $t0, $zero, 4		# 25: $t0 = 4
	noimm $sp, $t0, add		# 26: pop 4 items from stack
	noimm $ra, $zero, jr	# 27: and return
	.word 256 4 #for example I chose n=2
	.word 257 3 #for example I chose k=3
