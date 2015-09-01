import Bt

hbalTree c 0 = [EmptyTree]
hbalTree c 1 = [(Node c EmptyTree EmptyTree)]
hbalTree c h = hExpTree c (hbalTree c (h-1)) (hbalTree c (h-1)) 
            ++ hExpTree c (hbalTree c (h-2)) (hbalTree c (h-1)) 
            ++ hExpTree c (hbalTree c (h-1)) (hbalTree c (h-2)) 


hExpTree c l r = [Node c xl xr | xl<-l, xr<-r]

main = do
    print $ hbalTree 'x' 3
