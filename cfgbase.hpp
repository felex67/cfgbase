#pragma once
#include <sys/types.h>
#include <vector>
#include <string>

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
        const char *const MASK_PRINT[] = { "%s=(null)", "%s=%i", "%s=%u", "%s=%li", "%s=%lu", "%s=%.6f", "%s=%.14lf", "%s=\"%s\"", "[%s]", "%s/%s" };

        /**
         * @class Var
         * @author felexxx67@gmail.com
         * @version 1.0b
         * Абстрактный класс переменной
        */
        class Var {
        protected:
            const u_int64_t _type;
            std::string _name;
            u_int64_t _data;
            /**
             * @param const char *Name - Имя переменной в файле
             * @param const e_vartype Type - Тип переменной
             * @param const u_int64_t Default - Значение по умолчанию
            */
            Var(const char *Name, const e_vartype Type, const u_int64_t Default);

            /**
             * Сбрасывает хранимые данные
             * @return void
            */
            virtual void reset();
        public:
            virtual ~Var();
            /**
             * Сравнивает имя переменной с исходной строкой
             * @param const char *Source - Исходная строка
             * @return bool - Имена совпадают?
            */
            virtual bool chk_name(const std::string &Source) const;
            /**
             * Сканирует строку и сохраняет значение в u_int64_t Var::_data.
             * Для приведения типа используется reinterpret_cast<T &>(_data)
             * @param const std::string &Source - Исходная строка
             * @param const char *Mask = nullptr - Маска для sscanf()
             * @return int - В случае успеха возвращает 0, в противном: -1
            */
            virtual int scan(const std::string &Src, const char *Mask = nullptr) = 0;
            /**
             * Возвращает строковый эквивалент
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
            /**
             * @return e_vartype Type
            */
            virtual e_vartype type() const final;
        };
        /**
         * @class TPL_Var
        */
        template <typename T>
        class TPL_Var : public Var {
        public:
            TPL_Var(const char *Name, const e_vartype Type, const T &Default);
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
            void reset();
        };
        /**
         * @class Group
         * @implements Var
        */
        class Group : public Var {
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
            virtual int load();
            virtual int save() const;
            virtual void set_name(const char *NewName) final;
            virtual void set_path(const char *NewPath) final;
            virtual std::string name() const;
            virtual std::string path() const;
            virtual std::string toString() const;
        protected:
            virtual int scan(const std::string &Src, const char *Mask = nullptr);
            virtual std::vector<std::string> splitv(const std::string &Src) const;
            virtual std::vector<std::string> splitg(const std::string &Src) const;
            virtual size_t countv() const;
            virtual size_t countg() const;
            virtual std::string fload() const;
            virtual int makepath() const;
        };
        struct Int32 : public TPL_Var<int32_t> {
            Int32(const char *Name, const int32_t Default);
            virtual ~Int32();
            Int32& operator=(const int32_t&);
        };
        struct UInt32 : public TPL_Var<u_int32_t> {
            UInt32(const char *Name, const u_int32_t Default);
            virtual ~UInt32();
            UInt32& operator=(const u_int32_t&);
        };
        struct Int64 : public TPL_Var<int64_t> {
            Int64(const char *Name, const int64_t Default);
            virtual ~Int64();
            Int64& operator=(const int64_t&);
        };
        struct UInt64 : public TPL_Var<u_int64_t> {
            UInt64(const char *Name, const u_int64_t Default);
            virtual ~UInt64();
            UInt64& operator=(const u_int64_t&);
        };
        struct Float : public TPL_Var<float> {
            Float(const char *Name, const float Default);
            virtual ~Float();
            Float& operator=(const float&);
        };
        struct Double : public TPL_Var<double> {
            Double(const char *Name, const double Default);
            virtual ~Double();
            Double& operator=(const double&);
        };
        /**
         * @class String
        */
        struct String : public Var {
            String(const char *Name, const char *Default);
            virtual ~String();
            virtual int scan (const std::string &Source, const char *Mask);
            virtual void operator = (const std::string &Source);
            virtual std::string toString() const;
        private:
            virtual void reset();
        };
        std::vector<std::string> split(const char C, const std::string &S);
    };
};

typedef modules::__cfgbase::Instance cfg_inst_t;
typedef modules::__cfgbase::Group cfg_group_t;
typedef modules::__cfgbase::Int32 cfg_int32_t;
typedef modules::__cfgbase::UInt32 cfg_uint32_t;
typedef modules::__cfgbase::Int64 cfg_int64_t;
typedef modules::__cfgbase::UInt64 cfg_uint64_t;
typedef modules::__cfgbase::Float cfg_float;
typedef modules::__cfgbase::Double cfg_double;
typedef modules::__cfgbase::String cfg_string;

/**************************************************************************************************************************/
namespace modules {
    namespace __cfgbase {
    /** @class TPL_Var */
        template <typename T>
        TPL_Var<T>::TPL_Var(const char *N, const e_vartype Tp, const T &D)
            : Var(N, Tp, reinterpret_cast<const u_int64_t&>(D))
        {}
        template <typename T>
        TPL_Var<T>::~TPL_Var() {}
        template <typename T>
        std::string TPL_Var<T>::toString() const {
            char buff[1024];
            sprintf(buff, MASK_PRINT[_type], _name.c_str(), reinterpret_cast<const T&>(_data));
            return buff;
        };
        template <typename T>
        int TPL_Var<T>::scan(const std::string &S, const char *M) {
            return sscanf(S.substr(S.find(MASK_SCAN[_type][0])).c_str(), MASK_SCAN[_type], reinterpret_cast<T*>(&_data));
        }
        template <typename T>
        void TPL_Var<T>::reset() {
            _data = 0;
        }

    };
};
