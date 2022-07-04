

add_executable(mysql_api_test mysql_api_test.cpp)
target_link_libraries(mysql_api_test -lmysqlclient -lpthread -lz -lm -lrt -latomic -lssl -lcrypto -ldl)

add_executable(mysql_conn_pool connpool_test.cpp ConnPool.cpp)
target_link_libraries(mysql_conn_pool 
    -lmysqlclient -lpthread -lz -lm -lrt -latomic -lssl -lcrypto -ldl
    -ljsoncpp
    )

