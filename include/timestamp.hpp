#pragma once

#include <string>

namespace club{
    
    /*!
        \brief Класс для интерпретации времени формата <hh:mm> 
        
        Данный класс имеет только одну простую цель: упростить восприятие
        того, как выглядят временный метки в коде; а также сделать удобным 
        сравнение этих меток.
    */
    class Timestamp{
    public:
        Timestamp() = default;
        Timestamp(int h, int m) noexcept;
        Timestamp(std::string str);

        Timestamp parse(std::string time_str);
        std::string to_string() const;
        int hours() const noexcept; 
        int minutes() const noexcept; 
        int round();

        bool operator<(const Timestamp& other) const ;
        bool operator>(const Timestamp& other) const ;
        bool operator==(const Timestamp& other) const ;
        bool operator<=(const Timestamp& other) const ;
        bool operator>=(const Timestamp& other) const ;

        Timestamp& operator=(const Timestamp& other);
        bool operator!=(const Timestamp& other);

        Timestamp operator+(const Timestamp& other);
        Timestamp operator-(const Timestamp& other);
        Timestamp& operator+=(const Timestamp& other);
        friend std::ostream& operator<<(std::ostream& os, const Timestamp& ts);

    private:
        int hours_ = 0;
        int minutes_ = 0;
    };
}