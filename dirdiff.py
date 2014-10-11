import commands
import subprocess
import sys

def diffdir(dir1, dir2):
    cmd = 'diff -qr ' + dir1 + ' ' + dir2
    (status, output) = commands.getstatusoutput(cmd)
#    if status:    ## Error case, print the command's output to stderr and exit
#        print ",,,,,,,,,,,,,",status
#    sys.stderr.write(output)
#        sys.exit(1)
    return output
    #print output  ## Otherwise do something with the command's output

def difffile(file1, file2):
    #cmd = 'vimdiff ' + file1 + ' ' + file2
    #(status, output) = commands.getstatusoutput(cmd)
    #return output
    subprocess.call(['vimdiff', file1, file2])

def main():
    if len(sys.argv) < 3:
        print "usage %s dir1 dir2" %(sys.argv[0])
        sys.exit(1)
    dir1 = sys.argv[1]
    dir2 = sys.argv[2]
    diff_res_lines = diffdir(dir1, dir2).split("\n")
    hidden = '.DS_Store'
    start = 'Files '
    end = ' differ'

    identical = []
    for diff in diff_res_lines:
        diff = diff.strip()
        if diff[len(diff) - len(hidden):] == hidden:
            continue
        if diff.find('Only in ') == 0:
            #print diff
            identical.append(diff)
            continue
        if diff[0:len(start)] == start and diff[len(diff) - len(end):] == end:
            #print diff
            diff = diff[len(start):-len(end)]
            #print diff
            fa = diff.split(' and ')
            #print fa
            difffile(fa[0], fa[1])
    for il in identical:
        print il

if __name__ == '__main__':
    main()
