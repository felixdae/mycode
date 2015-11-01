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

main = do
    -- lr <- allSatWith z3{timing=True} gg
    lr <- allSat gg
    printRes lr
        where
        printRes (AllSatResult (br, ls)) = do
            -- print br
            mapM_ (display . extractModel) ls
-- main = extractModel `fmap` allSatWith z3{timing=True} gg >>= display
--     where
            where
            display :: Maybe (Word16, Word16, Word16) -> IO ()
            display Nothing = putStrLn "No solution."
            display (Just x) = print x
 
