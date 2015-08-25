-- drop l n
--     |(lenght l < n) = l
--     |otherwise = take (n-1) l ++ (drop l n-

-- myDrop l r c n 
myDrop [] r _ _ = r
myDrop (x:xs) r 0 n = myDrop xs r (n-1) n
myDrop (x:xs) r c n = myDrop xs (r ++ [x]) (c-1) n

dropEvery l n = myDrop l [] (n-1) n

main = do
    print $ dropEvery "abcdefghik" 3
