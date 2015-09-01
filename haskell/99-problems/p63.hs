import Bt

makeCbt root limit
    |root > limit = EmptyTree
    |otherwise = Node root (makeCbt (2*root) limit) (makeCbt (2*root+1) limit)
cbt n = makeCbt 1 n

allHave2Son l = foldr (\e acc -> acc && (have2Son e)) True l
    where 
    have2Son EmptyTree = False
    have2Son (Node _ EmptyTree _) = False
    have2Son (Node _ _ EmptyTree) = False
    have2Son (Node _ _ _) = True

allSons l = foldr (\(Node _ l r) acc -> (l:r:acc)) [] l

noSon EmptyTree acc = True && acc
noSon (Node _ EmptyTree EmptyTree) acc = True && acc
noSon _ acc = False

leftAdjust ((Node _ EmptyTree EmptyTree):xs) = foldr noSon True xs
leftAdjust ((Node _ EmptyTree _):_) = False
leftAdjust ((Node _ _ EmptyTree):xs) = foldr noSon True xs
leftAdjust ((Node _ _ _):xs) = leftAdjust xs

testCbt l
    |allHave2Son l = testCbt (allSons l)
    |otherwise = (leftAdjust l) && (foldr noSon True (allSons l))

main = do
    print $ cbt 4
    print $ cbt 7
    print $ testCbt [cbt 7]
