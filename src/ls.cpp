#include <algorithm>
#include <array>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <iomanip>
#include <iostream>
#include <pwd.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#define COUT(x,y) << setfill(' ') << x << y << " " << flush;
#define RESET(x)  << dirList.at(i) << "\033[0m" << x;
#define DIROUT(x) if(isDirectory(dirList.at(i))){ \
                  if(isHidden(dirList.at(i))){ cout << " " << "\033[1;34;47m" RESET(x);} \
                  else{ cout << " " << "\033[1;34m" RESET(x);} } \
                  else if(isExecutable(dirList.at(i))){ \
                  if(isHidden(dirList.at(i))){ cout << " " << "\033[1;32;47m" RESET(x);} \
                  else{ cout << " " << "\033[1;32m" RESET(x);} } \
                  else{ \
                  if(isHidden(dirList.at(i))){ cout << " " << "\033[47m" RESET(x);} \
                  else{ cout << " " RESET(x);} }
#define STAT(x,y,z) int x = stat(y, &z); \
                    if(x == -1){ perror("stat(): cannot access"); exit(-1); }
using namespace std;

struct _setwidth{
	int hard_links = 0;
	int username = 0;
	int group = 0;
	int bytesize = 0;
};

void readDir(vector<string> &dirList, string dirName, int aflag){
    DIR * dirp = opendir(dirName.c_str());
    if(dirp == NULL){
        perror("opendir(): cannot access");
        exit(-1);
    }
    dirent * dirent_p;
    if(aflag > 0){
        while((dirent_p = readdir(dirp))){
            if(dirent_p == NULL){
                perror("readdir(): cannot access");
                exit(-1);
            }
            dirList.push_back(dirent_p->d_name);
        }
    }
    else{
        while((dirent_p = readdir(dirp))){
            if(dirent_p == NULL){
                perror("readdir(): cannot access");
                exit(-1);
            }
            if(dirent_p->d_name[0] != '.') dirList.push_back(dirent_p->d_name);
        }
    }
    int close_status = closedir(dirp);
    if(close_status == -1){
        perror("closedir(): cannot access");
        exit(-1);
    }
}

void makeDir(vector<string> &dirList, const string dirName, int aflag){
    DIR * dirp = opendir(dirName.c_str());
    readDir(dirList, dirName, aflag);
    sort(dirList.begin(), dirList.end(), locale("en_US.UTF-8"));
    int close_status = closedir(dirp);
    if(close_status == -1){
        perror("closedir(): cannot access");
        exit(-1);
    }
}

void setwidth(vector<string> &dirList, _setwidth &widthInfo){
	struct stat width;
	string information;
	for(unsigned i = 0; i < dirList.size(); i++){
		int status = stat(dirList.at(i).c_str(), &width);
		if(status == -1){
			perror("stat(): cannot access");
			exit(-1);
		}
		information = to_string(static_cast<long long>(width.st_nlink));
		if((int)information.size() > widthInfo.hard_links) widthInfo.hard_links = information.size();
		
		struct passwd user = *getpwuid(width.st_uid);
		if(&user == NULL){
			perror("getpwuid(): cannot get user ID");
			exit(-1);
		}
		information = user.pw_name;
		if((int)information.size() > widthInfo.username) widthInfo.username = information.size();
		
		struct group _group = *getgrgid(width.st_gid);
		if(&_group == NULL){
			perror("getgrgid(): cannot get gorup ID)");
			exit(-1);
		}
		information = _group.gr_name;
		if((int)information.size() > widthInfo.group) widthInfo.group = information.size();
		
		information = to_string(static_cast<long long>(width.st_size));
		if((int)information.size() > widthInfo.bytesize) widthInfo.bytesize = information.size();
	}
	return;
}

bool isHidden(string path){
    if(path.at(0) == '.') return true;
    return false;
}

bool isDirectory(string path){
    struct stat buf;
    if(-1 == lstat(path.c_str(), &buf)) perror("lstat(): cannot access");
    return S_ISDIR(buf.st_mode);
}

bool isExecutable(string path){
    struct stat buf;
    if(-1 == lstat(path.c_str(), &buf)) perror("lstat(): cannot access");
    return buf.st_mode & S_IXUSR;
}

void ls(vector<string> &dirList, const vector<string> months, int lflag, _setwidth &widthInfo){ // FIXME: include the l and R flags once testing works
	int length = 0;
	unsigned i = 0;
    struct stat buf;
    struct stat _buf;
	sort(dirList.begin(), dirList.end(), locale("en_US.UTF-8"));
    if(lflag > 0){
        for(; i < dirList.size(); i++){
            STAT(status,dirList.at(i).c_str(),buf);
            if(i == 0){
                STAT(_status,dirList.at(i).c_str(),_buf);
                cout << "total " << _buf.st_blocks << endl;
            }
			//permissions
            char perms[10];
            string _perms;
            fill_n(perms, 10, '-');
            if(S_ISDIR(buf.st_mode)) perms[0] = 'd';
            else if(S_ISCHR(buf.st_mode)) perms[0] = 'l';
            if(buf.st_mode & S_IRUSR ) perms[1] = 'r';
            if(buf.st_mode & S_IWUSR ) perms[2] = 'w';
            if(buf.st_mode & S_IXUSR ) perms[3] = 'x';
            if(buf.st_mode & S_IRGRP ) perms[4] = 'r';
            if(buf.st_mode & S_IWGRP ) perms[5] = 'w';
            if(buf.st_mode & S_IXGRP ) perms[6] = 'x';
            if(buf.st_mode & S_IROTH ) perms[7] = 'r';
            if(buf.st_mode & S_IWOTH ) perms[8] = 'w';
            if(buf.st_mode & S_IXOTH ) perms[9] = 'x';
            for(unsigned j = 0; j < sizeof(perms); j++) _perms+=perms[j];
            cout << _perms << flush;
            cout << " " << flush;
			//hard links
			cout << setw(widthInfo.hard_links) COUT(right,buf.st_nlink);
			//username
            struct passwd user = *getpwuid(buf.st_uid);
            if(&user == NULL){
                perror("getpwuid(): cannot get user ID");
                exit(-1);
            }
			cout << setw(widthInfo.username) COUT(left,user.pw_name);
            //group
            struct group _group = *getgrgid(buf.st_gid);
            if(&_group == NULL){
                perror("getgrgid(): cannot get group ID");
                exit(-1);
            }
			cout << setw(widthInfo.group) COUT(left,_group.gr_name);
			//file size (bytes)
			cout << setw(widthInfo.bytesize) COUT(right,buf.st_size);
			//date last modified
            struct tm time = *localtime(&buf.st_mtime);
            if(time.tm_mon >= 0 || time.tm_mon <= 11) cout << months.at(time.tm_mon) << flush;
			else cout << setw(3) << setfill(' ') << "?" << flush;
            cout << " " << flush;
            cout << setw(2) << setfill(' ') << right << time.tm_mday << " " << flush;
            cout << setw(2) << setfill('0') << time.tm_hour << ":" << flush;
            cout << setw(2) << setfill('0') << time.tm_min << flush;
			//directory name
			DIROUT(endl);
        }
    }
    else{ //prints out the files in the regular format
        for(i = 0; i < dirList.size(); i++){
            length += strlen(dirList.at(i).c_str()) + 4;
			if(length > 64){
                cout << endl;
                length = 0;
            }
            DIROUT(flush);
        }
        cout << endl;
    }
}

int main(int argc, char * argv[]){
    //initialize defaults (size, char arrays, strings, iterators)
    int i = 1, j = 1, k = 0, _flag = 0, aflag = 0, lflag = 0, Rflag = 0;
    string dirName = ".";
    char flags[] = "-alR.";
    bool isfile = false, isdirectory = false;
    vector<string> list, _list, _list_, months;
	_setwidth widthInfo;
	
    months.push_back("Jan"); months.push_back("Feb"); months.push_back("Mar");
    months.push_back("Apr"); months.push_back("May"); months.push_back("Jun");
    months.push_back("Jul"); months.push_back("Aug"); months.push_back("Sep");
    months.push_back("Oct"); months.push_back("Nov"); months.push_back("Dec");

    for(; i < argc; i++){
        if(argv[i][0] == flags[0]){
            for(j = 1; j < (int)strlen(argv[i]); j++){
                if(argv[i][j] == flags[0]) _flag++;
                else if(argv[i][j] == flags[1]) aflag++;
                else if(argv[i][j] == flags[2]) lflag++;
                else if(argv[i][j] == flags[3]) Rflag++;
                else{
                    cerr << "ls: unrecognized option \'";
                    for(; k < (int)strlen(argv[i]); k++) cout << argv[i][k];
                    cerr << "\'" << endl;
                    exit(0);
                }
            }
        }
        else{
            struct stat buf;
            STAT(_status,argv[i],buf);
            isfile = true;
            if(S_ISDIR(buf.st_mode)){
                isdirectory = true;
                _list.push_back(argv[i]);
            }
            else list.push_back(argv[i]);
        }
    }
    if(isfile){
        if(isdirectory){
            makeDir(list, _list.at(0), aflag);
			setwidth(list, widthInfo);
            ls(list, months, lflag, widthInfo);
        }
        else{
			setwidth(list, widthInfo);
            ls(list, months, lflag, widthInfo);
		}
    }
    else{
        makeDir(list, dirName, aflag);
		setwidth(list, widthInfo);
        ls(list, months, lflag, widthInfo);
    }
    if(Rflag > 0){
        makeDir(list, dirName, aflag);
        struct stat buf;
        for(i = 0; i < (int)list.size(); i++){
            STAT(_status_,list.at(i).c_str(),buf);
            if(S_ISDIR(buf.st_mode)){
                readDir(_list_, list.at(i), aflag);
            }
        }
		/*while(_list_.size() != 0){
			list.push_back(_list_.front());
			_list_.erase(_list_.begin());
		}
		setwidth(list, widthInfo);
		ls(list, months, lflag, widthInfo);*/
    }
    return 0;
}
