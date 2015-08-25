import LP11

frf c [] = [Single c]
frf c acc@((Single a):xs)
    | c == a = (Multiple 2 a):xs
    | otherwise = (Single c):acc
frf c acc@((Multiple n a):xs)
    | c == a = (Multiple (n+1) a):xs
    | otherwise = (Single c):acc

encodeDirect = foldr frf []

main = do
    print $ encodeDirect "aaaabccaadeeee"
