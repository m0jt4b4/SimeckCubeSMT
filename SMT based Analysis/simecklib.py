__author__ = 'Mojtaba Zaheri'

import time
import random
import array
import numpy

def ROR(x, n):
    mask = (2**n) - 1
    mask_bits = x & mask
    return (x >> n) | (mask_bits << (16 - n))

def ROL(x, n):
    return ROR(x, 16 - n)

constant = [ 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffc, 0xfffc, 0xfffc, 0xfffd, 0xfffd, 0xfffc, 0xfffd, 0xfffd,
         0xfffd, 0xfffc, 0xfffd, 0xfffc, 0xfffd, 0xfffc, 0xfffc, 0xfffc, 0xfffc, 0xfffd, 0xfffc, 0xfffc, 0xfffd,
         0xfffc, 0xfffd, 0xfffd, 0xfffc, 0xfffc, 0xfffd ]

def key_schedule(rounds, *master_key):
    key = list(master_key)
    for i in range(4,rounds):
        key[i] = constant[i-4] ^ key[i-4] ^ (key[i-3] & ROL(key[i-3],5)) ^ ROL(key[i-3],1)
    return key

def encrypt(left_state, right_state, rounds, *master_key):
    key = list(master_key)
    for i in range(0,rounds):
        tmp = left_state
        left_state = (left_state & ROL(left_state,5)) ^ ROL(left_state,1) ^ key[i] ^ right_state
        right_state = tmp
    return left_state, right_state

def simeck32_64(left, right, rounds, *master_key):
    key = list(master_key)
    key = key_schedule(rounds, *key)
    return encrypt(left, right, rounds, *key)