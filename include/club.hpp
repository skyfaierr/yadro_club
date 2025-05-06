#pragma once // not today include guard...

#include <memory>
#include <vector>
#include <map>
#include <queue>
#include <string>

#include "handler.hpp"
#include "timestamp.hpp"
#include "table.hpp"
#include "client.hpp"

namespace club{    

    /*!
        \brief Класс описывает конфигурацию компьютерного клуба
    */
    struct ClubConfig{
        int table_amount = 0;
        int hourly_rate = 0;
        Timestamp opening_time;
        Timestamp closing_time; 
    };

    /*!
        \brief Основной класс, управляющий состоянием клуба.
    */
    class Club{
    public:
        const ClubConfig config_;

        Club () = delete;
        Club(const ClubConfig& cfg);

        std::shared_ptr<Client> get_or_create_client(const std::string& name);
        Table* get_table(int id);
        bool any_free_table() const ;
        bool is_open(const Timestamp& time) const ;

        void add_to_queue(Timestamp time, std::string& name);
        std::string remove_from_queue();
        bool any_waiting() const noexcept;
        bool queue_valid() const noexcept;

        std::vector<std::unique_ptr<Event>> depart_all_clients(Timestamp time);
        
        int hourly_rate() const noexcept;
        Timestamp open_time() const noexcept;
        Timestamp close_time() const noexcept;

    private:
        std::map<std::string,std::shared_ptr<Client>> clients_;                                 // все клиенты
        // std::unordered_map<ClientState, std::unordered_set<std::string>> clients_by_state_;  // клиенты в соотн. с состояниями
        // std::unordered_map<int, std::shared_ptr<Table>> tables_;                                
        std::map<int, std::shared_ptr<Table>> tables_;                                          // все столы
        std::queue<std::string> waiting_list_;                                                  // список ожидания

        friend class EventHandler;
    };
}