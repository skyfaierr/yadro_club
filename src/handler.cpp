#include "..//include/handler.hpp"
#include "..//include/club.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

namespace club{

/// Regex's for validation
    static const std::regex valid_time(R"((0[0-9]|1[0-9]|2[0-3]):[0-5][0-9])"); 
    static const std::regex valid_name(R"([a-z0-9_-]+)");
    static const std::regex valid_line(R"(^\d{2}:\d{2} \d{1,2}( .+)?$)");


/// EventHandler and his methods

    EventHandler::EventHandler(const std::string& input_file):input_file_(input_file) {}

    void EventHandler::run(){
        //trying to load our file
        try{
            load_file();
        } catch(std::exception& e) {
            //std::cerr<< e.what() << std::endl;
            return;
        }
        std::cout<< club_->config_.opening_time << std::endl;
        //trying to process our events continiously
        bool day_finished = false;
        try{
            for(auto& ev : events_){
                if(ev->time() == club_->config_.closing_time){
                    auto depart_events = club_->depart_all_clients(ev->time());
                    for(auto& dep_ev : depart_events){
                        process_event(std::move(dep_ev));
                    }
                    day_finished = true;
                    break;
                }
                process_event(std::move(ev));
            }
        } catch(std::exception& e) {
            std::cerr<< e.what() << std::endl;
        }
        if(!day_finished){
            auto depart_events = club_->depart_all_clients(club_->config_.closing_time);
            if(!depart_events.empty()) {
                for(auto& dep_ev : depart_events){
                    process_event(std::move(dep_ev));
                }   
            }
        }
        //printing overall log of the day
        std::cout<< club_->config_.closing_time << std::endl;
        print_report();
    }

    void EventHandler::load_file(){
    /// Opening our file
        std::ifstream in(input_file_, std::ios_base::in);
        if(!in.is_open()){
            throw std::runtime_error("Failed to open your file: " + input_file_);
        }

    /// First, second and third lines parsing
        // first line (@var table_amount for our config)
        std::string line; // buffer for each line
        std::getline(in,line);
        if(!std::regex_match(line, std::regex(R"(\d+)"))){
            std::cout<< line << std::endl;
            throw std::runtime_error("Bad format at line 1");
        }
        int table_amount = std::stoi(line);

        //second line (@var opeinng_time and @var closing_time for our config)
        std::getline(in, line);
        if(!std::regex_match(line, std::regex(R"(\d{2}:\d{2} \d{2}:\d{2})"))){
            std::cout<< line << std::endl;
            throw std::runtime_error("Bad format at line 2");
        }
        std::istringstream iss(line);
        std::string open_str, close_str;
        iss >> open_str >> close_str;
        if (!std::regex_match(open_str, valid_time) || !std::regex_match(close_str, valid_time)){
            std::cout<< line << std::endl;
            throw std::runtime_error("Bad time format at line 2");
        }
        if (open_str == close_str){
            std::cout<< line << std::endl;
            throw std::runtime_error("Equal opening and closing time");
        }
        club::Timestamp open_time(open_str);
        club::Timestamp closing_time(close_str);

        //third line (@var hourly_rate for our config)
        std::getline(in, line);
        if(!std::regex_match(line, std::regex(R"(\d+)"))){
            std::cout<< line << std::endl;
            throw std::runtime_error("Bad format at line 3");
        }
        int hourly_rate = std::stoi(line);
        
    /// Initializing our Club entity via config    
        ClubConfig config{table_amount, hourly_rate, open_time, closing_time};
        club_ = std::make_unique<Club>(config);

    /// Parsing leftover commands
        std::string command_line;
        int line_num = 4; //hardcoded num will be sufficient enough
        Timestamp previous_time(0,0);
        while(std::getline(in, command_line)){
            if(command_line.empty()) continue;
            try{
                auto event = EventFactory::create_from_input(command_line);
                if(event->time() < previous_time) throw std::invalid_argument("Time sequence break: " + command_line);
                previous_time = event->time();
                events_.push_back(std::move(event));
            } catch(const std::exception& e) {
                std::cout<< command_line << std::endl;
                //std::exit(EXIT_FAILURE);
                throw; //throw to stop run() in next catch sequence
            }
            ++line_num;
        }
    }

    void EventHandler::process_event(std::unique_ptr<Event> event){
        if(auto next_event = event->handle(*club_)){
            // std::cout<< event->to_string() << std::endl;
            process_event(std::move(*next_event));
        }
    };

    void EventHandler::finilize_day(){
        // TODO: free clients and depart queue
    };
    void EventHandler::print_report() const{
        for(auto [id, table] : club_->tables_){
            std::cout<< table->id() << " " << table->revenue() << " " << table->occupied_duration() << std::endl;
        }
        // TODO: also write in output file
    };



/// EventFactory shines through

    std::unique_ptr<Event> EventFactory::create_from_input(const std::string& line) {
        //first naive line validation 
        if(!std::regex_match(line, valid_line)){
            throw std::runtime_error("Bad format: " + line);
        }

        //buffer variables
        std::istringstream iss(line);
        std::string time_str;
        unsigned short type_id;
        std::string client_name;
        int table_id = -1;

        //read time and type of Event
        if(!(iss >> time_str >> type_id >>client_name)){
            throw std::invalid_argument("Bad format in: " + line);
        }
        //check time format
        if(!std::regex_match(time_str, valid_time)){
            throw std::invalid_argument("Bad time format in: " + line);
        }
        //check id format
        if(type_id > 4 || type_id < 1){
            throw std::invalid_argument("Forbidden input command id in: " + line);
        }
        //check name format
        if(!std::regex_match(client_name, valid_name)){
            throw std::invalid_argument("Bad name format in:" + line);
        }

        Timestamp time(time_str);
        auto type = static_cast<EventType>(type_id);

        switch(type){
            case EventType::client_came:        // ID 1
            case EventType::client_waiting:     // ID 3
            case EventType::client_left: {      // ID 4
                if(type == EventType::client_came) {
                    return std::make_unique<inEvents::ClientArrival>(time, client_name);
                }
                if(type == EventType::client_waiting) {
                    return std::make_unique<inEvents::ClientWaiting>(time, client_name);
                }
                return std::make_unique<inEvents::ClientLeaving>(time, client_name);
            }
            case EventType::client_seated:{
                if(!(iss >> table_id) || table_id <= 0){
                    throw std::invalid_argument("Bad format in: " + line);
                }
                return std::make_unique<inEvents::ClientSeated>(time, client_name, table_id);
            }
            default:
                throw std::invalid_argument("Unhandled input event type: " + std::to_string(static_cast<int>(type)));
        }

    }

    //string is ambiguous - can be client_name or error_message
    std::unique_ptr<Event> EventFactory::create_internal(Timestamp time, EventType type, const std::string& client_or_message, int table_id){
        switch(type){
            case club::EventType::lost_client:
                return std::make_unique<club::outEvents::DepartClients>(time, client_or_message);
            case club::EventType::moved_client:
                return std::make_unique<club::outEvents::MoveClient>(time, client_or_message, table_id);
            case club::EventType::error:
                return std::make_unique<club::outEvents::Error>(time, client_or_message);
            default:
                throw std::invalid_argument("Internal factory does not create event type: " + std::to_string(static_cast<int>(type)));
        }
    };

}
