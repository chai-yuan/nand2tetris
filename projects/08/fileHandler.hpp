#ifndef FILE_HANDLER_INCLUDED_H
#define FILE_HANDLER_INCLUDED_H

#include <dirent.h>
#include <sys/stat.h>
#include <string>

using namespace std;

// 用于管理与检测文件
namespace FileHandler {

string changeExtension(string fileName, string newExtension) {
    if (fileName.rfind(".") != string::npos)
        fileName.erase(fileName.rfind("."));
    fileName.append(newExtension);
    return fileName;
}

bool hasExtension(string fileName, string extension) {
    return fileName.rfind(extension) != string::npos ? true : false;
}

bool isFile(string fileName) {
    struct stat buf;
    stat(fileName.c_str(), &buf);
    return S_ISREG(buf.st_mode);
}

bool isDir(string inputName) {
    struct stat buf;
    stat(inputName.c_str(), &buf);
    return S_ISDIR(buf.st_mode);
}

// 从目录中获取指定后缀的文件列表
vector<string> getFileFromDir(string inputName, string extension) {
    DIR* dir = NULL;
    dirent* dirContent;
    vector<string> ret;

    dir = opendir(inputName.c_str());
    while ((dirContent = readdir(dir)) != NULL) {
        string fileName(dirContent->d_name);
        if (fileName.find(extension) != string::npos)
            ret.push_back(fileName);
    }

    return ret;
}

string removePath(string fileName) {
    if (fileName.back() == '/')
        fileName.erase(fileName.rfind("/"));
    if (fileName.rfind("/") != string::npos)
        fileName.erase(0, fileName.rfind("/") + 1);

    if (fileName.back() == '\\')
        fileName.erase(fileName.rfind("\\"));
    if (fileName.rfind("\\") != string::npos)
        fileName.erase(0, fileName.rfind("\\") + 1);
    return fileName;
}

};  // namespace FileHandler

#endif
