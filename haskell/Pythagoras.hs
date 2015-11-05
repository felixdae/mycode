import Data.SBV

gg :: Symbolic SBool
gg = do
    x <- exists "x"
    y <- exists "y"
    z <- exists "z"
    constrain $ x .> 0
    constrain $ y .> 0
    constrain $ z .> 0
    constrain $ x .< 100
    constrain $ y .< 100
    constrain $ z .< 100
    -- constrain $ x*x + y*y .== z*z
    -- return $ x*x*x + y*y*y .== z*z*(z::SWord16)
    return $ x*x + y*y .== z*(z::SWord16)

pp :: Symbolic SBool
pp = do
    [x,y,z,w] <- sWord32s ["x","y","z","w"]
    constrain $ x .> 0
    constrain $ y .> 0
    constrain $ z .> 0
    constrain $ w .> 0
    constrain $ x .< 5
    constrain $ y .< 5
    constrain $ z .< 5
    constrain $ w .< 5
    constrain $ x ./= y
    constrain $ x ./= z
    constrain $ x ./= w
    constrain $ y ./= z
    constrain $ y ./= w
    constrain $ z ./= w
    return true

-- send more money
vp :: Symbolic SBool
vp = do
    [s,e,n,d,m,o,r,y] <- sWord32s ["s","e","n","d","m","o","r","y"]
    constrain $ s .>= 1 &&& s .<= 9
    constrain $ m .>= 1 &&& m .<= 9
    constrain $ e .>= 0 &&& e .<= 9
    constrain $ n .>= 0 &&& n .<= 9
    constrain $ d .>= 0 &&& d .<= 9
    constrain $ o .>= 0 &&& o .<= 9
    constrain $ r .>= 0 &&& r .<= 9
    constrain $ y .>= 0 &&& y .<= 9
    constrain $ s ./= e &&& s ./= n &&& s ./= d &&& s ./= m &&& s ./= o &&& s ./= r &&& s./= y
    constrain $ e ./= n &&& e ./= d &&& e ./= m &&& e ./= o &&& e ./= r &&& e./= y
    constrain $ n ./= d &&& n ./= m &&& n ./= o &&& n ./= r &&& n./= y
    constrain $ d ./= m &&& d ./= o &&& d ./= r &&& d./= y
    constrain $ m ./= o &&& m ./= r &&& m./= y
    constrain $ o ./= r &&& o./= y
    constrain $ r./= y
    constrain $ 1000*s+100*e+10*n+d + 1000*m+100*o+10*r+e .== 10000*m+1000*o+100*n+10*e+y
    return true

main = do
    -- lr <- allSatWith z3{timing=True} gg
    -- lr <- allSat gg
    lr <- allSat pp
    -- lr <- allSat vp
    -- print lr
    case lr of
        AllSatResult (br, ls) -> do
            mapM_ (display . extractModel) ls
            where 
                display :: Maybe [Word32] -> IO ()
                display (Just x) = print x
                display Nothing = print "no solution"
                -- display :: Maybe (Word32, Word32, Word32, Word32) -> IO ()
                -- -- display :: Maybe (Word32, Word32, Word32, Word32, Word32, Word32, Word32, Word32) -> IO ()
                -- display (Just x) = print x
                -- display Nothing = print "no solution"
