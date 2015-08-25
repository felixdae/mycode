import System.Random
import LP23

choose :: (RandomGen aa)=>aa->Int->Int->Int->[Int]
choose _ _ _ 0 = []
choose rg st ed m
    |m > n = []
    |(tr $ clapse rg m n) == True = st:(choose (gr $ clapse rg m n) (st+1) ed (m-1))
    |otherwise = choose (gr $ clapse rg m n) (st+1) ed m
        where 
        tr (res, gen) = res
        gr (res, gen) = gen
        n = (ed - st + 1)

main = do
    gen <- getStdGen
    print $ choose gen 6 49 3
