cmake_minimum_required(VERSION 3.10)
project(MyServer)
set(CMAKE_CXX_STANDARD 17)


file (GLOB tt_sources ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(server server.cpp ${tt_sources} ${PROJECT_SOURCE_DIR}/src/LogBlockQueue.h)
target_link_libraries(server -pthread -lpthread -lmysqlclient -lz -lm -lrt -latomic -lssl -lcrypto -ldl -ljsoncpp)

add_executable(multi_client multi_client.cpp ${tt_sources} ${PROJECT_SOURCE_DIR}/src/LogBlockQueue.h)
target_link_libraries(multi_client -pthread -lpthread -lmysqlclient -lz -lm -lrt -latomic -lssl -lcrypto -ldl -ljsoncpp)



# add_executable(Timer_demo src/Timer.h src/Timer.cpp)

# add_subdirectory(MySQL)



# add_executable(timer_test Timer_test.cpp src/Timer.cpp)

# add_executable(logqueue_test LogQueue_test.cpp  ${tt_sources} ${PROJECT_SOURCE_DIR}/src/LogBlockQueue.h)
# target_link_libraries(logqueue_test -pthread -lpthread -lmysqlclient -lz -lm -lrt -latomic -lssl -lcrypto -ldl -ljsoncpp )
