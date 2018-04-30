#include "fsm.h"



FSM::FSM()
{

}

FSM::FSMProcessMode FSM::get_fsm_process_mode() const {
    return fsm_process_mode;
}
void FSM::set_fsm_process_mode(FSMProcessMode p_mode) {

    if (fsm_process_mode == p_mode)
        return;

    switch (fsm_process_mode) {
        case FSM_PROCESS_PHYSICS:
            if (is_physics_processing_internal()) {
                set_physics_process_internal(false);
                set_process_internal(true);
            }
            break;
        case FSM_PROCESS_IDLE:
            if (is_processing_internal()) {
                set_process_internal(false);
                set_physics_process_internal(true);
            }
            break;
    }
    fsm_process_mode = p_mode;
}
void FSM::_set_process(bool p_process) {
    switch (fsm_process_mode) {
        case FSM_PROCESS_PHYSICS: set_physics_process_internal(p_process && !paused); break;
        case FSM_PROCESS_IDLE: set_process_internal(p_process && !paused); break;
    }
    processing = p_process;
}

void FSM::_create_blank_state_map(void){
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

void FSM::_connect_state_signals(void){
    Array states = state_map.keys();
    for(int i = 0; i < states.size(); ++i){
        Node* node = states[i];
        State* state = node->cast_to<State>(node);
        state->connect("request_connection", this, "change_state");
    }
}

void FSM::change_state(String connection){
    transition=true;
    previous_state = current_state;

    Dictionary sm = state_map[current_state];
    Node* n = sm[connection];
    current_state = Object::cast_to<State>(n);
}

StringName FSM::get_state_name(){
    return current_state->call("get_name");
}

void FSM::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY: {
            _set_process(true);

            State* fs = Object::cast_to<State>(get_node(first_state));
            current_state=fs;
            transition=true;

            _create_blank_state_map();
            _connect_state_signals();
        } break;

        case NOTIFICATION_INTERNAL_PROCESS: {
            if(fsm_process_mode!=FSM_PROCESS_IDLE)
                return;

            _update();
        } break;

        case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
            if(fsm_process_mode!=FSM_PROCESS_PHYSICS)
                return;

            _update();
        } break;
    }
}

void FSM::connect_state(Node* source, Node* destination, String connection){
    Dictionary sm = state_map[source];
    sm[connection] = Object::cast_to<Node>(destination);
    state_map[source] = sm;
}

void FSM::add_state(State* s){
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

void FSM::_update() {
    Variant dt = get_process_delta_time();
    if(transition){
        transition = false;
        if(previous_state!=nullptr){
            if(previous_state->get_script_instance()){
                previous_state->get_script_instance()->call("_state_exit");
            }
        }
        previous_state=current_state;

        if(current_state!=nullptr){
            if(current_state->get_script_instance()){
                current_state->get_script_instance()->call("_state_enter");
            }
        }
    }
    if(current_state){
        if(current_state->get_script_instance()){
            current_state->get_script_instance()->call("_state_update", dt);
        }
    }
}

NodePath FSM::get_first_state() const{
    return first_state;
}

void FSM::set_first_state(const NodePath &path){
    first_state = path;
}

void FSM::_bind_methods() {

    ClassDB::bind_method(D_METHOD("connect_state", "source", "destination", "connection"), &FSM::connect_state);
    ClassDB::bind_method(D_METHOD("change_state", "connection"), &FSM::change_state);


    ClassDB::bind_method(D_METHOD("set_first_state", "state"), &FSM::set_first_state);
    ClassDB::bind_method(D_METHOD("get_first_state"), &FSM::get_first_state);

    ClassDB::bind_method(D_METHOD("set_fsm_process_mode", "process_mode"), &FSM::set_fsm_process_mode);
    ClassDB::bind_method(D_METHOD("get_fsm_process_mode"), &FSM::get_fsm_process_mode);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "first state"), "set_first_state", "get_first_state");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "process_mode", PROPERTY_HINT_ENUM, "Fixed,Idle"), "set_fsm_process_mode", "get_fsm_process_mode");
}
