#include <iostream>
#include <chrono>
#include <fstream>

#include "..//include/handler.hpp"

int main(int argc, char* argv[]){
/// Starts timer of execution.
    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();

/// No second argument handling
    if (argc < 2) {
        std::cerr << "[Usage:] " << argv[0] << " <filename.txt>" << std::endl;
        return 1;
    }

    /*
    ----- Считываю test.txt:
    <количество столов в компьютерном клубе>
    <время начала работы> <время окончания работы>
    <стоимость часа в компьютерном клубе>
    <время события 1> <идентификатор события 1> <тело события 1>
    <время события 2> <идентификатор события 2> <тело события 2>
    ...
    <время события N> <идентификатор события N> <тело события N>

    ----- После этого нужно обрабатывать события, имеющии время и привязанные к ID события
    */
    
// Mainframe of programm

    std::ofstream file("output.txt");
    std::streambuf* cout_buffer = std::cout.rdbuf();
    std::cout.rdbuf(file.rdbuf());
    try{
        club::EventHandler handler(argv[1]);
        handler.run();
    }
    catch(const std::exception& e){
        //std::cerr<< "[Error:] " << e.what() << std::endl;
        return 1;
    }
    catch(...){
        //std::cerr<<"[Error:] Not defined" << std::endl;
    }
    std::cout.rdbuf(cout_buffer); 

// Outputs duration of an execution.
    auto end = clock::now();
    std::chrono::duration<double> elapsed = end - start;
    //std::cout << "Execution time: " << elapsed.count() << " seconds." << std::endl; 
}