	limm $sp, $zero, 1		      # $sp = 1
	limm $t1, $zero, 10		      # $t1 = 10
	noimm $sp, $t1, sll		      # $sp = 1 << 10 = 1024
	limm $t3   ,$zero ,1     	  # t3 = 1
    limm $t0   ,$zero ,15     	  # t0 = 15
    limm $t1   ,$zero ,15    	  # t1 = 15
    limm $a0 , $zero , 8		  # a0 = 8

LOOP:
    limm $t2, $zero, 1			  # t2 = 1
    noimm $t2 , $a0 , sll		  # t2 = 256
	noimm $t2,   ,$t1,  add		  # t2 = 1 << 8 = 256 = 0x100
    lw   $s0    ,$t2   ,0  	      # s0 = MEM[t2+256]
    noimm  $t1  ,$t3   ,sub       # t1 -= 1
    noimm  $t2  ,$t3   ,sub		  # t2 -= 1
    lw   $s1    ,$t2   ,0	      # s1 = MEM[t2+1]

    limm $t2    ,$zero ,0		  # t2 = 0
    noimm  $t2  ,$s1 ,add         # t2 += s1
    noimm  $t2  ,$s0  ,sub	      # t2 -= s0
    blez  $t2   ,$zero ,NOREPLACE # if(s1 <= s0) jump to NOREPLACE

    limm $t2, $zero, 1			 # t2 = 1
    noimm $t2 , $a0 , sll		 # t2 = 1 << 8 = 256 = 0x100
	noimm $t2,   ,$t1,  add      # t2 += t1
    sw   $s0   ,$t2 ,0	         # MEM[t2+256] = s0
    sw   $s1   ,$t2 ,1	         # MEM[t2+1+256] = s1

NOREPLACE:
    bgtz  $t1   ,$zero  ,LOOP     # if(t1 > 0) jump to Loop

	limm $t1   ,$zero ,15		 # t1 = 15
    noimm  $t0 ,$t3   ,sub       # t0 -= 1
    bgtz  $t0  ,$zero ,LOOP      # if(t0>0) jump to loop
    halt $zero ,$zero , 0        # exit

# input array
    .word 0x100 100
    .word 0x101 99
    .word 0x102 98
    .word 0x103 97
    .word 0x104 96
    .word 0x105 95
    .word 0x106 94
    .word 0x107 93
    .word 0x108 92
    .word 0x109 91
    .word 0x10A 90
    .word 0x10B 89
    .word 0x10C 88
    .word 0x10D 87
    .word 0x10E 86
    .word 0x10F 85
