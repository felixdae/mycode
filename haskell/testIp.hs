{-# LANGUAGE ImplicitParams #-}
hh::(?jj :: Int) => Int->Int
hh x = ?jj + x

main = do
    print $ hh 12
        where ?jj = 33
