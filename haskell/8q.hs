import Data.List (permutations)
-- import Debug.Trace (trace)
sameRow (ar, ac) (br, bc) = ar /= br
sameDiag (ar, ac) (br, bc) = (ar - ac) /= (br - bc)
sameRDiag (ar, ac) (br, bc) = (ar + ac) /= (br + bc)

check cmp a b = cmp a b

checkAgainst _ _ _ [] = True
-- checkAgainst i x j (y:ys) = (sameRow (x, i) (y, j))
--                             && (sameDiag (x, i) (y, j))
checkAgainst i x j (y:ys) = (sameDiag (x, i) (y, j))
                            && (sameRDiag (x, i) (y, j))
                            && (checkAgainst i x (j+1) ys)

gCheck _ [] = True
gCheck i (x:xs) = (checkAgainst i x (i+1) xs) && (gCheck (i + 1) xs)

-- eightQueen = [x | x <- [ [r1,r2,r3,r4,r5,r6,r7,r8] | r1 <- [1..8], r2 <- [1..8], r3 <- [1..8], r4 <- [1..8],
--                                         r5 <- [1..8], r6 <- [1..8], r7 <- [1..8], r8 <- [1..8]], trace (show x) gCheck 1 x]
eightQueen = [x | x <- permutations [1,2,3,4,5,6,7,8], {-trace (show x)-} gCheck 1 x]

main = do
    print $ length eightQueen
    print eightQueen
