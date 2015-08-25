import System.Random
import LP23
-- 编程珠玑上的算法，要从 n 个元素中取 m 个，以 m/n 为概率来取

choose _ _ 0 = []
choose rg l@(x:xs) m
    |m > (length l) = []
    |(tr $ clapse rg m (length l)) == True = x:(choose (gr $ clapse rg m (length l)) xs (m-1))
    |otherwise = choose (gr $ clapse rg m (length l)) xs m
        where 
        tr (res, gen) = res
        gr (res, gen) = gen

main = do
    gen <- getStdGen
    print $ choose gen "abcdefgh" 3
