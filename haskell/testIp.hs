{-# LANGUAGE ImplicitParams #-}
hh::(?jj :: Int) => Int->Int
hh x = ?jj + x


gg h = hh h*8
main = do
    let ?jj = 22
    print $ gg 12
        --where ?jj = 33
