data Comp = Multiple n c|Single c

listToComp l
    |(length l) == 1 = Single (head l)
    |otherwise = Multiple (length l) (head l)

encodeModified x = map listToComp $ pack x

main = do
    print $ encodeModified "aaaabccaadeeee"
