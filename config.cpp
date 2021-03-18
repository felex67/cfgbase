/**
 * @class Config
 * @version 1.0b
 * @author felexxx67@gmail.com
*/
#include <sys/stat.h>
#include <cstring>
#include <cstdlib>

#include "config.hpp"

namespace modules {
    /**
     * @namespace __conf
     * Inherit base structures of config
    */
    namespace __conf {
        /**
         * @class iVar
         * constructor iVar made as protected to prevent direct usage
         * all methods are virtual
         * @property std::string name - inherits name of variable
         * @property u_int64_t data:
         *      In variables: inherits actual data, in String-variable - pointer to std::string
         *      In Group: inherits count of (variables + 1)
         *      In Instance: inherits total count of variables except Instance
        */
        iVar::iVar(const std::string name, const size_t data) : name(name), data(data) {}
        iVar::~iVar() {}
        std::string iVar::toString() const { return (name + "=NaN"); }
        size_t iVar::count() const { return 1; }
        bool iVar::cmp_name(const std::string Src) const {
            return (0 == Src.compare(0, name.length(), name) ? true : false);
        }
        iVar& iVar::nextVar() const {
            return *(const_cast<iVar*>(this + 1));
        };

        Group::Group(const std::string name, const size_t size) : iVar(name, size / sizeof(iVar)) {}
        Group::~Group() {}
        void Group::make_vector(std::vector<std::string> &V, const std::string &Src) const {
            size_t size = Src.size();
            size_t prev = 0;
            for (size_t i = 0; i < size; i++) {
                if ('\n' != Src[i]);
                else {
                    if (1 < i - prev){
                        V.push_back(Src.substr(prev, i - prev));
                    }
                    prev = i + 1;
                }
            }
            if (prev < (size - 1)) V.push_back(Src.substr(prev));
        }
        int Group::scan(const std::string Src) {
            std::vector<std::string> V;
            iVar* vars = this;
            make_vector(V, Src);
            if (cmp_name(V[0])) V.erase(V.begin());
            else { return -1; }
            for (size_t i = 1; i < data; i++) {
                if (0 < V.size()) {
                    for (auto n = V.begin(); n != V.end(); n++) {
                        if (vars[i].cmp_name(*n)) {
                            vars[i].scan(*n);
                            V.erase(n);
                            break;
                        }
                    }
                }
                else {
                    break;
                }
            }
            return 0;
        }
        bool Group::cmp_name (const std::string Src) const {
            char buff[128];
            iVar *vars;
            sprintf(buff, "[%s]", name.c_str());
            return 0 == Src.compare(0, strlen(buff), buff);
        }
        std::string Group::toString() const {
            std::string res("[");
            res += (name + "]\n");
            const iVar* V = this;
            for (size_t i = 1; i < data; i++) {
                res += V[i].toString() + '\n';
            }
            return res;
        }
        size_t Group::count() const { return data; }
        iVar& Group::nextVar() const {
            return *(const_cast<Group*>(this + data));
        };

        Instance::Instance(const std::string name, const std::string path, const size_t size)
            : iVar(name, (size - sizeof(Instance)) / sizeof(iVar))
            , path(path)
        {}
        Instance::~Instance() {}
        void Instance::set_name(const std::string name) { this->name = name; }
        void Instance::set_path(const std::string path) { this->path = path; }
        int Instance::scan(const std::string fname) {
            std::vector<std::string> vSrc;
            std::string Src = (2 < fname.length() ? fname : (path + '/' + name));
            iVar *cur = this + 1;
            size_t n = this->count();
            try {
                if (-1 != fload(Src));
                else { return -1; }
                if (-1 != split(Src, vSrc));
                else { return -1; }
                for (size_t i = 0; i < n; i++) {
                    for (auto iter = vSrc.begin(); iter != vSrc.end(); iter++) {
                        if (cur->cmp_name(*iter)) {
                            if (-1 != cur->scan(*iter));
                            else { return -1; }
                            vSrc.erase(iter);
                            break;
                        }
                    }
                    cur = &(cur->nextVar());
                }
            }
            catch (std::exception &e) { return -1; }
            return 0;
        }
        int Instance::fload(std::string &Dest) const {
            std::string fname = (Dest.empty() ? (path + '/' + name) : Dest);
            char *Src;
            struct stat fstt;
            FILE *fin;
            if (-1 != stat(fname.c_str(), &fstt));
            else { return -1; }
            if (nullptr != (Src = new char[fstt.st_size + 1]));
            else { return -1; }
            if (nullptr != (fin = fopen(fname.c_str(), "rb")));
            else { return -1; }
            if (0 < fread(Src, fstt.st_size, 1, fin));
            else { return -1; }
            try { Dest = Src; }
            catch (std::exception &e) { return -1; }
            return 0;
        }
        int Instance::split(const std::string &Src, std::vector<std::string> &Dest) const {
            iVar *next = const_cast<Instance*>(this + 1);
            if (1 < next->count()) { return split_grouped(Src, Dest); }
            return split_simple(Src, Dest);
        }
        int Instance::split_simple(const std::string &Src, std::vector<std::string> &Dest) const {
            size_t now, pos = 0;
            while (pos < (now = Src.find('\n', pos))) {
                if (2 < (now - pos)) Dest.push_back(Src.substr(pos, now - pos));
                pos = now + 1;
            }
            if (pos < (Src.size() - 1)) { Dest.push_back(Src.substr(pos)); }
            return 0;
        }
        int Instance::split_grouped(const std::string &Src, std::vector<std::string> &Dest) const {
            size_t start = 0, size = Src.length();
            const char *src = Src.c_str();
            for (size_t i = 1; i < size; i++) {
                if ('\n' != src[i]);
                else {
                    if ('[' != src[i + 1]);
                    else {
                        Dest.push_back(Src.substr(start, i - start));
                        start = ++i;
                    }
                }
            }
            Dest.push_back(Src.substr(start));
            return ((0 < Dest.size()) ? 0 : -1);
        }
        size_t Instance::count() const {
            iVar *n = const_cast<Instance*>(this + 1);
            size_t r = 0;
            if (1 < n->count()) {
                for (size_t i = 0; i < data; i++) {
                    if (1 == n[i].count());
                    else if (1 < n[i].count()) { ++r; }
                    else { return -1; }
                }
            }
            else {
                r = data;
            }
            return (r == 0 ? -1 : r);
        }
        std::string Instance::toString() const {
            std::string r;
            iVar *v = (const_cast<Instance*>(this + 1));
            size_t n = this->count();
            for (size_t i = 0; i < n; i++) {
                r += v->toString();
                v = &(v->nextVar());
            }
            return r;
        }
        int Instance::load(const char *file) {
            return this->scan(file != nullptr ? file : "");
        }
        int Instance::save() const {
            std::string fname = path + '/' + name;
            FILE *out = fopen(fname.c_str(), "wb");
            if (nullptr != out);
            else {
                if (-1 != makepath()) { out = fopen(fname.c_str(), "wb"); }
                else { return -1; }
            }
            fname = this->toString();
            fwrite(fname.c_str(), fname.length(), 1, out);
            fclose(out);
            return 0;
        }
        int Instance::makepath() const {
            std::vector<std::string> vPath;
            std::string t;
            size_t now, pos = 0, len = path.length();
            int res = -1;
            try {
                vPath = this->split_path(path);
                const char *src = path.c_str();
                for (auto i = vPath.begin(); i != vPath.end(); i++) {
                    t += *i;
                    res = mkdir(t.c_str(), (S_IRWXU | S_IRWXG | S_IRWXO));
                    t += '/';
                }
            }
            catch (std::exception &e) {
                return -1;
            }
            return res;
        }
        std::vector<std::string> Instance::split_path(const std::string &Src) const {
            std::vector<std::string> vpath;
            size_t pos = (Src[0] == '/' ? 1 : 0), len = Src.length();
            const char *src = Src.c_str();
            for (size_t i = 0; i < len; i++) {
                std::string t;
                if ('\n' != src[i]);
                else {
                    t = Src.substr(pos, i - pos);
                    if (1 < t.length()) vpath.push_back(t);
                    pos = ++i;
                }
            }
            if (pos < len) vpath.push_back(Src.substr(pos));
            return vpath;
        }

        Int32::Int32(const std::string name, const int32_t data)
            : iVar(name, reinterpret_cast<const u_int64_t&>(data))
        {}
        Int32::~Int32() {}
        int Int32::scan(const std::string Src) {
            if (cmp_name(Src)) {
                return (sscanf(Src.substr(Src.find('=')).c_str(), "=%i", reinterpret_cast<int32_t*>(&data)) ? 0 : -1);
            }
            return -1;
        }
        std::string Int32::toString() const {
            char buff[512];
            sprintf(buff, "%s=%i", name.c_str(), reinterpret_cast<const int32_t&>(data));
            return buff;
        }
        bool Int32::operator == (const int32_t& i) const {
            return reinterpret_cast<const int32_t&>(data) == i;
        }
        Int32& Int32::operator = (const int32_t& i) {
            reinterpret_cast<int32_t&>(data) = i;
            return *this;
        }
        Int32& Int32::operator = (const Int32& i) {
            data = i.data;
            return *this;
        }

        UInt32::UInt32(const std::string name, const u_int32_t data)
            : iVar(name, reinterpret_cast<const u_int64_t&>(data))
        {}
        UInt32::~UInt32() {}
        int UInt32::scan(const std::string Src) {
            if (cmp_name(Src)) {
                return (sscanf(Src.substr(Src.find('=')).c_str(), "=%u", reinterpret_cast<u_int32_t*>(&data)) ? 0 : -1);
            }
            return -1;
        }
        std::string UInt32::toString() const {
            char buff[512];
            sprintf(buff, "%s=%u", name.c_str(), reinterpret_cast<const u_int32_t&>(data));
            return buff;
        }
        bool UInt32::operator == (const u_int32_t& i) const {
            return reinterpret_cast<const u_int32_t&>(data) == i;
        }
        UInt32& UInt32::operator = (const u_int32_t& i) {
            reinterpret_cast<u_int32_t&>(data) = i;
            return *this;
        }
        UInt32& UInt32::operator = (const UInt32& i) {
            data = i.data;
            return *this;
        }

        Int64::Int64(const std::string name, const int64_t data)
            : iVar(name, reinterpret_cast<const u_int64_t&>(data))
        {}
        Int64::~Int64() {}
        int Int64::scan(const std::string Src) {
            if (cmp_name(Src)) {
                return (sscanf(Src.substr(Src.find('=')).c_str(), "=%li", reinterpret_cast<int64_t*>(&data)) ? 0 : -1);
            }
            return -1;
        }
        std::string Int64::toString() const {
            char buff[512];
            sprintf(buff, "%s=%li", name.c_str(), reinterpret_cast<const int64_t&>(data));
            return buff;
        }
        bool Int64::operator == (const int64_t& i) const {
            return reinterpret_cast<const int64_t&>(data) == i;
        }
        Int64& Int64::operator = (const int64_t& i) {
            reinterpret_cast<int64_t&>(data) = i;
            return *this;
        }
        Int64& Int64::operator = (const Int64& i) {
            data = i.data;
            return *this;
        }

        UInt64::UInt64(const std::string name, const u_int64_t data)
            : iVar(name, data)
        {}
        UInt64::~UInt64() {}
        int UInt64::scan(const std::string Src) {
            if (cmp_name(Src)) {
                return (sscanf(Src.substr(Src.find('=')).c_str(), "=%lu", &data) ? 0 : -1);
            }
            return -1;
        }
        std::string UInt64::toString() const {
            char buff[512];
            sprintf(buff, "%s=%lu", name.c_str(), data);
            return buff;
        }
        bool UInt64::operator == (const u_int64_t& i) const {
            return data == i;
        }
        UInt64& UInt64::operator = (const u_int64_t& i) {
            data = i;
            return *this;
        }
        UInt64& UInt64::operator = (const UInt64& i) {
            data = i.data;
            return *this;
        }

        Float::Float(const std::string name, const float data)
            : iVar(name, reinterpret_cast<const u_int64_t&>(data))
        {}
        Float::~Float() {}
        int Float::scan(const std::string Src) {
            if (cmp_name(Src)) {
                return (sscanf(Src.substr(Src.find('=')).c_str(), "=%f", reinterpret_cast<float*>(&data)) ? 0 : -1);
            }
            return -1;
        }
        std::string Float::toString() const {
            char buff[512];
            sprintf(buff, "%s=%f", name.c_str(), reinterpret_cast<const float&>(data));
            return buff;
        }
        bool Float::operator == (const float& i) const {
            return reinterpret_cast<const float&>(data) == i;
        }
        Float& Float::operator = (const float& i) {
            reinterpret_cast<float&>(data) = i;
            return *this;
        }
        Float& Float::operator = (const Float& i) {
            data = i.data;
            return *this;
        }

        Double::Double(const std::string name, const double data)
            : iVar(name, reinterpret_cast<const u_int64_t&>(data))
        {}
        Double::~Double() {}
        int Double::scan(const std::string Src) {
            if (cmp_name(Src)) {
                return (sscanf(Src.substr(Src.find('=')).c_str(), "=%lf", reinterpret_cast<double*>(&data)) ? 0 : -1);
            }
            return -1;
        }
        std::string Double::toString() const {
            char buff[512];
            sprintf(buff, "%s=%lf", name.c_str(), reinterpret_cast<const double&>(data));
            return buff;
        }
        bool Double::operator == (const double& i) const {
            return reinterpret_cast<const double&>(data) == i;
        }
        Double& Double::operator = (const double& i) {
            reinterpret_cast<double&>(data) = i;
            return *this;
        }
        Double& Double::operator = (const Double& i) {
            data = i.data;
            return *this;
        }

        String::String(const std::string name, const std::string data)
            : iVar(name, reinterpret_cast<const u_int64_t>(new std::string(data)))
        {}
        String::~String() {
            if (0 != data) delete reinterpret_cast<std::string*>(data);
        }
        int String::scan(const std::string Src) {
            if (cmp_name(Src)) {
                std::string t = Src.substr(Src.find('='));
                size_t start = t.find('"') + 1, len = t.length();
                const char *src = t.c_str();
                for (size_t i = start; i < len; i++) {
                    if (('"' != src[i]) || ('\\' == src[i - 1]));
                    else {
                        reinterpret_cast<std::string*&>(data) = new std::string(t.substr(start, i - start));
                        break;
                    }
                }
                return 0;
            }
            return -1;
        }
        std::string String::toString() const {
            char buff[512];
            sprintf(buff, "%s=\"%s\"", name.c_str(), reinterpret_cast<std::string*&>(const_cast<u_int64_t&>(data))->c_str());
            return buff;
        }
        bool String::operator == (const std::string &s) const {
            return (reinterpret_cast<const std::string*>(data))->compare(s);
        }
        String& String::operator = (const std::string& i) {
            reinterpret_cast<std::string&>(data) = i;
            return *this;
        }
        String& String::operator = (const String& i) {
            reinterpret_cast<std::string*&>(data) = new std::string(*(reinterpret_cast<const std::string*>(i.data)));
            return *this;
        }
        bool String::compare (const std::string &c) const {
            const std::string *s = reinterpret_cast<const std::string *>(data);
            return s->compare(c);
        }
    };
};