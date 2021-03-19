#include <sys/stat.h>
#include <cstring>
#include <cstdio>

#include "cfgbase.hpp"

/**************************************************************************************************************************/
namespace modules {
    namespace __cfgbase {
        std::vector<std::string> split(const char C, const std::string &S) {
            std::vector<std::string> R;
            size_t size = S.size();
            size_t prev = 0;
            for (size_t i = 0; i < size; i++) {
                if (C != S[i]);
                else {
                    if (1 < i - prev){
                        R.push_back(S.substr(prev, i - prev));
                    }
                    prev = i + 1;
                }
            }
            if (prev < (size - 1)) R.push_back(S.substr(prev));
            return R;
        }
    /** @class Var */
        Var::Var(const char *N, const e_vartype T, const u_int64_t D)
            : _name(N), _type(T), _data(D)
        {}
        Var::~Var() {}
        void Var::reset() { _data = 0; }
        bool Var::chk_name(const std::string &S) const { return 0 == S.compare(0, _name.length(), _name); }
        size_t Var::varcount() const { return 1; }
        Var* Var::nextVar() const { return const_cast<Var*>(this + 1); }
        e_vartype Var::type() const  { return reinterpret_cast<const e_vartype&>(_type); }

    /** @class Group */
        Group::Group(const char *N, const size_t Sz)
            : Var(N, VT_GROUP, Sz / sizeof(*this) - 1)
        {}
        Group::~Group() {}
        bool Group::chk_name(const std::string &S) const {
            std::string n('[' + _name + ']');
            return 0 == S.compare(0, n.length(), n);
        }
        Var* Group::nextVar() const { return const_cast<Group*>(this + _data + 1); }
        std::string Group::toString() const {
            std::string R('[' + _name + "]\n");
            const Var *V = (this + 1);
            for (size_t i = 0; i < _data; i++) {
                R += (V[i].toString() + '\n');
            }
            R.erase(R.length() - 1);
            return R;
        }
        int Group::scan(const std::string &S, const char *M) {
            const char *m = (nullptr != M ? M : MASK_SCAN[_type]);
            std::vector<std::string> V = split('\n', S);
            Var *v = this + 1;
            if (chk_name(V.front())) V.erase(V.begin());
            for (size_t i = 0; i < _data; i++) {
                if (0 < V.size()) {
                    for (auto iter = V.begin(); iter != V.end(); iter++) {
                        if (v[i].chk_name(*iter)) {
                            if (-1 != v[i].scan(*iter)) {
                                /* handle error */
                            }
                            V.erase(iter);
                            break;
                        }
                    }
                }
            }
            return 0;
        }

    /** @class Instance */
        Instance::Instance(const char *N, const char *P, const size_t S)
            : Var(N, VT_INSTANCE, (S - sizeof(*this)) / sizeof(Var))
            , _path(P)
        {}
        Instance::~Instance() {}
        void Instance::set_name(const char *n) { this->_name = n; }
        void Instance::set_path(const char *p) { this->_path = p; }
        std::string Instance::name() const { return _name; }
        std::string Instance::path() const { return _path; }
        std::string Instance::fload() const {
            struct stat fstt;
            std::string R(_path + '/' + _name);
            char *buff = nullptr;
            if (-1 != (stat(R.c_str(), &fstt)));
            else { throw std::exception(); }
            buff = new char[fstt.st_size + 1];
            FILE *fin = fopen(R.c_str(), "rb");
            if (nullptr != fin);
            else { throw std::exception(); }
            if (0 < fread(buff, fstt.st_size, 1, fin));
            else { throw std::exception(); }
            buff[fstt.st_size] = 0;
            return buff;
        }
        int Instance::scan(const std::string &S, const char *M) {
            std::vector<std::string> vsrc;
            size_t N = 0;
            Var *next = (this + 1);
            try {
                if (next->type() == VT_GROUP) {
                    vsrc = splitg(S);
                    N = countg();
                }
                else {
                    vsrc = splitv(S);
                    N = countv();
                }
                for (size_t i = 0; i < N; i++) {
                    if (0 < vsrc.size()) {
                        for (auto iter = vsrc.begin(); iter != vsrc.end(); i++) {
                            if (next->chk_name(*iter)) {
                                next->scan(*iter);
                            }
                            vsrc.erase(iter);
                            break;
                        }
                    }
                    next = next->nextVar();
                }
            }
            catch (std::exception &e) { return -1; }
            return 0;
        }
        std::vector<std::string> Instance::splitv(const std::string &Src) const {
            std::vector<std::string> Dest;
            size_t now, pos = 0;
            while (pos < (now = Src.find('\n', pos))) {
                if (2 < (now - pos)) Dest.push_back(Src.substr(pos, now - pos));
                pos = now + 1;
            }
            if (pos < (Src.size() - 1)) { Dest.push_back(Src.substr(pos)); }
            return Dest;
        }
        std::vector<std::string> Instance::splitg(const std::string &Src) const {
            std::vector<std::string> Dest;
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
            return Dest;
        }
        size_t Instance::countv() const  { return _data; }
        size_t Instance::countg() const {
            Var *n = const_cast<Instance*>(this + 1);
            size_t r = 0;
            for (size_t i = 0; i < _data; i++) {
                if (n[i].type() != VT_GROUP);
                else { ++r; }
            }
            return r;
        }
        std::string Instance::toString() const {
            std::string R;
            const Var *iter = (this + 1);
            const Var *end = iter + _data;
            for ( ; iter < end; iter = iter->nextVar()) {
                R += iter->toString();
                R += '\n';
            }
            return R;
        }
        int Instance::load() {
            std::string S;
            try {
                S = fload();
                scan(S, MASK_SCAN[_type]);
            }
            catch (std::exception &e) { return -1; }
            return 0;
        }
        int Instance::save() const {
            std::string fname = _path + '/' + _name;
            FILE *out = fopen(fname.c_str(), "wb");
            size_t r = 0;
            if (nullptr != out);
            else {
                if (-1 != makepath()) {
                    out = fopen(fname.c_str(), "wb");
                }
                else { return -1; }
            }
            fname = toString();
            r = fwrite(fname.c_str(), fname.length(), 1, out);
            fclose(out);
            return r != 0 ? 0 : -1;
        }
        int Instance::makepath() const {
            std::vector<std::string> v = modules::__cfgbase::split('/', _path);
            std::string p;
            int r = -1;
            try {
                if ('/' == _path[0]) { p = '/'; }
                for (auto dir = v.begin(); dir != v.end(); dir++) {
                    if (0 < dir->length()) {
                        p += *dir;
                        r = mkdir(p.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
                        p += '/';
                    }
                }
            }
            catch (std::exception &e) { return -1; }
            return r;
        }

    /** @class Int32 */
        Int32::Int32(const char *N, const int32_t D)
            : TPL_Var(N, VT_INT32, D)
        {}
        Int32::~Int32() {}
        Int32& Int32::operator=(const int32_t& v) {
            reinterpret_cast<int32_t&>(_data) = v;
            return *this;
        }
    /** @class UInt32 */
        UInt32::UInt32(const char *N, const u_int32_t D)
            : TPL_Var(N, VT_UINT32, D)
        {}
        UInt32::~UInt32() {}
        UInt32& UInt32::operator=(const u_int32_t& v) {
            reinterpret_cast<u_int32_t&>(_data) = v;
            return *this;
        }

    /** @class Int64 */
        Int64::Int64(const char *N, const int64_t D)
            : TPL_Var(N, VT_INT64, D)
        {}
        Int64::~Int64() {}
        Int64& Int64::operator=(const int64_t& v) {
            reinterpret_cast<int64_t&>(_data) = v;
            return *this;
        }
    /** @class UInt64 */
        UInt64::UInt64(const char *N, const u_int64_t D)
            : TPL_Var(N, VT_UINT64, D)
        {}
        UInt64::~UInt64() {}
        UInt64& UInt64::operator=(const u_int64_t& v) {
            reinterpret_cast<u_int64_t&>(_data) = v;
            return *this;
        }

    /** @class Float */
        Float::Float(const char *N, const float D)
            : TPL_Var(N, VT_FLOAT, D)
        {}
        Float::~Float() {}
        Float& Float::operator=(const float& v) {
            reinterpret_cast<float&>(_data) = v;
            return *this;
        }
    /** @class Double */
        Double::Double(const char *N, const double D)
            : TPL_Var(N, VT_DOUBLE, D)
        {}
        Double::~Double() {}
        Double& Double::operator=(const double& v) {
            reinterpret_cast<double&>(_data) = v;
            return *this;
        }

    /** @class String */
        String::String(const char *N, const char *D)
            : Var(N, VT_STRING, 0)
        {
            try {
                reinterpret_cast<std::string*&>(_data) = new std::string(D);
            }
            catch (std::exception &e) {}
        }
        String::~String() { reset(); }
        int String::scan(const std::string &S, const char *M) {
            const char *m = (nullptr != M ? M : MASK_SCAN[_type]);
            const char *src = S.c_str();
            size_t start = S.find(*m) + 1;
            size_t end = S.length();
            for (size_t i = start; i < end; i++) {
                if (*m != src[i] || '\\' == src[i - 1]);
                else {
                    reset();
                    reinterpret_cast<std::string* &>(_data) = new std::string(S.substr(start, i - start));
                    return 0;
                }
            }
            return -1;
        }
        void String::reset() { if (0 != _data) delete reinterpret_cast<std::string*&>(_data); _data = 0; }
        void String::operator= (const std::string &S) {
            reset();
            try {
                reinterpret_cast<std::string* &>(_data) = new std::string(S);
            }
            catch (std::exception &e) {}
        }
        std::string String::toString() const {
            return (_name + "=\"" + *(reinterpret_cast<const std::string*>(_data)) + '"');
        }

    };
};
