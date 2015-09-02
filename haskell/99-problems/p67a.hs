import Bt

treeToString EmptyTree = ""
treeToString (Node x EmptyTree EmptyTree) = [x]
treeToString (Node x l r) = [x] ++ "(" ++ (treeToString l) ++ "," ++ (treeToString r) ++ ")"

linkNodes (r:l:(Node x _ _):rs) = ((Node x l r):rs)
-- stringToTree strRep stack
stringToTree :: [Char]->[Btree Char]->Btree Char
stringToTree [] stk = head stk
stringToTree (x:xs@(y:ys)) stk
    |x >= 'a' && x <= 'z' = stringToTree xs ((Node x EmptyTree EmptyTree):stk)
    |x == '(' = if y /=',' then stringToTree xs stk else stringToTree ys (EmptyTree:stk)
    |x == ',' = if y /=')' then stringToTree xs stk else stringToTree xs (EmptyTree:stk)
    |x == ')' = stringToTree xs (linkNodes stk)
stringToTree [x] stk
    |x /= ')' = error "error"
    |otherwise = stringToTree [] (linkNodes stk)

main = do
     print $ (stringToTree (treeToString (Node 'n'
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
             ))) [])
     print $ (treeToString (Node 'n'
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
             )))
