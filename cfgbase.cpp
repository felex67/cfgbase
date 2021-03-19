#include <sys/types.h>
#include <cstring>
#include <string>
#include <vector>

namespace modules {
    namespace __cfgbase {
        enum e_vartype {
            VT_INVALID,
            VT_INT32,
            VT_UINT32,
            VT_INT64,
            VT_UINT64,
            VT_FLOAT,
            VT_DOUBLE,
            VT_STRING,
            VT_GROUP,
            VT_INSTANCE,
        };
        const char *const MASK_SCAN[] = { nullptr, "=%i", "=%u", "=%li", "=%lu", "=%f", "=%lf", "\"", "[%[^]]s", nullptr };
        const char *const MASK_PRINT[] = { "%s=\"NaN\"", "%s=%i", "%s=%u", "%s=%li", "%s=%lu", "%s=%f", "%s=%lf", "%s=\"%s\"", "[%s]", "%s/%s" };

        /**
         * @class Var
         * @author felexxx67@gmail.com
         * @version 1.0b
        */
        class Var {
        protected:
            const u_int64_t _type;
            std::string _name;
            u_int64_t _data;
            /**
             * @param const char *Name
             * @param const e_vartype Type
             * @param const u_int64_t Default
            */
            Var(const char *Name, const e_vartype Type, const u_int64_t Default);

            /**
             * @return void
            */
            virtual void reset();
        public:
            virtual ~Var();
            /**
             * @param const char *Source
             * @return bool
            */
            virtual bool chk_name(const std::string &Src) const;
            /**
             * @param const std::string &Source
             * @param const char *Mask = nullptr
             * @return int - 0 on success, -1 for error
            */
            virtual int scan(const std::string &Src, const char *Mask = nullptr) = 0;
            /**
             * @return std::string
            */
            virtual std::string toString() const = 0;
            /**
             * @return size_t : count of Vars in container
            */
            virtual size_t varcount() const;
            /**
             * @return Var* - pointer to next variable of same type(Group | Var)
            */
            virtual Var* nextVar() const;
        private:
            void operator=(const Var &O);
        };
        /**
         * @class TPL_Var
        */
        template <typename T>
        class TPL_Var : public Var {
            void operator=(const TPL_Var<T>&);
        public:
            TPL_Var(const char *Name, const e_vartype Type, const T& Default);
            virtual ~TPL_Var();
            /**
             * @param const std::string &Source
             * @param const char *Mask
             * @return int - 0 on success, -1 for error
            */
            virtual int scan(const std::string &Src, const char *Mask = nullptr);
            /**
             * @return std::string
            */
            virtual std::string toString() const;
            /**
             * @param const T &Value
             * @return void
            */
            void operator = (const T& value);
        };
        /**
         * @class Group
         * @implements Var
        */
        class Group : public Var {
            void operator=(const Group &);
        public:
            Group(const char *Name, size_t SizeofContainer);
            virtual ~Group();
            virtual int scan(const std::string &Src, const char *Mask = nullptr);
            virtual std::string toString() const;
            virtual bool chk_name(const std::string &Src) const;
            virtual Var* nextVar() const;
        };
        /**
         * @class Instance
         * @implements Var
        */
        class Instance : protected Var {
            std::string _path;
        public:
            Instance(const char *FileName, const char *Path, const size_t SizeofContainer);
            virtual ~Instance();
            virtual int load(const char *FileName);
            virtual std::string toString() const;
            virtual int save() const;
            virtual void set_name(const char *NewName) final;
            virtual void set_path(const char *NewPath) final;
        protected:
            virtual int scan(const const std::string &Src, const char *Mask = nullptr);
            virtual std::vector<std::string> split_Vars(const char *Src) const;
            virtual std::vector<std::string> split_Groups(const char *Src) const;
            virtual size_t count_Vars() const;
            virtual size_t count_Groups() const;
            virtual std::string fload() const;
        };
        struct Int32 : public TPL_Var<int32_t> {
            Int32(const char *Name, const int32_t Default);
            virtual ~Int32();
        };
        struct UInt32 : public TPL_Var<u_int32_t> {
            UInt32(const char *Name, const u_int32_t Default);
            virtual ~UInt32();
        };
        struct Int64 : public TPL_Var<int64_t> {
            Int64(const char *Name, const int64_t Default);
            virtual ~Int64();
        };
        struct UInt64 : public TPL_Var<u_int64_t> {
            UInt64(const char *Name, const u_int64_t Default);
            virtual ~UInt64();
        };
        struct Float : public TPL_Var<float> {
            Float(const char *Name, const float Default);
            virtual ~Float();
        };
        struct Double : public TPL_Var<double> {
            Double(const char *Name, const double Default);
            virtual ~Double();
        };
        /**
         * @class String
        */
        struct String : public Var {
            String(const char *Name, const char *Default);
            virtual ~String();
            virtual int scan (const std::string &Source, const char *Mask);
            virtual void operator = (const std::string &Source);
        private:
            virtual void reset();
        };
        std::vector<std::string> split(const char C, const std::string &S);
    };
};
/**************************************************************************************************************************/
using namespace modules::__cfgbase;
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
        std::string R;
        const Var *V = (this + 1);
        for (size_t i = 0; i < _data; i++) {
            R += (V[i].toString() + '\n');
        }
        return R;
    }
    int Group::scan(const std::string &S, const char *M = nullptr) {
        const char *m = (nullptr != M ? M : MASK_SCAN[_type]);
        std::vector<std::string> V = split('\n', S);
        Var *v = this + 1;
        V.erase(V.begin());
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
