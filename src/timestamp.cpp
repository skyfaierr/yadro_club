#include "..//include/timestamp.hpp"
#include <stdexcept>
#include <string>
#include <format>

using namespace std::string_literals;

club::Timestamp::Timestamp(int h, int m) noexcept: hours_(h), minutes_(m){}

club::Timestamp::Timestamp(std::string str){
    size_t delim_pos = str.find(":");
    if (delim_pos == std::string::npos) {
        // throw std::invalid_argument("bla bla bla");
    }

    hours_ = std::stoi(str.substr(0,delim_pos));
    minutes_ = std::stoi(str.substr(delim_pos+1));

    if(hours_ < 0 || hours_ > 23 || minutes_ < 0 || minutes_ > 59){
        // throw std::invalid_argument("potom catch ne zabud'");
    }
}

club::Timestamp club::Timestamp::parse(std::string time_str){
    return Timestamp(time_str);
}
std::string club::Timestamp::to_string() const {
    return std::format("{:02d}:{:02d}", hours_, minutes_);
}
int club::Timestamp::hours() const noexcept {
    return hours_;
}
int club::Timestamp::minutes() const noexcept {
    return minutes_;
}
int club::Timestamp::round() {
    if (minutes_ > 0) return (hours_ + 1);
    return hours_;
}

bool club::Timestamp::operator<(const Timestamp& other) const {
    return (hours_ < other.hours_) || (hours_==other.hours_ && minutes_ < other.minutes_);
}
bool club::Timestamp::operator>(const Timestamp& other) const {
    return !(*this < other);
}
bool club::Timestamp::operator==(const Timestamp& other) const {
    return (hours_==other.hours_ && minutes_==other.minutes_);
}
bool club::Timestamp::operator<=(const Timestamp& other) const {
    return (*this < other) || (*this == other);
}
bool club::Timestamp::operator>=(const Timestamp& other) const {
    return (*this > other) || (*this == other);
}

bool club::Timestamp::operator!=(const Timestamp& other){
    return (hours_ == other.hours_ && minutes_ == other.minutes_ ? false : true);
};
club::Timestamp& club::Timestamp::operator=(const Timestamp& other){
    if(*this != other) {
        this->hours_ = other.hours_;
        this->minutes_ = other.minutes_;
    }
    return *this;
};

club::Timestamp club::Timestamp::operator+(const Timestamp& other){
    int total_minutes = (hours_ + other.hours_) * 60 + (minutes_ + other.minutes_);
    return Timestamp(total_minutes / 60, total_minutes % 60);
};
club::Timestamp club::Timestamp::operator-(const Timestamp& other){
    if (other < *this){
        int total_minutes = (hours_ * 60 + minutes_) - (other.hours_ * 60 + other.minutes_);
        return Timestamp(total_minutes / 60, total_minutes % 60);
    }
    return *this;
};
club::Timestamp& club::Timestamp::operator+=(const Timestamp& other){
    this->hours_ += other.hours_;
    this->minutes_ += other.minutes_;
    return *this;
};
std::ostream& club::operator<<(std::ostream& os, const Timestamp& ts) {
    os << ts.to_string();
    return os;
};
