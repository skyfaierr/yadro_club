#pragma once

#include "timestamp.hpp"
#include "client.hpp"
#include <memory>

namespace club{   

    /*!
        \brief Класс олицетворяющий игровой стол
        Содержит в себе std::shared_ptr<Client> указывающий на текущего клиента.
        Обратных ссылок у клиента нет для обеспечения безопасности.
    */
    class Table{
    public:
        Table (int id, int hourly_rate) noexcept;

        int id() const noexcept;
        bool is_free() const noexcept;

        void occupy(std::shared_ptr<Client> client, Timestamp time);
        void release(Timestamp time);
        std::shared_ptr<Client> get_client();

        int revenue() const noexcept;
        Timestamp occupied_duration() const noexcept;

    private:
        int id_ = 0;
        int hourly_rate_ = 0;
        bool free_ = true;
        std::shared_ptr<Client> current_client_ = nullptr;
        Timestamp last_occupied_time_;
        Timestamp total_occupied_time_ = Timestamp(0,0);
        unsigned int total_revenue_ = 0;
    };
}