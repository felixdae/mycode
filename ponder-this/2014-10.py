
def inver(a,m):
#    for x in xrange(1,m):
#        if a*x%m==1:
#            return x
    k=inverse(a,m)
    if a*k%m != 1:
        print 10%0
    return k

def inverse(a,m):
    if a==4:
        return 0-(m/a)
    if a==2**10:
        return 1745224#inver(a,m)
    if a==5**10:
        return 841#inver(a,m)
    return 1 

def pr(a,b,m):
    if b==0:
        return 1
    if b%2==1:
        return pr(a,b/2,m)*pr(a,b/2,m)*a%m
    return pr(a,b/2,m)*pr(a,b/2,m)%m

def rem(base,d):#%2**10, %4
    if d%2==0:
        if base%2==0:
            return 0
        return 1
    exp=octo(base+1, 4, d/5)
    return pr(base, exp, d)

#2**3**4**5**6**7**8**9%5**10
def octo(base, d1, d2):#d2=5**x
#    print base
    if base == 8:
        ans = 8**9%(4*5**4)
        print ans
        return ans
    x=rem(base,d1)*d2*inver(d2,d1)
    y=rem(base,d2)*d1*inver(d1,d2)
    ans = (x+y)%(d1*d2)
    print ans
    return ans

#print pr(7,228,5**5)
#print inverse(5**10,2**10)
print octo(2,2**10,5**10)
