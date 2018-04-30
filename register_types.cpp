#include "register_types.h"
#include "fsm.h"
#include "state.h"

void register_fsm_types() {
    ClassDB::register_class<FSM>();
    ClassDB::register_class<State>();
}

void unregister_fsm_types() {}
