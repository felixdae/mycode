import Data.List

n2a 1 = "one"
n2a 2 = "two"
n2a 3 = "three"
n2a 4 = "four"
n2a 5 = "five"
n2a 6 = "six"
n2a 7 = "seven"
n2a 8 = "eight"
n2a 9 = "nine"
n2a 0 = "zero"

dissect n 
    |n<10 = [n]
    |otherwise = dissect (div n 10) ++ [mod n 10]
fullWords n = intercalate "-" (map n2a (dissect n))

main = do
    print $ fullWords 175
    print $ fullWords 1789750
    print $ fullWords 1
