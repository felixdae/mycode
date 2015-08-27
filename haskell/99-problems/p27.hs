-- myGroup [] _ [[]]
-- myGroup nl l
import PnC

multinomi [] _ = [[]]
multinomi (x:xs) l = ff (combs2 x l)
    where
    ff [] = []
    ff ((h,t):cs) = map (\kk -> (h:kk))  (multinomi xs t) ++ ff cs

main = do
    let res = multinomi [2,3,4] ["aldo","beat","carla","david","evi","flip","gary","hugo","ida"]
    print $ length res
    let res = multinomi [2,2,5] ["aldo","beat","carla","david","evi","flip","gary","hugo","ida"]
    print $ length res
