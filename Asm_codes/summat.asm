	 limm $sp, $zero, 1		# 0: $sp = 1
	 limm $t1, $zero, 10	# 1: $t1 = 10
	 noimm $sp, $t1, sll	# 2: $sp = 1 << 10 = 1024
	 limm $t2, $zero, 16 	# d: $t2 = 16
	 limm $t1, $zero, 8		# 4: t1 = 8
	 limm $t0, $zero, 1		# 3: t0 = 1
	 noimm $t0, $t1, sll	# 5: t0 = 1 << 8 = 256 = 0x100
	 limm $a0, $zero, 0		# 3: $a0 = 0
	 noimm $a0, $t0, add	#  $a0 = 256
	 limm $t0, $zero, 1		# 3: t0 = 1
	 noimm $t0, $t1, sll	# 5: t0 = 1 << 8 = 256 = 0x100
	 noimm $t0, $t2, add    # $t0+=16=0x110=272
	 limm $a1, $zero, 0		# 3: $a1 = 0
	 noimm $a1, $t0, add	#  $a1 = 272
	 limm $t0, $zero, 1		# 3: t0 = 1
	 noimm $t0, $t1, sll	# 5: t0 = 1 << 8 = 256 = 0x100
   	 noimm $t0, $t2, add    # $t2=0x110
	 noimm $t0, $t2, add	# $t2=0x110+16=288
	 limm $v0, $zero, 0		# 3: $v0 = 0
	 noimm $v0, $t0, add	#  $v0 = 288
	 limm $t0, $zero, 4 	# d: $t0 = 4
	 limm $t1, $zero, 2 	# d: $t1 = 2
	 limm $s0, $zero, 0 	# d: $s0 = 0 = i
L1:	 limm $s1, $zero, 0  	# d: $s1 = 0 = j
L2: limm $t2, $zero, 0 		# $t2 = 0
	noimm $t2, $s0, add		# $t2=i
	noimm $t2, $t1, sll		# $t2=4*i
	noimm $t2, $s1, add		# $t2=i*4+j the line in the memory of the current node
	noimm $a0, $t2, add		# the address in memory of the first byte of the current node A[i][j]
	noimm $a1, $t2, add		# the address in memory of the first byte of the current node B[i][j]
	limm $s2, $zero, 0 		# $s2 = 0
	limm $t3, $zero, 0 		# $t3 = 0
	lw $s2, $a0, 0			# load A[i][j] to $s2
	noimm $t3, $s2, add		# $t3 += A[i][j] move A[i][j] to $t3
	limm $s2, $zero, 0 		# $s2 = 0
	lw $s2, $a1, 0			# load B[i][j] to $s2 
	noimm $t3, $s2, add		# $t3 += B[i][j] add B[i][j] to $t3
	noimm $a0, $t2, sub		# after saving this node's vale, return the value of A[0][0] to it's original value in prepration for the next iter.
	noimm $a1, $t2, sub		# after saving this node's vale, return the value of B[0][0] to it's original value in prepration for the next iter.
	noimm $v0, $t2, add		# the address in memory of the first byte of the current node C[i][j]
	sw $t3, $v0, 0			# store the value in register $t3 at address of C[i][j]
	noimm $v0, $t2, sub		# after saving this node's vale, return the value of A[0][0] to it's original value in prepration for the next iter.
	limm $t3, $zero, 1 		# $t3 = 1 (reusing $t3 register)
	noimm $s1, $t3, add		# j++
	bne $s1, $t0, L3		# brunch if j!=4 
	noimm $s0, $t3, add		# i++
	bne $s0, $t0, L4		# brunch if i!=4
	halt $zero, $zero, 0	# c: halt
L3:	j $zero, $zero, L2
L4:	j $zero, $zero, L1
	.word 0x100 20
	.word 0x101 30
	.word 0x102 90
	.word 0x103 90
	.word 0x104 20
	.word 0x105 30
	.word 0x106 90
	.word 0x107 90
	.word 0x108 20
	.word 0x109 30
	.word 0x10A 90
	.word 0x10B 90
	.word 0x10C 20
	.word 0x10D 30
	.word 0x10E 90
	.word 0x10F 90
	.word 0x110 -5
	.word 0x111 -5
	.word 0x112 -5
	.word 0x113 -5
	.word 0x114 -5
	.word 0x115 -5
	.word 0x116 -5
	.word 0x117 -5
	.word 0x118 -5
	.word 0x119 -5
	.word 0x11A -5
	.word 0x11B -5
	.word 0x11C -5
	.word 0x11D -5
	.word 0x11E -5
	.word 0x11F -5
	.word 0x120 0
	.word 0x121 0
	.word 0x122 0
	.word 0x123 0
	.word 0x124 0
	.word 0x125 0
	.word 0x126 0
	.word 0x127 0
	.word 0x128 0
	.word 0x129 0
	.word 0x12A 0
	.word 0x12B 0
	.word 0x12C 0
	.word 0x12D 0
	.word 0x12E 0
	.word 0x12F 0