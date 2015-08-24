myButLast :: [a] -> a
myButLast [] = error "empty list"
myButLast [x] = error "only one elem"
myButLast [x,y] = x
myButLast (x:ys) = myButLast ys

main = do
    print (myButLast [7,1,3,5,7,2,4])
    print (myButLast (reverse [7,1,3,5,7,2,4]))
    print (myButLast [7])
