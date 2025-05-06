#pragma once

#include "timestamp.hpp"

#include <memory>
#include <optional>
#include <string>

namespace club{    

    //forward declaration
    class Club; 

    /*!
        \brief Идентификторы возможных событий
    */
    enum class EventType{
        client_came = 1,        // Клиент пришел в клуб 
        client_seated = 2,      // Клиент садится за стол 
        client_waiting = 3,     // Клиент ожидает в очереди
        client_left = 4,        // Клиент выходит из клуба

        lost_client = 11,       // Потеря клиента(переполнение очереди) либо конец рабочего дня клуба.
        moved_client = 12,      // Клиент из очереди садится на освободившийся стол

        error = 13              // Выводится сразу после события, вызвавшего ошибку. 
                                // Ошибочное событие не выполняется, эффекта на состояние клиентов не оказывает.
    };

    /*!
        \brief Базовый класс события
    */
    class Event{
    public:
        Event() = default;
        Event(Timestamp time, EventType type);
        virtual ~Event() = default;

        Timestamp time() const noexcept;
        EventType type() const noexcept;
        virtual std::string to_string() const;

        // while Event can create only one output event - using vector is overhead(for future purposes only)
        // virtual std::vector<std::unique_ptr<Event> handle(Club& club) = 0;
        virtual std::optional<std::unique_ptr<Event>> handle(Club& club) = 0;

    protected:
        Timestamp time_;
        EventType type_;
        // std::string client_name_;
    };
    
    namespace inEvents{    
    
        class ClientArrival : public Event {
        public:
            ClientArrival(Timestamp time, std::string client_name);
            std::string to_string() const override;
            std::optional<std::unique_ptr<Event>> handle(Club& club) override;
        private:
            std::string client_name_;
        };
        
        class ClientSeated : public Event {
        public:
            ClientSeated(Timestamp time, std::string client_name, int table_id);
            std::string to_string() const override;
            std::optional<std::unique_ptr<Event>> handle(Club& club) override;
        private:
            std::string client_name_;
            int table_id_;
        };
        
        class ClientWaiting : public Event {
        public:
            ClientWaiting(Timestamp time, std::string client_name);
            std::string to_string() const override;
            std::optional<std::unique_ptr<Event>> handle(Club& club) override;
        private:
            std::string client_name_;
        };
        
        class ClientLeaving : public Event {
        public:
            ClientLeaving(Timestamp time, std::string client_name);
            std::string to_string() const override;
            std::optional<std::unique_ptr<Event>> handle(Club& club) override;
        private:
            std::string client_name_;
        };
    }
    
    namespace outEvents{
        
        class DepartClients : public Event {
        public:
            DepartClients(Timestamp time, std::string client_name);
            std::string to_string() const override;
            std::optional<std::unique_ptr<Event>> handle(Club& club) override;
        private:
            std::string client_name_;
        };
        
        class MoveClient : public Event {
        public:
            MoveClient(Timestamp time, std::string client_name, int table_id);
            std::string to_string() const override;
            std::optional<std::unique_ptr<Event>> handle(Club& club) override;
        private:
            std::string client_name_;
            int table_id_;
        };
        
        class Error : public Event {
        public:
            Error(Timestamp time, std::string error_message);
            std::string to_string() const override;
            std::optional<std::unique_ptr<Event>> handle(Club& club) override;
        private:
            std::string error_message_;
        };
    }
}