set(CLIENT_SRC
    client/client.cpp)

set(SERVER_SRC
    server/main.cpp)

# If you want to create a library you could replace the following
# add_executable with add_library
add_executable(client ${CLIENT_SRC})
add_executable(server ${SERVER_SRC})
