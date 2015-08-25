import LP9
import LP11

listToComp l
    |(length l) == 1 = Single (head l)
    |otherwise = Multiple (length l) (head l)

encodeModified x = map listToComp $ pack x

main = do
    print $ encodeModified "aaaabccaadeeee"
