__author__ = 'Mojtaba Zaheri'

import time
import random
import gc

from simecklib import simeck32_64, ROL, ROR
from z3 import *

leq_five_min = True
loops = 50
is_cube_added =  1

def lrot16(x,r):
    return (x << r) | LShR(x,16-r)

slvr = Solver()

# smt variables
cnst = [ BitVec("c_%s" % i,16) for i in range(32) ]
key = [ BitVec("k_%s" % i,16) for i in range(32) ]
left = [ [ BitVec("l_%s_%s" % (i, j),16) for j in range(33) ] for i in range(2**10) ]
right = [ [ BitVec("r_%s_%s" % (i, j),16) for j in range(33) ] for i in range(2**10) ]

slvr.add(cnst[0] == BitVecVal(0xfffd,16))
slvr.add(cnst[1] == BitVecVal(0xfffd,16))
slvr.add(cnst[2] == BitVecVal(0xfffd,16))
slvr.add(cnst[3] == BitVecVal(0xfffd,16))
slvr.add(cnst[4] == BitVecVal(0xfffd,16))
slvr.add(cnst[5] == BitVecVal(0xfffc,16))
slvr.add(cnst[6] == BitVecVal(0xfffc,16))
slvr.add(cnst[7] == BitVecVal(0xfffc,16))
slvr.add(cnst[8] == BitVecVal(0xfffd,16))
slvr.add(cnst[9] == BitVecVal(0xfffd,16))
slvr.add(cnst[10] == BitVecVal(0xfffc,16))
slvr.add(cnst[11] == BitVecVal(0xfffd,16))
slvr.add(cnst[12] == BitVecVal(0xfffd,16))
slvr.add(cnst[13] == BitVecVal(0xfffd,16))
slvr.add(cnst[14] == BitVecVal(0xfffc,16))
slvr.add(cnst[15] == BitVecVal(0xfffd,16))
slvr.add(cnst[16] == BitVecVal(0xfffc,16))
slvr.add(cnst[17] == BitVecVal(0xfffd,16))
slvr.add(cnst[18] == BitVecVal(0xfffc,16))
slvr.add(cnst[19] == BitVecVal(0xfffc,16))
slvr.add(cnst[20] == BitVecVal(0xfffc,16))
slvr.add(cnst[21] == BitVecVal(0xfffc,16))
slvr.add(cnst[22] == BitVecVal(0xfffd,16))
slvr.add(cnst[23] == BitVecVal(0xfffc,16))
slvr.add(cnst[24] == BitVecVal(0xfffc,16))
slvr.add(cnst[25] == BitVecVal(0xfffd,16))
slvr.add(cnst[26] == BitVecVal(0xfffc,16))
slvr.add(cnst[27] == BitVecVal(0xfffd,16))
slvr.add(cnst[28] == BitVecVal(0xfffd,16))
slvr.add(cnst[29] == BitVecVal(0xfffc,16))
slvr.add(cnst[30] == BitVecVal(0xfffc,16))
slvr.add(cnst[31] == BitVecVal(0xfffd,16))

# cubes + characteristics (zero constants)

cube = [
    [19, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [6, 23, 0, 0, 0, 0, 0, 0, 0, 0],
    [4, 19, 21, 0, 0, 0, 0, 0, 0, 0],
    [3, 5, 20, 22, 0, 0, 0, 0, 0, 0],
    [11, 13, 15, 16, 30, 0, 0, 0, 0, 0],
    [3, 5, 18, 20, 22, 26, 0, 0, 0, 0],
    [1, 3, 5, 18, 20, 22, 27, 0, 0, 0],
    [2, 3, 5, 6, 19, 20, 22, 23, 0, 0],
    [3, 4, 5, 6, 7, 21, 22, 23, 24, 0],
    [3, 4, 5, 7, 11, 18, 21, 22, 24, 28],
]
Matrix = [
    [#size 1
        [ 0xfff7, 0xfee7, 0xdcc7, 0x9883, 0x1003, 0x0002, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000  ], #left
        [ 0xffff, 0xfff7, 0xfee7, 0xdcc7, 0x9883, 0x1003, 0x0002, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000  ], #right        
        ],
    [#size 2
        [ 0xffff, 0xffff, 0xffff, 0xefbf, 0xc73d, 0x8238, 0x0010, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000  ], #left
        [ 0xffff, 0xffff, 0xffff, 0xffff, 0xefbf, 0xc73d, 0x8238, 0x0010, 0x0000, 0x0000, 0x0000, 0x0000  ], #right        
        ],
    [#size 3
        [ 0xffff, 0xffff, 0xffff, 0xffff, 0xf9ff, 0x30ef, 0x0046, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000  ], #left
        [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf9ff, 0x30ef, 0x0046, 0x0000, 0x0000, 0x0000, 0x0000  ], #right        
        ],
    [#size 4
     [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf1ff, 0x00ce, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000  ], #left
        [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf1ff, 0x00ce, 0x0000, 0x0000, 0x0000, 0x0000  ], #right        
        ],
    [#size 5
     [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x7803, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000  ], #left
        [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x7803, 0x0000, 0x0000, 0x0000, 0x0000  ], #right        
        ],
    [#size 6
     [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x01ff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000  ], #left
        [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x01ff, 0x0000, 0x0000, 0x0000, 0x0000  ], #right        
        ],
    [#size 7
     [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc1ff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000  ], #left
     [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc1ff, 0x0000, 0x0000, 0x0000, 0x0000  ], #right        
        ],    
    [#size 8
     [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x8fff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000  ], #left
        [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x8fff, 0x0000, 0x0000, 0x0000, 0x0000  ], #right        
        ],
    [#size 9
     [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0300, 0x0000, 0x0000, 0x0000, 0x0000  ], #left
        [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0300, 0x0000, 0x0000, 0x0000  ], #right        
        ],
    [#size 10
     [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0300, 0x0000, 0x0000, 0x0000, 0x0000  ], #left
        [ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0300, 0x0000, 0x0000, 0x0000  ], #right        
        ],
]

rcharac = [7, 8, 8, 8, 8, 8, 8, 8, 9, 9]

#add the simeck cipher constraints

for nr in range(12,13):
    for cs in range(1,11):
        avg_time = 0
        cnt = 0
        time.sleep(10)
        for l in range(loops):# attack
    
            slvr.push()  
    
            k_tmp = [0]*32
    
            #generate the test key
            k_tmp[0] = random.randint(0,2**16-1)
            k_tmp[1] = random.randint(0,2**16-1)
            k_tmp[2] = random.randint(0,2**16-1)
            k_tmp[3] = random.randint(0,2**16-1)
    
            #generate the cube plaintexts
            i = Int('i')
            i = 0
            while i < 2**cs:
                a = 0x0000
                b = 0x0000
                c = 0x0000
                d = 0x0000
        
                s = Int('s')
                s = 0
                while s < cs:
                    if (cube[cs-1][s] < 16):
                        a = a ^ ((ROR(i,s) & 0x0001) << cube[cs-1][s])
                        s = s+1
                    else:
                        b = b ^ ((ROR(i,s) & 0x0001) << (cube[cs-1][s]-16))
                        s = s+1
                
                #add the plaintext value constraints
                slvr.add(left[i][0] == a)
                slvr.add(right[i][0] == b)
                
        
                # generate the ciphertexts
                c, d = simeck32_64(a, b, nr, *k_tmp)

                # add the ciphertext value constraints
                slvr.add(left[i][nr] == c)
                slvr.add(right[i][nr] == d)
        
                i = i+1

            # add Simeck32/64 round constraints
            for i in range(4,nr):
                slvr.add(key[i] == cnst[i-4] ^ key[i-4] ^ (key[i-3] & lrot16(key[i-3],5)) ^ lrot16(key[i-3],1))
        
            for i in range(0,2**cs):
                for r in range(1,nr+1):
                    slvr.add(left[i][r] == (left[i][r-1] & lrot16(left[i][r-1],5)) ^ lrot16(left[i][r-1],1) ^ key[r-1] ^ right[i][r-1])
                    slvr.add(right[i][r] == left[i][r-1])
            
    
            ## the full key recovery attack
    
            # add the cube characteristics to the constraints
    
            if (is_cube_added == 1):
                for r in range(1,nr):
                    rr = 0x0000
                    if r < rcharac[cs-1]:
                        for i in range(2**cs):
                            #ll ^= left[i][r]
                            rr ^= right[i][r]
                        #slvr.add( ll & Matrix[cube_size-1][0][r-1] == 0x0000 )
                        slvr.add( rr & Matrix[cs-1][1][r-1] == 0x0000 )        

    
            # try to reveal the key

            slvr.set("timeout", 1000000) # ms
            start = time.time()
            result = slvr.check()
            end = time.time()
            slvr.pop()
            if result == sat:
                secs = (end - start)
                avg_time += secs
                cnt +=1
            
        print (nr, cs, cnt)
        if cnt != 0:
            print ('average time :', avg_time/cnt)
            ff = 'simeck-smt-based-cube-attack-1000-50-results.txt'
            file = open(ff,'a')
            file.write('rounds:\t%d\tsize:\t%d\taverage time:\t%f\t-- last loop :\t%d\n' % (nr, cs, avg_time/cnt, cnt))
            file.close()
		
        gc.collect()
