#pragma once

#include "timestamp.hpp"
#include <string>

namespace club{
    
    enum class ClientState {
        outside, 
        inside,
        waiting, 
        seated
    };

    /*!
        \brief Класс клиента клуба
    */
    class Client {
    public:
        Client() = delete;
        Client(std::string name);

        std::string name() const noexcept;
        ClientState state() const noexcept;

        void arrive(const Timestamp& time);             // command ID 1
        void sit(int table_id, const Timestamp& time);  // command ID 2
        void wait(const Timestamp& time);               // command ID 3\11
        void leave(const Timestamp& time);              // command ID 4\12

        int get_table_id() const noexcept;
        void add_charge(int charge, int hours);
        int total_charge() const noexcept;

    private:
        std::string name_;
        ClientState state_ = ClientState::outside;
        int table_id_ = 0;

        int total_hours_round_ = 0;
        int total_charge_ = 0;
    };
}