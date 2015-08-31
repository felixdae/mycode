import Nt

goldbachList l u res
    |l `mod` 2 == 1 = goldbachList (l+1) u res
    |l > u = res
    |otherwise = goldbachList (l+2) u (res++[goldbach l])

main = do
    print $ goldbachList 9 20 []
    print $ goldbachList 3 2000 []
    print $ filter (\(x,y) -> ((x>50) && (y>50))) (goldbachList 3 2000 [])
