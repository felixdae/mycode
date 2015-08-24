myLast :: [a] -> a
myLast [] = error "empty list"
myLast (x:[]) = x
myLast (x:xs) = myLast xs

-- main = print (myLast "fdsfafsfsf")
main = print (myLast [1,2,3,5,3])
