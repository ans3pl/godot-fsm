#include "fsm.h"



FSM::FSM()
{

}

void FSM::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY: {
            set_process_internal(true);

            State* fs = Object::cast_to<State>(get_node(first_state));
            current_state=fs;
            transition=true;

            _create_blank_state_map();
            connect_state_signals();
        } break;

        case NOTIFICATION_INTERNAL_PROCESS: {
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
        } break;
    }
}

NodePath FSM::get_first_state(){
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

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "first state"), "set_first_state", "get_first_state");
}
