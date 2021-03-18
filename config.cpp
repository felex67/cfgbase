/**
 * @class Config
 * @version 1.0b
 * @author felexxx67@gmail.com
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>

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
        class iVar {
        protected:
            std::string name;
            u_int64_t data;

            inline iVar(const std::string name, const size_t data);
        public:
            inline virtual ~iVar();
            inline virtual bool operator == (const std::string Src) const;
            inline virtual int scan(const std::string) = 0;
            inline virtual std::string toString() const;
            inline virtual iVar& nextVar () const;
            inline virtual size_t count() const;
        };
        iVar::iVar(const std::string name, const size_t data) : name(name), data(data) {}
        iVar::~iVar() {}
        std::string iVar::toString() const { return (name + "=NaN"); }
        size_t iVar::count() const { return 1; }
        bool iVar::operator==(const std::string Src) const { return (0 == Src.compare(0, name.length(), name) ? true : false); }
        iVar& iVar::nextVar() const {
            return *(const_cast<iVar*>(this + 1));
        };

        class Group : protected iVar {
        public:
            inline Group(const std::string name, const size_t size);
            inline virtual ~Group();
            inline virtual int scan(const std::string);
            inline virtual std::string toString() const;
            inline virtual size_t count() const;
            inline virtual bool operator == (const std::string Src) const;
            inline virtual iVar& nextVar () const;
        protected:
            void make_vector(std::vector<std::string> &V, const std::string &Src) const;
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
                    prev = i;
                }
            }
            if (prev < (size - 1)) V.push_back(Src.substr(prev));
        }
        int Group::scan(const std::string Src) {
            std::vector<std::string> V;
            iVar* vars = this;
            make_vector(V, Src);
            if (*this == V[0]) V.erase(V.begin());
            else { return -1; }
            for (size_t i = 1; i < data; i++) {
                size_t vsz = V.size();
                if (0 < vsz) {
                    for (auto n = V.begin(); n != V.end(); n++) {
                        if (vars[i] == *n) {
                            vars[i].scan(n->substr(n->find('=')));
                            V.erase(n);
                        }
                    }
                }
                else {
                    break;
                }
            }
            return 0;
        }
        bool Group::operator == (const std::string Src) const {
            char buff[128];
            iVar *vars;
            sprintf(buff, "[%s]", name.c_str());
            return 0 == Src.compare(0, strlen(buff), buff);
        }
        std::string Group::toString() const {
            std::string res("[");
            res += (name + "]\n");
            const iVar* V = (this + 1);
            for (size_t i = 0; i < data; i++) {
                res += V[i].toString() + '\n';
            }
            return res;
        }
        size_t Group::count() const { return data; }
        iVar& Group::nextVar() const {
            return *(const_cast<Group*>(this + data));
        };

        class Instance : protected iVar {
            std::string path;
        public:
            inline Instance(const std::string name, const std::string path, const size_t size);
            inline ~Instance();
            inline virtual void set_name(const std::string name);
            inline virtual void set_path(const std::string path);
            inline virtual size_t count() const;
            inline virtual int load(const char *FileName = "");
            inline virtual int save() const;
            inline virtual int save_as(const std::string fname) const;
        protected:
            inline virtual int scan(const std::string);
            inline virtual std::string toString() const;
            inline virtual int fload(std::string &Dest) const;
            inline virtual int split(const std::string &Src, std::vector<std::string> &Dest) const;
            inline virtual int split_simple(const std::string &Src, std::vector<std::string> &Dest) const;
            inline virtual int split_grouped(const std::string &Src, std::vector<std::string> &Dest) const;
            inline virtual int makepath() const;
            inline virtual std::vector<std::string> split_path(const std::string &Src) const final;
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
            std::string Src = fname;
            iVar &cur = *(this + 1);
            size_t n = this->count();
            try {
                if (-1 != fload(Src));
                else { return -1; }
                if (-1 != split(Src, vSrc));
                else { return -1; }
                for (size_t i = 0; i < n; i++) {
                    for (auto iter = vSrc.begin(); iter != vSrc.end(); iter++) {
                        if (cur == *iter) {
                            if (-1 != cur.scan(*iter));
                            else { return -1; }
                            vSrc.erase(iter);
                            break;
                        }
                    }
                    cur = cur.nextVar();
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
            iVar &v = *(const_cast<Instance*>(this + 1));
            size_t n = this->count();
            for (size_t i = 0; i < n; i++) {
                r += v.toString();
                v = v.nextVar();
            }
            return r;
        }
        int Instance::load(const char *file) {
            return this->scan(file != nullptr ? file : "");
        }
        int Instance::save() const {
            std::string fname = path + name;
            FILE *out = fopen(fname.c_str(), "wb");
            if (nullptr != out);
            else {
                if (-1 != makepath()) { out = fopen(fname.c_str(), "wb"); }
                else { return -1; }
            }
        }
        int Instance::makepath() const {
            std::vector<std::string> vPath;
            std::string t;
            size_t now, pos = 0, len = path.length();
            int res = -1;
            try {
                vPath = this->split_path(path);
                const char *src = path.c_str();
                for (size_t i = 1; i < len; i++) {
                    if (src[i] != '/');
                    else {
                        vPath.push_back(path.substr(pos, now - pos));
                        pos = i + 1;
                    }
                }
                for (auto i = vPath.begin(); i != vPath.end(); i++) {
                    t += "/" + *i;
                    res = mkdir(t.c_str(), (S_IRWXU | S_IRWXG | S_IRWXO));
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
    };
};