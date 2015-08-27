import Gcd

coprime x y = myGcd x y == 1

main = do
    print $ coprime 35 64
    print $ coprime 35 63
    
