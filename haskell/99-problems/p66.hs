import Bt

type Pos = (Int, Int)
layout :: Btree a -> Btree (a, Pos)
layout t = t'
  where (l, t', r) = layoutAux x1 1 t
        x1 = maximum l + 1
 
        layoutAux :: Int -> Int -> Btree a -> ([Int], Btree (a, Pos), [Int])
        layoutAux x y EmptyTree = ([], EmptyTree, [])
        layoutAux x y (Node a l r) = (ll', Node (a, (x,y)) l' r', rr')
          where (ll, l', lr) = layoutAux (x-sep) (y+1) l
                (rl, r', rr) = layoutAux (x+sep) (y+1) r
                sep = maximum (0:zipWith (+) lr rl) `div` 2 + 1
                ll' = 0 : overlay (map (+sep) ll) (map (subtract sep) rl)
                rr' = 0 : overlay (map (+sep) rr) (map (subtract sep) lr)
 
-- overlay xs ys = xs padded out to at least the length of ys
-- using any extra elements of ys
overlay :: [a] -> [a] -> [a]
overlay [] ys = ys
overlay xs [] = xs
overlay (x:xs) (y:ys) = x : overlay xs ys

main = do
    print $ layout (Node 'n'
            (Node 'k'
             (Node 'c'
              (Node 'a' EmptyTree EmptyTree)
              (Node 'e'
               (Node 'd' EmptyTree EmptyTree)
               (Node 'g' EmptyTree EmptyTree)
              )
             )
             (Node 'm' EmptyTree EmptyTree)
            )
            (Node 'u'
             (Node 'p'
              EmptyTree
              (Node 'q' EmptyTree EmptyTree)
             )
             EmptyTree
            ))
