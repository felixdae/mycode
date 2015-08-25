import P9

encode x = map (\g -> (length g, head g)) $ pack x

main = do
    print $ encode "aaaabccaadeeee"
