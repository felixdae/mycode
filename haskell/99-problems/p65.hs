import Bt

leftAndDeep EmptyTree = (0,0)
-- leftAndDeep (Node x EmptyTree r) = (1, (1+snd.leftAndDeep r))
leftAndDeep (Node x l r) = (1+(fst (leftAndDeep l)), (1+(max (snd (leftAndDeep l)) (snd (leftAndDeep r)))))

layout :: Btree a -> Btree (a,(Integer, Integer))
layout root = makeLayout root depth depthLm
    where
    depth = snd (leftAndDeep root)
    depthLm = fst (leftAndDeep root)
    makeLayout EmptyTree 0 _ = EmptyTree
    makeLayout (Node x EmptyTree EmptyTree) 1 _ = Node (x, (1,1)) EmptyTree EmptyTree
    makeLayout root depth depthLm = makeLayoutSX root xr span 1
    span = 2^(depth-2)
    xr = 2^(depth-1) - 2^(depth-depthLm) + 1
    makeLayoutSX EmptyTree _ _ _ = EmptyTree
    makeLayoutSX (Node x l r) xr span d = Node (x, (xr,d)) (makeLayoutSX l (xr-span) (div span 2) (d+1)) (makeLayoutSX r (xr+span) (div span 2) (d+1))

main = do
    print $ layout (Node 'n'
            (Node 'k'
             (Node 'c'
              (Node 'a' EmptyTree EmptyTree)
              (Node 'h'
               (Node 'g'
                (Node 'e' EmptyTree EmptyTree)
                EmptyTree
               )
               EmptyTree
              )
             )
             (Node 'm' EmptyTree EmptyTree)
            )
            (Node 'u'
             (Node 'p'
              EmptyTree
              (Node 's'
               (Node 'q' EmptyTree EmptyTree)
               EmptyTree
              )
             )
             EmptyTree
            )
            )

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
