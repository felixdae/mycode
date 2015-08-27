isprime x = check 2 x

check :: Int->Int->Bool
check i x
    |i > (floor (sqrt $ fromIntegral x)) = True
    |otherwise = if mod x i == 0 then False else check (i+1) x


main = do
    print [3,4,7,12,45,65,17,122]
    print $ map isprime [3,4,7,12,45,65,17,122]
