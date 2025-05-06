#include "..//include/table.hpp"

club::Table::Table(int id, int hourly_rate) noexcept: id_(id), hourly_rate_(hourly_rate) {};

int club::Table::id() const noexcept {
    return id_;
}
bool club::Table::is_free() const noexcept {
    return free_;
}

void club::Table::occupy(std::shared_ptr<Client> client, Timestamp time){
    current_client_ = client;
    last_occupied_time_ = time;
    free_ = false;
};
void club::Table::release(Timestamp time){
    if (!current_client_) return;
    // Intermediate computations
    Timestamp occupied_time = (time - last_occupied_time_);
    int charge = hourly_rate_ * (occupied_time.round());
    total_revenue_ += charge;
    total_occupied_time_ += occupied_time;
    // Adding charge to client
    current_client_->add_charge(charge, occupied_time.round());
    // Updating current state
    last_occupied_time_ = time;
    free_ = true;
    current_client_.reset();
};
std::shared_ptr<club::Client> club::Table::get_client() {
    return current_client_;
};


int club::Table::revenue() const noexcept{
    return total_revenue_;
};
club::Timestamp club::Table::occupied_duration() const noexcept{
    return total_occupied_time_;
};