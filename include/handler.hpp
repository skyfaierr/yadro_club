#pragma once

#include <string>
#include <vector>
#include <memory>

#include "event.hpp"
#include "club.hpp"

namespace club{

    //forward declaration
    class Club;

    /*!
        \brief Класс описывает конфигурацию компьютерного клуба и запускает его работу
    */
    class EventHandler{
    public:
        EventHandler(const std::string& input_file);
        void run();
    private:
        void load_file();
        void process_event(std::unique_ptr<Event> event);

        void finilize_day();
        void print_report() const;
    
    private:
        std::string input_file_;
        std::vector<std::unique_ptr<Event>> events_;
        std::unique_ptr<Club> club_;
    };

    /*!
        \brief Фабрика событий для входящих и исходящих комманд
    */
    class EventFactory{
    public:
        //parsing lines from input file (ID 1-4)
        static std::unique_ptr<Event> create_from_input(const std::string& line);
        //creates internal events (ID 11-13)
        static std::unique_ptr<Event> create_internal(Timestamp time, EventType type, const std::string& client_or_message, int table_id = -1);
    };
}