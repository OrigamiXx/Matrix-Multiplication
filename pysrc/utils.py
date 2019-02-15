import math
import sys

def max_s(k):
    '''Returns the maximum number of rows a strong USP may have based on
    Lemma 3.2 of [CKSU05] and strong USP capacity <= USP capacity.'''
    
    return int(math.floor((3/(2**(2/3)))**k))


def min_omega(s,k):

    omega = 3
    for m in range(3,1000):
        curr = 3 * math.log(m) / math.log(m - 1) - 3 * log_fact(s) / (s * k * math.log(m - 1))
        omega = min(omega, curr)
    
    return omega

def log_fact(s):

    res = 0
    if s < 1000:
        for i in range(1, s + 1):
            res += math.log(i)
    else:
        res = s * math.log(s) - s + math.log(s * (1 + 4 * s * (1 + 2 * s))) / 6 + math.log(math.pi)/2
        
    return res

def min_tensor_omega(s,k):

    sp = s
    kp = k
    omega = 3
    while sp < 1e300:
        curr = min_omega(sp, kp)
        omega = min(omega, curr)
        sp *= s
        kp += k
        
    return omega
        


if __name__ == "__main__":
    s = int(sys.argv[1])
    k = int(sys.argv[2])
    print("s = ", s, "k =", k, "omega <=", min_omega(s,k), min_tensor_omega(s,k))
    print(min_tensor_omega(2**(k-1)*(2**k + 1), 3*k))
