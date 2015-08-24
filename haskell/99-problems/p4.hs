myLength [] = 0
myLength (_:xs) = 1 + (myLength xs)

main = do
    print (myLength [])
    print (myLength [2,1,3,1,3,4,6,7])
