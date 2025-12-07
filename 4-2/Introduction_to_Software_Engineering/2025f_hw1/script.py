import os, argparse 

toppath = os.path.abspath(os.getcwd())
pgm = "sqlite3"
srcpath = toppath + "/" + pgm
binpath = srcpath
def build_gcov():
    # download the benchmark 
    os.system("wget https://www.sqlite.org/2020/sqlite-amalgamation-3330000.zip")
    os.system("unzip sqlite-amalgamation-3330000.zip && rm -r sqlite-amalgamation-3330000.zip")
    os.system("cp -r sqlite-amalgamation-3330000 sqlite3 && rm -r sqlite-amalgamation-3330000")
    os.chdir(srcpath)
    os.system("gcc -g -fprofile-arcs -ftest-coverage -O0 -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_DEFAULT_MEMSTATUS=0 -DSQLITE_MAX_EXPR_DEPTH=0 -DSQLITE_OMIT_DECLTYPE -DSQLITE_OMIT_DEPRECATED -DSQLITE_DEFAULT_PAGE_SIZE=512 -DSQLITE_DEFAULT_CACHE_SIZE=10 -DSQLITE_DISABLE_INTRINSIC -DSQLITE_DISABLE_LFS -DYYSTACKDEPTH=20 -DSQLITE_OMIT_LOOKASIDE -DSQLITE_OMIT_WAL -DSQLITE_OMIT_PROGRESS_CALLBACK -DSQLITE_DEFAULT_LOOKASIDE='64,5' -DSQLITE_OMIT_PROGRESS_CALLBACK -DSQLITE_OMIT_SHARED_CACHE -I. shell.c sqlite3.c -o sqlite3")
    if os.path.exists(f"{binpath}/sqlite3"):
        print("Build Complete")
    else:
        print("Build Error")
    
def run_testcase(file_name):
    with open(file_name, 'r') as f:
        testcases=[l.split('\n')[0] for l in f.readlines()]
    
    os.system("find " + srcpath + ' -name "*.gcda" -exec rm {} \;')
    os.system("find " + srcpath + ' -name "*.gcov" -exec rm {} \;')
    
    os.chdir(binpath)
    print("----------------Run Test-Cases-------------------------------------")
    print("-------------------------------------------------------------------")
    for tc in testcases:
        os.system(tc)
    gcov_file="cov_result"
    os.system("find " + srcpath + ' -name "*.gcda" -exec gcov -bc {} 1>'+gcov_file+' 2> err \;')
    os.system("find " + srcpath + ' -name "*.gcda" -exec rm {} \;')
    os.system("find " + srcpath + ' -name "*.gcov" -exec rm {} \;')
    return cal_coverage(gcov_file)

def cal_coverage(cov_file):
    coverage=0
    total_coverage=0
    with open(cov_file, 'r') as f:
        lines= f.readlines()
        for line in lines:
            if "Taken at least" in line:
                data=line.split(':')[1]
                percent=float(data.split('% of ')[0])
                total_branches=float((data.split('% of ')[1]).strip())
                covered_branches=int(percent*total_branches/100)
                
                coverage=coverage + covered_branches    
                total_coverage=total_coverage + total_branches 
    print("----------------Results--------------------------------------------")
    print("The number of covered branches: " + str(coverage))
    print("The number of total branches: " + str(int(total_coverage)))
    print("-------------------------------------------------------------------")
    return coverage

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', action='store_true')
    parser.add_argument("--testcase_file",dest='testcase_file', default="test.txt")
    args = parser.parse_args()
    flag = args.build
    testcase_file = args.testcase_file
    
    if flag==True:
        build_gcov()
    else:
        if not testcase_file:
            print("No testcase")
            exit()
        cov = run_testcase(testcase_file)

if __name__ == '__main__':
    main()
