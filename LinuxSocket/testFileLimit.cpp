/*************************************************************************
	> File Name: testFileLimit.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Fri Sep  8 15:46:12 2017
 ************************************************************************/

#include <iostream>

#include <sys/resource.h>

using namespace std;

int main()
{
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) == -1)
    {
        cerr << "getrlimit error" << endl;
        exit(1);
    }

    cout << "soft limit: " << rl.rlim_cur << endl;
    cout << "hard limit: " << rl.rlim_max << endl;
    cout << "---------------------------->" << endl;

    rl.rlim_cur = 2048;
    rl.rlim_max = 2048;
    if (setrlimit(RLIMIT_NOFILE, &rl) == -1)
    {
        cerr << "setrlimit error" << endl;
        exit(1);
    }

    if (getrlimit(RLIMIT_NOFILE, &rl) == -1)
    {
        cerr << "getrlimit error" << endl;
        exit(1);
    }

    cout << "soft limit: " << rl.rlim_cur << endl;
    cout << "hard limit: " << rl.rlim_max << endl;
}
