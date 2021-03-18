#pragma once

#include <sys/types.h>
#include <string>
#include <vector>

namespace modules {
    namespace __conf {
        class iVar {
        protected:
            std::string name;
            u_int64_t data;

            iVar(const std::string name, const size_t data);
        public:
            virtual ~iVar();
            virtual bool cmp_name (const std::string Src) const;
            virtual int scan(const std::string) = 0;
            virtual std::string toString() const;
            virtual iVar& nextVar () const;
            virtual size_t count() const;
        };

        class Group : protected iVar {
        public:
            Group(const std::string name, const size_t size);
            virtual ~Group();
            virtual int scan(const std::string);
            virtual std::string toString() const;
            virtual size_t count() const;
            virtual bool cmp_name (const std::string Src) const;
            virtual iVar& nextVar () const;
        protected:
            void make_vector(std::vector<std::string> &V, const std::string &Src) const;
        };

        class Instance : protected iVar {
            std::string path;
        public:
            Instance(const std::string name, const std::string path, const size_t size);
            ~Instance();
            virtual void set_name(const std::string name);
            virtual void set_path(const std::string path);
            virtual size_t count() const;
            virtual int load(const char *FileName = "");
            virtual int save() const;
            virtual std::string toString() const;
        protected:
            virtual int scan(const std::string);
            virtual int fload(std::string &Dest) const;
            virtual int split(const std::string &Src, std::vector<std::string> &Dest) const;
            virtual int split_simple(const std::string &Src, std::vector<std::string> &Dest) const;
            virtual int split_grouped(const std::string &Src, std::vector<std::string> &Dest) const;
            virtual int makepath() const;
            virtual std::vector<std::string> split_path(const std::string &Src) const final;
        };

        class Int32 : public iVar {
        public:
            Int32(const std::string name, const int32_t Data);
            virtual ~Int32();
            virtual int scan(const std::string Src);
            virtual std::string toString() const;
            virtual bool operator == (const int32_t& i) const;
            virtual Int32& operator = (const int32_t& i);
            virtual Int32& operator = (const Int32& i);
        };

        class UInt32 : public iVar {
        public:
            UInt32(const std::string name, const u_int32_t Data);
            virtual ~UInt32();
            virtual int scan(const std::string Src);
            virtual std::string toString() const;
            virtual bool operator == (const u_int32_t& i) const;
            virtual UInt32& operator = (const u_int32_t& i);
            virtual UInt32& operator = (const UInt32& i);
        };

        class Int64 : public iVar {
        public:
            Int64(const std::string name, const int64_t Data);
            virtual ~Int64();
            virtual int scan(const std::string Src);
            virtual std::string toString() const;
            virtual bool operator == (const int64_t& i) const;
            virtual Int64& operator = (const int64_t& i);
            virtual Int64& operator = (const Int64& i);
        };

        class UInt64 : public iVar {
        public:
            UInt64(const std::string name, const u_int64_t Data);
            virtual ~UInt64();
            virtual int scan(const std::string Src);
            virtual std::string toString() const;
            virtual bool operator == (const u_int64_t& i) const;
            virtual UInt64& operator = (const u_int64_t& i);
            virtual UInt64& operator = (const UInt64& i);
        };

        class Float : public iVar {
        public:
            Float(const std::string name, const float Data);
            virtual ~Float();
            virtual int scan(const std::string Src);
            virtual std::string toString() const;
            virtual bool operator == (const float& i) const;
            virtual Float& operator = (const float& i);
            virtual Float& operator = (const Float& i);
        };

        class Double : public iVar {
        public:
            Double(const std::string name, const double Data);
            virtual ~Double();
            virtual int scan(const std::string Src);
            virtual std::string toString() const;
            virtual bool operator == (const double& i) const;
            virtual Double& operator = (const double& i);
            virtual Double& operator = (const Double& i);
        };

        class String : public iVar {
        public:
            String(const std::string name, const std::string Data);
            virtual ~String();
            virtual int scan(const std::string Src);
            virtual std::string toString() const;
            virtual bool operator == (const std::string &s) const;
            virtual String& operator = (const std::string& i);
            virtual String& operator = (const String& i);
            virtual bool compare (const std::string &c) const;
        };
    };
};

typedef modules::__conf::Instance cfg_inst_t;
typedef modules::__conf::Group cfg_group_t;
typedef modules::__conf::Int32 cfg_int32_t;
typedef modules::__conf::UInt32 cfg_uint32_t;
typedef modules::__conf::Int64 cfg_int64_t;
typedef modules::__conf::UInt64 cfg_uint64_t;
typedef modules::__conf::Float cfg_float_t;
typedef modules::__conf::Double cfg_double_t;
typedef modules::__conf::String cfg_string_t;

