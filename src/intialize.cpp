#include "main.h"

bool auton_mode = false;

bool mirror = false;

//*********************************LCD SCREEN

void initialize() {
pros::lcd::initialize();
pros::lcd::set_text(1, "Tissan is Awesome!");
pros::lcd::set_text(2, "RIP 356A");
auton_mode = false;
mirror = false;
}

void disabled() {
auton_mode = false;
mirror = false;
}

void competition_initialize() {
auton_mode = false;
mirror = false;
}
