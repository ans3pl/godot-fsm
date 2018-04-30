#ifndef STATE_H
#define STATE_H

#include "scene/main/node.h"
#include "core/pool_allocator.h"
#include "core/variant.h"

#include <iostream>

class State : public Node {
    GDCLASS(State, Node);

protected:
    static void _bind_methods(){
        ADD_SIGNAL(MethodInfo("request_connection", PropertyInfo(Variant::STRING,"connection_name")));

        ClassDB::bind_method(D_METHOD("add_connection", "connection_name"), &State::add_connection);
        ClassDB::bind_method(D_METHOD("call_connected_state", "connection_name"), &State::call_connected_state);

        ClassDB::bind_method(D_METHOD("set_connections", "connections"), &State::set_connections);
        ClassDB::bind_method(D_METHOD("get_connections"), &State::get_connections);

        ADD_PROPERTY(PropertyInfo(Variant::POOL_STRING_ARRAY, "connections"), "set_connections", "get_connections");
    };

public:
    void set_connections(PoolStringArray connections){
        this->connections=connections;
    }
    PoolStringArray get_connections(){
        return connections;
    }

    void call_connected_state(String connection_name){
        Error error = emit_signal("request_connection", connection_name);
    }

    void add_connection(String connection_name){
        connections.push_back(connection_name);
    }

    PoolStringArray connections;

    State(){

    }
};

#endif // STATE_H
