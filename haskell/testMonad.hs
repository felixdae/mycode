check :: Int -> Maybe Int
check n | n < 10    = Just n
    | otherwise = Nothing

halve :: Int -> Maybe Int
halve n | even n    = Just $ n `div` 2
    | otherwise = Nothing

ex01 = return 7 >>= check >>= halve
ex02 = return 12 >>= check >>= halve
ex03 = return 12 >>= halve >>= check

addOneOrTwo :: Int -> [Int]
addOneOrTwo x = [x+1, x+2]

ex07 = [10,20,30] >>= addOneOrTwo
ex08 = do
    num <- [10, 20, 30]
    addOneOrTwo num

main = do
    let h = 12
    print $ ex01
    print $ ex02
    print $ ex03
    print $ ex07
    print $ ex08
