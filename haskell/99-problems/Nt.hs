module Nt
(myGcd
,isprime
,totient
,primesTME
,primeR
,prime_factors
,goldbach
,coprime)
where

-- import Debug.Trace (trace)
-- goldbach::Integer->(Integer,Integer)
goldbach n = ssr (reverse (primeR 2 (n `div` 2)))
    where
    --ssr::[Integer]->(Integer, Integer)
    ssr [] = error "goldbach is broken"
    ssr (x:xs)
        |isprime (n-x) = (x, n-x)
        |otherwise = {-trace (show x)-} ssr xs

-- import Data.List (union)
isprime x = checkPrime 2 x

checkPrime :: Integer->Integer->Bool
checkPrime i x
    |i > (floor (sqrt $ fromIntegral x)) = True
    |otherwise = if mod x i == 0 then False else checkPrime (i+1) x


coprime x y = myGcd x y == 1

myGcd x y
    |x < 0 = myGcd (abs x) y
    |y < 0 = myGcd x (abs y)
    |y < x = myGcd y x
    |x == 0 = y
    -- |otherwise = trace (show x ++ ":" ++ show y) myGcd (mod y x) x
    |otherwise = myGcd (mod y x) x

totient x = filter (coprime x) [1..x-1]

-- {-# OPTIONS_GHC -O2 -fno-cse #-}
primesTME = 2 : gaps 3 (join [[p*p,p*p+2*p..] | p <- primes'])
    where
    primes' = 3 : gaps 5 (join [[p*p,p*p+2*p..] | p <- primes'])
    join  ((x:xs):t)        = x : union xs (join (pairs t))
    pairs ((x:xs):ys:t)     = (x : union xs ys) : pairs t
    gaps k xs@(x:t) 
        | k==x  = gaps (k+2) t 
        | True  = k : gaps (k+2) xs

union (x:xs) (y:ys) = case (compare x y) of 
    LT -> x : union  xs  (y:ys)
    EQ -> x : union  xs     ys 
    GT -> y : union (x:xs)  ys

primeR x y = fp x y primesTME []
-- fp :: Int->Int->[Int]->[Int]->[Int]
fp x y (a:rs) acc
    |a < x = fp x y rs acc
    |a <= y = fp x y rs (a:acc)
    |otherwise = acc

nap n f = naap n f 0
    where
    naap n f c
        | n `mod` f == 0 = naap (n `div` f) f (c+1)
        | otherwise = (f, c)

prime_factors n = filter (\(f, c) -> c > 0) (map (nap n) $ primeR 2 (n `div` 2))

