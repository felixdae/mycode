import Bt
import LP17

preOrder EmptyTree = ""
preOrder (Node x EmptyTree EmptyTree) = [x]
preOrder (Node x l r) = [x] ++ (preOrder l) ++ (preOrder r)

inOrder EmptyTree = ""
inOrder (Node x EmptyTree EmptyTree) = [x]
inOrder (Node x l r) = (inOrder l) ++ [x] ++ (inOrder r)

reConTree :: [Char]->[Char]->Btree Char
reConTree [] _ = EmptyTree
reConTree (x:xs) i = Node x (reConTree lp li) (reConTree rp ri)
    where
    lp = fst (mySplit xs (ix-1))
    rp = snd (mySplit xs (ix-1))
    li = init (fst (mySplit i ix))
    ri = snd (mySplit i ix)
    ix = findXini x i 0
    findXini ne [] _ = error "error"
    findXini ne (st:sts) ct
        |ne == st = ct+1
        |otherwise = findXini ne sts (ct+1)

main = do
    let bt = (Node 'n'
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
    print $ preOrder bt
    print $ inOrder bt

    print $ reConTree (preOrder bt) (inOrder bt)
