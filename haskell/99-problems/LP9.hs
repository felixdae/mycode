module LP9
(pack)
where 

import LP5

-- pf :: [a]->a->[[a]]
pf [] c = [[c]]
pf (x@(y:ys):xs) c
    | y == c = ((c:x):xs)
    | otherwise = ([c]:x:xs)
pack x = myReverse $ foldl pf [] x

-- pack (x:@xs(y:ys))
--     | x == y = f1 $ pack xs
--     | otherwise = f2 $ pack
-- pack [x] = [[x]]
-- pack [] = []

--main = do
--    print $ pack ['a', 'a', 'a', 'a', 'b', 'c', 'c', 'a', 
--                 'a', 'd', 'e', 'e', 'e', 'e']
