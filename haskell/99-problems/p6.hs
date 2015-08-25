import LP5

isPalindrome x = (x == myReverse x)

main = do
    print (isPalindrome "madamimadam")
    print (isPalindrome [1,2,4,8,16,8,4,2,1])
