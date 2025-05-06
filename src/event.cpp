#include "..//include/event.hpp"
#include "..//include/club.hpp"
#include <cstdlib>
#include <string>
#include <iostream>

using namespace std::string_literals;

namespace club{

    Event::Event(Timestamp time, EventType type): time_(time), type_(type){}

    std::string Event::to_string() const{
        return(time_.to_string() + " "s + std::to_string(static_cast<int>(type_)) + " "s);
    }

    Timestamp Event::time() const noexcept {
        return time_;
    }
    EventType Event::type() const noexcept {
        return type_;
    }

    namespace inEvents{

    /// Client arrival in Event class 
        ClientArrival::ClientArrival(Timestamp time, std::string client_name)
            :Event(time, EventType::client_came), client_name_(std::move(client_name)) {}
        std::string ClientArrival::to_string() const {
            //at first wrote std::to_string(client_name_)...
            return Event::to_string() + client_name_;
        }

        std::optional<std::unique_ptr<Event>> ClientArrival::handle(Club& club) {
            //output in ostream; TODO: later write to output file
            std::cout<< this->to_string() << std::endl;

            //conitnuing with handling
            if(!(club.is_open(time_))) {
                return EventFactory::create_internal(time_, EventType::error, "NotOpenYet"s);
            }
            auto client = club.get_or_create_client(client_name_);
            if(!(client->state() == ClientState::outside)){
                return EventFactory::create_internal(time_, EventType::error, "YouShallNotPass"s);
            }
            client->arrive(time_);

            return std::nullopt;
        }
        
    /// Client seated in Event class 
        ClientSeated::ClientSeated(Timestamp time, std::string client_name, int table_id)
        : Event(time, EventType::client_seated), client_name_(std::move(client_name)), table_id_(table_id)  {}
        std::string ClientSeated::to_string() const {
            //at first wrote std::to_string(client_name_)...
            return Event::to_string() + client_name_ + " "s + std::to_string(table_id_);
        }

        std::optional<std::unique_ptr<Event>> ClientSeated::handle(Club& club){
            //output in ostream; TODO: later write to output file
            std::cout<< this->to_string() << std::endl;

            //continuing with handling
            if(table_id_ <= 0 || table_id_ > club.config_.table_amount) {
                //check just in case
                throw std::invalid_argument("Bad format of table id");
                // std::exit(EXIT_FAILURE);
            }

            auto client = club.get_or_create_client(client_name_);
            Table* table = club.get_table(table_id_);
            if(table == nullptr){
                //check just in case
                std::exit(EXIT_FAILURE);
            }

            if(client->state() == ClientState::outside){
                return EventFactory::create_internal(time_, EventType::error, "ClientUnknown"s);
            }
            if(!table->is_free() || (table->id() == client->get_table_id())){
                return EventFactory::create_internal(time_, EventType::error, "PlaceIsBusy");
            }
            if(client->state() == ClientState::seated){
                Table* current_table = club.get_table(client->get_table_id());
                current_table->release(time_);
                table->occupy(client, time_);
                return std::nullopt;
            }

            client->sit(table_id_, time_);
            table->occupy(client, time_);

            return std::nullopt;
        }
        
    /// Client waiting in Event class 
        ClientWaiting::ClientWaiting(Timestamp time, std::string client_name)
        : Event(time, EventType::client_waiting), client_name_(std::move(client_name)) {}
        std::string ClientWaiting::to_string() const {
            //at first wrote std::to_string(client_name_)...
            return Event::to_string() + client_name_;
        }

        std::optional<std::unique_ptr<Event>> ClientWaiting::handle(Club& club){
            //output in ostream; TODO: later write to output file
            std::cout<< this->to_string() << std::endl;

            //continuing with handling
            //we suggest that that command wont be called when Club is not open, but just in case lets check:
            if(!club.is_open(time_)){
                std::exit(EXIT_FAILURE);
            }
            //continuing with our Event
            auto client = club.get_or_create_client(client_name_);
            //suggesting that client can "wait" if he is in queue already
            if(client->state() == ClientState::waiting) return std::nullopt;
            
            if(!club.any_free_table()){
                if(!club.queue_valid()){
                    return EventFactory::create_internal(time_, EventType::lost_client, client_name_);
                }
                club.add_to_queue(time_, client_name_);
                client->wait(time_); //were embedded in add_to_queue() method, TODO: client state observer, event system or state mediator
                return std::nullopt;
            }
            return EventFactory::create_internal(time_, EventType::error, "ICanWaitNoLonger!");
        }   

    /// Client leaving in Event class 
        ClientLeaving::ClientLeaving(Timestamp time, std::string client_name)
        : Event(time, EventType::client_left), client_name_(std::move(client_name)) {}
        std::string ClientLeaving::to_string() const {
            //at first wrote std::to_string(client_name_)...
            return Event::to_string() + client_name_;
        }

        std::optional<std::unique_ptr<Event>> ClientLeaving::handle(Club& club) {
            //output in ostream; TODO: later write to output file
            std::cout<< this->to_string() << std::endl;

            //continuing with handling
            //we also suggest that that command wont be called when Club is not open, but just in case lets check:
             if(!club.is_open(time_)){
                std::exit(EXIT_FAILURE);
            }
            //continuing with our Event
            auto client = club.get_or_create_client(client_name_);
            if(client->state() == ClientState::outside){
                return EventFactory::create_internal(time_, EventType::error, "ClientUnknown"s);
            }
            if(client->state() == ClientState::seated){
                int table_id = client->get_table_id();
                auto table = club.get_table(client->get_table_id());
                table->release(time_);
                client->leave(time_);       
                if(club.any_waiting()){
                    return EventFactory::create_internal(time_, EventType::moved_client, club.remove_from_queue(), table_id);
                }     
            }

            client->leave(time_);

            return std::nullopt;
        }

    }

    namespace outEvents{

    /// DepartClients out Event class
        DepartClients::DepartClients(Timestamp time, std::string client_name)
            : Event(time, EventType::lost_client), client_name_(std::move(client_name)) {}
        std::string DepartClients::to_string() const {
            //at first wrote std::to_string(client_name_), man...
            return Event::to_string() + client_name_;
        }

        std::optional<std::unique_ptr<Event>> DepartClients::handle(Club& club) {
            //output in ostream; TODO: later write to output file
            std::cout<< this->to_string() << std::endl;

            //continuing with handling
            if( club.is_open(time_)){
                auto client = club.get_or_create_client(client_name_);
                client->leave(time_);
            }
            //dedkljfsd

            return std::nullopt;
        }    

    /// MoveClient out Event class
        MoveClient::MoveClient(Timestamp time, std::string client_name, int table_id)
            : Event(time, EventType::moved_client), client_name_(std::move(client_name)), table_id_(table_id) {}
        std::string MoveClient::to_string() const {
            return Event::to_string() + client_name_ + " "s + std::to_string(table_id_);
        }
        
        std::optional<std::unique_ptr<Event>> MoveClient::handle(Club& club) {
            //output in ostream; TODO: later write to output file
            std::cout<< this->to_string() << std::endl;

            //continuing with handling
            auto client = club.get_or_create_client(client_name_);
            auto table = club.get_table(table_id_);
            
            table->occupy(client, time_);
            client->sit(table_id_, time_);

            return std::nullopt;
        }

    /// Error out Event class
        Error::Error(Timestamp time, std::string error_message)
            : Event(time, EventType::error), error_message_(std::move(error_message)) {}
        std::string Error::to_string() const {
            return Event::to_string() + error_message_;
        }

        std::optional<std::unique_ptr<Event>> Error::handle(Club& club) {
            //output in ostream; TODO: later write to output file
            std::cout<< this->to_string() << std::endl;

            //continuing with handling
            //nothing much to do

            return std::nullopt;
        }

    }
}
