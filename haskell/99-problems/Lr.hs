module Lr
(and' 
,or'  
,nand'
,nor' 
,xor' 
,impl'
,equ'
,enumerateN)
where

and'  a b = a && b
or'   a b = a || b
nand' a b = not (and' a b)
nor'  a b = not (or' a b)
xor'  a b = not (equ' a b)
impl' a b = or' (not a) b
equ'  a b = a == b

enumerateN ::Integer->[a]->[[a]]
enumerateN 0 _ = [[]]
enumerateN n domain = 
    let ende = (enumerateN (n-1) domain) 
    in joinN domain ende
        where joinN x y = [ (xx:yy) | xx<-x, yy<-y]

infixl 4 `or'`
infixl 6 `and'`
