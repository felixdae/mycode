-- import Debug.Trace (trace)
import Bt

dotRep EmptyTree = "."
-- dotRep (Node x EmptyTree EmptyTree) = [x]
dotRep (Node x l r) = [x] ++ (dotRep l) ++ (dotRep r)

unSettleNode c = Node c (Node '*' EmptyTree EmptyTree) (Node '*' EmptyTree EmptyTree)

isUnSettleNode (Node _ (Node '*' EmptyTree EmptyTree) (Node '*' EmptyTree EmptyTree)) = True
isUnSettleNode _ = False

fromDotRep [] [] = EmptyTree
fromDotRep [] [x] = x
fromDotRep [] stk@(r:l:rt@(Node c _ _):rts) = fromDotRep [] ((Node c l r):rts)
fromDotRep srep@(x:xs) stk@(r:l:rt@(Node c _ _):rts)
    |(not (isUnSettleNode r)) && (not (isUnSettleNode l)) && (isUnSettleNode rt) = {- trace ((show (x:xs)) ++ "4444" ++ (show stk)) -}fromDotRep srep ((Node c l r):rts)
    |x == '.' = {-trace ((show (x:xs)) ++ "3333" ++ (show stk)) -}fromDotRep xs (EmptyTree:stk)
    |otherwise = fromDotRep xs ((unSettleNode x):stk)
fromDotRep srep@(x:xs) stk
    |x == '.' = {-trace ((show srep) ++ "1111" ++ (show stk)) -}fromDotRep xs (EmptyTree:stk)
    |otherwise = {-trace ((show srep) ++ "2222" ++ (show stk)) -}fromDotRep xs ((unSettleNode x):stk)
-- fromDotRep x y = trace ((show x) ++ "000000" ++ (show y)) error "errror"

main = do
    let bt = Node 'a' (Node 'b' (Node 'd' EmptyTree EmptyTree) (Node 'e' EmptyTree EmptyTree)) (Node 'c' EmptyTree (Node 'f' (Node 'g' EmptyTree EmptyTree) EmptyTree))
    print $ dotRep bt
    print $ fromDotRep (dotRep bt) []
    print bt
    print $ fromDotRep "fg..." []
