{------------------------------------------------
A Haskell solution to: http://www.coinheist.com/rubik/a_regular_crossword/grid.pdf
using the Z3 SMT solver.

Z3 finds the following solution in about 7 mins
on my Mac Core-i5, and another 7 mins to prove
it's unique.

   NHPEHAS
   DIOMOMTH
  FOXNXAXPH
  MMOMMMMRHH
 MCXNMMCRXEM
 CMCCCCMMMMMM
HRXRCMIIIHXLS
 OREOREOREORE
 VCXCCHHMXCC
  RRRRHHHRRU
  NCXDXEXLE
   RRDDMMMM
   GCCHHCC

NB. This program uses the regex-genex package, make
sure you get the just released 0.6.0 version.
------------------------------------------------}
{-# LANGUAGE ImplicitParams #-}
import Regex.Genex
import Data.SBV
import Data.Char

mkChars :: Int -> Symbolic [SWord8]
mkChars n = do ns <- mkExistVars n
               let a, z :: SWord8
                   a = literal (fromIntegral (ord 'A'))
                   z = literal (fromIntegral (ord 'Z'))
                   c i = constrain $ i .>= a &&& i .<= z
               mapM_ c ns
               return ns

puzzle :: Symbolic SBool
puzzle = do let ?maxRepeat = 13
            let regexMN n = let ?maxRepeat = n in regexMatch
            a@[a0, a1, a2, a3, a4, a5, a6]                            <- mkChars 7
            b@[b0, b1, b2, b3, b4, b5, b6, b7]                        <- mkChars 8
            c@[c0, c1, c2, c3, c4, c5, c6, c7, c8]                    <- mkChars 9
            d@[d0, d1, d2, d3, d4, d5, d6, d7, d8, d9]                <- mkChars 10
            e@[e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10]           <- mkChars 11
            f@[f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11]      <- mkChars 12
            g@[g0, g1, g2, g3, g4, g5, g6, g7, g8, g9, g10, g11, g12] <- mkChars 13
            h@[h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11]      <- mkChars 12
            i@[i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10]           <- mkChars 11
            j@[j0, j1, j2, j3, j4, j5, j6, j7, j8, j9]                <- mkChars 10
            k@[k0, k1, k2, k3, k4, k5, k6, k7, k8]                    <- mkChars 9
            l@[l0, l1, l2, l3, l4, l5, l6, l7]                        <- mkChars 8
            m@[m0, m1, m2, m3, m4, m5, m6]                            <- mkChars 7
            constrain =<< regexMatch [".*H.*H.*"]                     a
            constrain =<< regexMatch ["(DI|NS|TH|OM)*"]               b
            constrain =<< regexMatch ["F.*[AO].*[AO].*"]              c
            constrain =<< regexMatch ["(O|RHH|MM)*"]                  d
            constrain =<< regexMatch [".*"]                           e
            constrain =<< regexMatch ["C*MC(CCC|MM)*"]                f
            -------------------------------------------------------------------------------------------------------------------
            constrain =<< regexMatch ["[^C]*[^R]*III.*"]              g
            -------------------------------------------------------------------------------------------------------------------
            constrain =<< regexMN 5  ["(...?)\\1*"]                h
            constrain =<< regexMatch ["([^X]|XCC)*"]                  i
            constrain =<< regexMatch ["(RR|HHH)*.?"]                  j
            constrain =<< regexMatch ["N.*X.X.X.*E"]                  k
            constrain =<< regexMatch ["R*D*M*"]                       l
            constrain =<< regexMatch [".(C|HH)*"]                     m
            -------------------------------------------------------------------------------------------------------------------
            constrain =<< regexMatch [".*G.*V.*H.*"]                  [m0, l0, k0, j0, i0,  h0,  g0]
            constrain =<< regexMatch ["[CR]*"]                        [m1, l1, k1, j1, i1,  h1,  g1,  f0]
            constrain =<< regexMatch [".*XEXM*"]                      [m2, l2, k2, j2, i2,  h2,  g2,  f1,  e0]
            constrain =<< regexMatch [".*DD.*CCM.*"]                  [m3, l3, k3, j3, i3,  h3,  g3,  f2,  e1,  d0]
            constrain =<< regexMatch [".*XHCR.*X.*"]                  [m4, l4, k4, j4, i4,  h4,  g4,  f3,  e2,  d1, c0]
            constrain =<< regexMN 4  [".*(.)(.)(.)(.)\\4\\3\\2\\1.*"] [m5, l5, k5, j5, i5,  h5,  g5,  f4,  e3,  d2, c1, b0]
            -------------------------------------------------------------------------------------------------------------------
            constrain =<< regexMatch [".*(IN|SE|HI)"]                 [m6, l6, k6, j6, i6,  h6,  g6,  f5,  e4,  d3, c2, b1, a0]
            -------------------------------------------------------------------------------------------------------------------
            constrain =<< regexMatch ["[^C]*MMM[^C]*"]                [    l7, k7, j7, i7,  h7,  g7,  f6,  e5,  d4, c3, b2, a1]
            constrain =<< regexMN 6  [".*(.)C\\1X\\1.*"]              [        k8, j8, i8,  h8,  g8,  f7,  e6,  d5, c4, b3, a2]
            constrain =<< regexMatch ["[CEIMU]*OH[AEMOR]*"]           [            j9, i9,  h9,  g9,  f8,  e7,  d6, c5, b4, a3]
            constrain =<< regexMatch ["(RX|[^R])*"]                   [                i10, h10, g10, f9,  e8,  d7, c6, b5, a4]
            constrain =<< regexMatch ["[^M]*M[^M]*"]                  [                     h11, g11, f10, e9,  d8, c7, b6, a5]
            constrain =<< regexMatch ["(S|MM|HHH)*"]                  [                          g12, f11, e10, d9, c8, b7, a6]
            -------------------------------------------------------------------------------------------------------------------
            constrain =<< regexMatch [".*SE.*UE.*"]                   [                          g12, h11, i10, j9, k8, l7, m6]
            constrain =<< regexMatch [".*LR.*RL.*"]                   [                     f11, g11, h10, i9,  j8, k7, l6, m5]
            constrain =<< regexMatch [".*OXR.*"]                      [                e10, f10, g10, h9,  i8,  j7, k6, l5, m4]
            constrain =<< regexMatch ["([^EMC]|EM)*"]                 [            d9, e9,  f9,  g9,  h8,  i7,  j6, k5, l4, m3]
            constrain =<< regexMatch ["(HHX|[^HX])*"]                 [        c8, d8, e8,  f8,  g8,  h7,  i6,  j5, k4, l3, m2]
            constrain =<< regexMatch [".*PRR.*DDC.*"]                 [    b7, c7, d7, e7,  f7,  g7,  h6,  i5,  j4, k3, l2, m1]
            -------------------------------------------------------------------------------------------------------------------
            constrain =<< regexMatch [".*"]                           [a6, b6, c6, d6, e6,  f6,  g6,  h5,  i4,  j3, k2, l1, m0]
            -------------------------------------------------------------------------------------------------------------------
            constrain =<< regexMatch ["[AM]*CM(RC)*R?"]               [a5, b5, c5, d5, e5,  f5,  g5,  h4,  i3,  j2, k1, l0]
            constrain =<< regexMatch ["([^MC]|MM|CC)*"]               [a4, b4, c4, d4, e4,  f4,  g4,  h3,  i2,  j1, k0]
            constrain =<< regexMatch ["(E|CR|MN)*"]                   [a3, b3, c3, d3, e3,  f3,  g3,  h2,  i1,  j0]
            constrain =<< regexMN 4  ["P+(..)\\1.*"]                  [a2, b2, c2, d2, e2,  f2,  g2,  h1,  i0]
            constrain =<< regexMatch ["[CHMNOR]*I[CHMNOR]*"]          [a1, b1, c1, d1, e1,  f1,  g1,  h0]
            constrain =<< regexMatch ["(ND|ET|IN)[^X]*"]              [a0, b0, c0, d0, e0,  f0,  g0]
            return true

main :: IO ()
main = extractModel `fmap` satWith z3{timing=True} puzzle >>= display
  where points = [7,8,9,10,11,12,13,12,11,10,9,8,7]
        expected = sum points
        mask c | isAscii c = c
               | True      = '_'
        extract n xs = let (f, r) = splitAt n xs in (map (mask . chr . fromIntegral) f, r)
        center s = replicate lp ' ' ++ s ++ replicate rp ' '
         where e  = 13 - length s
               rp = e `div` 2
               lp = e - rp
        go []     [] = return ()
        go (p:ps) xs
         | p == length f = putStrLn (center f) >> go ps r
         | True          = error "Mismatched lengths"
         where (f, r) = extract p xs
        display :: Maybe ([Word8], [Word64]) -> IO ()
        display Nothing = putStrLn "No solution."
        display (Just (xs, _))
          | length xs /= expected
          = error $ "Expected " ++ show expected ++ " chars, got: " ++ show (length xs)
          | True
          = go points xs