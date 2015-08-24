-- elementAt :: [a] -> Integer -> a
elementAt (x:_) 1 = x
elementAt [] _ = error "no that elem"
elementAt (_:xs) n = elementAt xs (n-1)

main = do
    print (elementAt [1,2,3] 2)
    print (elementAt "haskell" 4)
    print (elementAt [1,2,3] 4)
