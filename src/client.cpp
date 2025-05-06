#include "..//include/client.hpp"

club::Client::Client(std::string name):name_(name) {}

std::string club::Client::name() const noexcept {
    return name_;
}
club::ClientState club::Client::state() const noexcept {
    return this->state_;
};

// command ID 1
void club::Client::arrive(const Timestamp& time) {
    this->state_ = ClientState::inside;
};     
// command ID 2\12    
void club::Client::sit(int table_id, const Timestamp& time){
    this->state_ = ClientState::seated;
    table_id_ = table_id;
};      
// command ID 3   
void club::Client::wait(const Timestamp& time){
    this->state_ = ClientState::waiting;
};         
// command ID 4\11
void club::Client::leave(const Timestamp& time){
    this->state_ = ClientState::outside;
    table_id_ = 0;
};         

int club::Client::get_table_id() const noexcept {
    return table_id_;
}
void club::Client::add_charge(int charge, int hours){
    this->total_charge_ += charge;
    this->total_hours_round_ += hours;
};         
int club::Client::total_charge() const noexcept{
    return this->total_charge_;
};