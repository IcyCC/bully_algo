//
// Created by zhandong on 2019-06-04.
//

#ifndef BULLY_ALGO_UTIL_H
#define BULLY_ALGO_UTIL_H

#include <vector>
#include <string>


std::vector<std::string> SplitString(std::string src, std::string sp) {
    std::string::size_type pos1, pos2 = 0;
    std::vector<std::string> v;
    pos2 = src.find(sp);
    pos1 = 0;
    while (std::string::npos != pos2) {
        v.push_back(src.substr(pos1, pos2 - pos1));

        pos1 = pos2 + sp.size();
        pos2 = src.find(sp, pos1);
    }
    if (pos1 != src.length()) {
        v.push_back(src.substr(pos1));
    }
    return v;
}


#endif //BULLY_ALGO_UTIL_H
