#ifndef FSM_H
#define FSM_H

#include "scene/main/node.h"
#include "core/dictionary.h"

#include "state.h"
#include "node_path.h"

class FSM : public Node {
    GDCLASS(FSM, Node);

    bool transition = false;
    bool processing = false;
    bool paused;

protected:
    static void _bind_methods();

public:
    enum FSMProcessMode {
        FSM_PROCESS_PHYSICS,
        FSM_PROCESS_IDLE
    };

    StringName get_state_name();
    void add_state(State* s);

    void change_state(String connection);
    void connect_state(Node* source, Node* destination, String connection);

    void _notification(int p_what);

    NodePath get_first_state() const;
    void set_first_state(const NodePath &path);

    FSMProcessMode get_fsm_process_mode() const;
    void set_fsm_process_mode(FSMProcessMode p_mode);

    FSM();

private:
    Dictionary state_map;
    State* previous_state = nullptr;
    State* current_state = nullptr;
    NodePath first_state;
    FSMProcessMode fsm_process_mode;


    void _set_process(bool p_process);
    void _update();
    void _create_blank_state_map(void);
    void _connect_state_signals(void);
};

VARIANT_ENUM_CAST(FSM::FSMProcessMode);

#endif // FSM_H
