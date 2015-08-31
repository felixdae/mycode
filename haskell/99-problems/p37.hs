phi m = phiList $ prime_factors m
    where
    phiList [] = 1
    phiList ((p,m):xs) == (p-1)*(p^(m-1)) * (phiList xs)

