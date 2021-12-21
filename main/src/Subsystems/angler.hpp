#include "../Libraries/subsystem.hpp"
#define NUM_OF_ANGLER_STATES 4

enum class angler_states {
  lowered,
  searching,
  raised,
  lowering
};

extern angler_states angler_state;
extern angler_states last_angler_state;

extern int angler_encoder_position;

extern const int angler_bottom_position;
extern const int angler_raised_position;
extern const int angler_top_position;

extern std::array<const char*, NUM_OF_ANGLER_STATES> angler_state_names;

void set_angler_state();
void angler_handle();
void angler_reset();
