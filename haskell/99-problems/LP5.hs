module LP5
(myReverse
)where

myReverse [] = []
myReverse (x:xs) = (myReverse xs) ++ [x]

-- main = do
--     print (myReverse "")
--     print (myReverse [1,2,3,4])
--     print (myReverse "A man, a plan, a canal, panama!")
