module LP17
(mySplit)
where

mySplit [] n = ([], [])
mySplit l 0 = ([], l)
mySplit (x:xs) n = addD x (mySplit xs (n-1))
    where addD x (a,b) = (x:a, b)
