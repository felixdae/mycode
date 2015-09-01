import Bt
import Qs

cmp :: Btree (Char, Int)->Btree (Char, Int)->Bool
cmp (Node (_, f1) _ _) (Node (_, f2) _ _) = f1 < f2

-- huffman :: [(Char,Int)]->[Btree (Char,Int)]
-- huffman = makeHuffman . quickSort cmp . map makeTreeNode
huffman = makeHuffman . quickSort cmp . map makeTreeNode

-- makeHuffman [] = []
makeHuffman [x] = traverseBtree x
makeHuffman (x@(Node (_, v1) _ _):y@(Node (_, v2) _ _):ys) = makeHuffman $ quickSort cmp ((Node ('*', v1 + v2) x y):ys)

-- traverseBtree :: Btree (Char, Int)->String
-- traverseBtree EmptyTree = []
traverseBtree (Node (s,_) EmptyTree EmptyTree) = [(s,"")]
traverseBtree (Node (s,_) leftTree rightTree) = (map (\ (s,x) -> (s,'0':x)) (traverseBtree leftTree)) ++ (map (\ (s,x) -> (s,'1':x)) (traverseBtree rightTree))

main = do
    -- print $ huffman [('a',45),('b',13),('c',12),('d',16),('e',9),('f',5)]
    print $ huffman [('a',45),('b',13),('c',12),('d',16),('e',9),('f',5)]
