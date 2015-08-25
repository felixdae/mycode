module LP23
(clapse)
where

import System.Random

clapse rg m n = f (randomR (1,n) rg) m
    where
    f (ch, nrg) m
        | ch > m = (False, nrg)
        | otherwise = (True, nrg)
