#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <string>
#include <vector>

namespace modules {
    namespace __conf {
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
        protected:
            inline virtual size_t count() const;
        };
        iVar::iVar(const std::string name, const size_t data) : name(name), data(data) {}
        iVar::~iVar() {}
        std::string iVar::toString() const { return (name + "=NaN"); }
        size_t iVar::count() const { return 1; }
        bool iVar::operator==(const std::string Src) const { return (0 == Src.compare(0, name.length(), name) ? true : false); }

        class Group : iVar {
        public:
            inline Group(const std::string name, const size_t size);
            inline virtual ~Group();
            inline virtual int scan(const std::string);
            inline virtual std::string toString() const;
            inline virtual size_t count() const;
            inline virtual bool operator == (const std::string Src) const;
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

        class Instance : iVar {
            std::string path;
        public:
            inline Instance(const std::string name, const std::string path, const size_t size);
            inline ~Instance();
            int load(const char *file = nullptr);
            inline virtual int toString(std::string&);
        protected:
            inline virtual int scan(const std::string);
        private:
            int load(std::string &Dest) const;
        };
        int Instance::load(std::string &Dest) const {
            std::string fname(path + '/' + name);
            char *Src;
            struct stat fstt;
            FILE *fin;
            if (-1 != stat(fname.c_str(), &fstt));
            else { return -1; }
            Src = new char[fstt.st_size + 1];
            if (nullptr != (fin = fopen(fname.c_str(), "rb")));
            else { return -1; }
        }
    };
    
};