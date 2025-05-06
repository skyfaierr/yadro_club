#include "..//include/club.hpp"
#include <algorithm>
#include <memory>

namespace club {
    /// Constructor of Club in club namespace :)
    Club::Club(const ClubConfig& cfg): config_(std::move(cfg)) {
        for (int id = 1; id <= config_.table_amount; ++id){
            tables_.emplace(
                id, std::make_shared<Table>(id, config_.hourly_rate)
            );
        }
    };

    std::shared_ptr<Client> Club::get_or_create_client(const std::string& name){
        auto it = clients_.find(name);
        //return existing client
        if (it != clients_.end()){
            return it->second; 
        }
        //create new client and put that mf in map
        auto client = std::make_shared<Client>(name);
        clients_[name] = client;
        //than throw him in state map 
        // clients_by_state_[client->state()].insert(name);

        return client;
    };
    Table* Club::get_table(int id){
        //trivial id search; returns nullptr if not found
        auto it = tables_.find(id);
        return (it != tables_.end() ? it->second.get() : nullptr);
    };
    bool Club::any_free_table() const {
        return std::any_of(tables_.begin(), tables_.end(), 
            [](const auto& pair) {
                return pair.second->is_free();
            });
    };
    bool Club::is_open(const Timestamp& time) const {
        return (time >= config_.opening_time && time < config_.closing_time);
    };

    void Club::add_to_queue(Timestamp time, std::string& name){
        waiting_list_.push(name);

        //find client
        auto client = get_or_create_client(name);
        ClientState old_state = client->state();
        // clients_by_state_[old_state].erase(name);
        // clients_by_state_[ClientState::waiting].insert(name);
        // client->wait(time); // TODO: see ClientWaiting Event
    };
    std::string Club::remove_from_queue(){
        std::string name = waiting_list_.front();
        waiting_list_.pop();
        // clients_by_state_[ClientState::waiting].erase(name);
        // clients_by_state_[ClientState::seated].insert(name);
        return name;
    };
    bool Club::any_waiting() const noexcept{
        if(waiting_list_.empty()){
            return false;
        }
        return true;
    }
    bool Club::queue_valid() const noexcept{
        if(waiting_list_.size() < tables_.size()){
            return true;
        }
        return false;
    }

    // std::vector<Event> ?? ret value
    std::vector<std::unique_ptr<Event>> Club::depart_all_clients(Timestamp time){
        std::vector<std::unique_ptr<Event>> departure_events;
        //getting every soul in the club through present_clients
        std::set<std::string> present_clients; 
        
        if(!is_open(time)){
            for(auto& [id, table] : tables_){
                if(table->is_free()) continue;

                auto client = table->get_client();
                client->leave(time);
                table->release(time);
                present_clients.insert(client->name());
            }
            
            while(!waiting_list_.empty()){
                auto name = waiting_list_.front();
                waiting_list_.pop();
                present_clients.insert(name);
            }
            
            for (const auto& [name, client] : clients_) {
                if (client->state() == ClientState::inside) {
                    present_clients.insert(name);
                }
            }
        }

        std::vector<std::string> sorted_names(present_clients.begin(), present_clients.end());
        std::sort(sorted_names.begin(), sorted_names.end());

        for(const auto& name : sorted_names){
            auto event = EventFactory::create_internal(time, EventType::lost_client, name);            
            departure_events.push_back(std::move(event));
        }

        return departure_events;
    }

    int Club::hourly_rate() const noexcept{
        return this->config_.hourly_rate;
    };
    Timestamp Club::open_time() const noexcept{
        return this->config_.opening_time;
    };
    Timestamp Club::close_time() const noexcept{
        return this->config_.closing_time;
    };

}
