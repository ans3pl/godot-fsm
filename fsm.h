#ifndef FSM_H
#define FSM_H

#include "scene/main/node.h"
#include "core/dictionary.h"

#include "state.h"
#include "node_path.h"

class FSM : public Node {
    GDCLASS(FSM, Node);

    bool transition = false;

protected:
    static void _bind_methods();


public:
    StringName get_state_name(){
        return current_state->call("get_name");
    }

    void _notification(int p_what);

    void _create_blank_state_map(void){
        for (int c = 0; c < get_child_count(); ++c){
            Node* node = get_child(c);
            if(node != NULL){
                State* state = node->cast_to<State>(node);
                if(state != NULL){
                    add_state(state);
                }
            }
        }
    }
    void connect_state_signals(void){
        Array states = state_map.keys();
        for(int i = 0; i < states.size(); ++i){
            Node* node = states[i];
            State* state = node->cast_to<State>(node);
            state->connect("request_connection", this, "change_state");
        }
    }

    void change_state(String connection){
        transition=true;
        previous_state = current_state;

        Dictionary sm = state_map[current_state];
        Node* n = sm[connection];
        current_state = Object::cast_to<State>(n);
    }

    void connect_state(Node* source, Node* destination, String connection){
        Dictionary sm = state_map[source];
        sm[connection] = Object::cast_to<Node>(destination);
        state_map[source] = sm;
    }

    void add_state(State* s){
        PoolStringArray connections = s->get_connections();
        Node* n = Object::cast_to<Node>(s);

        Dictionary con = state_map[n];
        for (int i=0; i<connections.size(); ++i){
            if(!con.has(connections[i])){
                con[connections[i]]=NULL;
            }
        }
        state_map[n] = con;
    }

    NodePath get_first_state();
    void set_first_state(const NodePath &path);

    FSM();

    Dictionary state_map;
    State* previous_state = nullptr;
    State* current_state = nullptr;
    NodePath first_state;
};

#endif // FSM_H
