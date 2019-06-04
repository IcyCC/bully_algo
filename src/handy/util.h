    //
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_UTIL_H
#define BULLY_ALGO_UTIL_H

#include<string>
#include <netinet/in.h>

namespace handy {
    struct noncopyable {
    protected:
        noncopyable() = default;
        virtual ~noncopyable() = default;

        noncopyable(const noncopyable &) = delete;
        noncopyable &operator=(const noncopyable &) = delete;
    };

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
        virtual std::string GetSubstr(std::size_t pos, std::size_t n) = 0;
    };

    class BufferCRLF: public Buffer {
    private:
        std::string buffer;
        std::string sep;
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
                res = buffer.substr(0, pos);
                buffer = buffer.substr(pos);
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
}
#endif //BULLY_ALGO_UTIL_H
