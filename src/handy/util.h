    //
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_UTIL_H
#define BULLY_ALGO_UTIL_H

#include<string>
#include <vector>
#include <iostream>
#include <netinet/in.h>

namespace handy {
    struct noncopyable {
    protected:
        noncopyable() = default;
        virtual ~noncopyable() = default;

        noncopyable(const noncopyable &) = delete;
        noncopyable &operator=(const noncopyable &) = delete;
    };

    inline std::vector<std::string> SpliteString(const std::string& src,const std::string & sp) {
        // 分割字符串
        std::string::size_type pos1, pos2=0;
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
    };


    inline bool IsEndWith(std::string s, std::string m) {
        if (m.length() > s.length()) {
            return false;
        }
        for (int i = 0; i < m.length(); i++) {
            if (m[i] != s[s.length() - m.length() + i]) {
                return false;
            }
        }
        return true;
    }

    int64_t TimeMicro();
    inline int64_t TimeMilli() { return TimeMicro() / 1000; }

    enum BufferType {
        BUFF_CRLF
    };

    class Buffer {
    public:
        Buffer() = default;
        ~Buffer() = default;
        virtual void Push(const std::string &msg) = 0;
        virtual std::string GetLine() = 0;
        virtual std::string GetBuffer() = 0;
        virtual void Clear() = 0;
        virtual std::size_t Size() = 0;
        virtual std::string GetSubstr(std::size_t pos, std::size_t n)=0;
    };

    class BufferCRLF: public Buffer {
    private:
        std::string buffer;
        std::string sep;
        std::string tmp_buffer;
    public:
        BufferCRLF(): sep("\r\n") {}
        ~BufferCRLF() = default;
        inline void Push(const std::string &msg) { buffer += msg; }
        inline void Clear() { buffer.clear(); };
        inline std::string GetBuffer() { return buffer; }
        inline std::size_t Size() { return buffer.size(); }
        inline std::string GetSubstr(std::size_t pos, std::size_t n) { return buffer.substr(pos, n); }
        inline std::string GetLine() { 
            std::string res;
            auto pos = buffer.find(sep);
            if(pos == buffer.npos)
                res = "";
            else {
                res = buffer.substr(0, pos+sep.length());
                buffer = buffer.substr(pos+sep.length());
            }
            return res;
        }
    };

    class BufferFactory {
    public:
        inline Buffer* CreateBuffer(BufferType type) {
            if(type == BufferType::BUFF_CRLF) {
                return new BufferCRLF();
            }
        }
    };

    

    struct IPv4Addr {
        struct sockaddr_in addr_;
        IPv4Addr(const std::string &host, unsigned short port);
        IPv4Addr(const struct sockaddr_in &addr) : addr_(addr) {};
    };

    inline void PutLog(const std::string s) {
        std::cout<<s<<std::endl;
    };
}
#endif //BULLY_ALGO_UTIL_H
