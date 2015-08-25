import LP5

compress [] = []
compress (x:xs) = myReverse $ myCompress xs [x]

myCompress [] r = r
myCompress (x:xs) r@(y:rs)
    |x == y = myCompress xs r
    |otherwise = myCompress xs (x:r)

main = do
    print (compress "aaaabccaadeeee")
