import Qs

cmp x y
    | (length x <= length y) = True
    | otherwise = False

flf [] e = [[e]]
flf (x@(y:ys):xs) e 
    | (length y) == (length e) = (e:x):xs
    | otherwise = [e]:x:xs

-- groupByLength :: [a] -> [[a]]
groupByLength x = foldl flf [] x

joinList [] = []
joinList (x:xs) = x ++ joinList xs

main = do
    print $ joinList . (quickSort cmp) . groupByLength . quickSort cmp $ ["abc", "de", "fgh", "de", "ijkl", "mn", "o"]
    -- print  (groupByLength . quickSort cmp $ ["abc", "de", "fgh", "de", "ijkl", "mn", "o"])
