import Lr

gray n = enumerateN n "01"

main = do
    print $ gray 3
    print $ gray 6
