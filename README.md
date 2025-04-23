# EEET2490-assignment-3
🧠 Convention Case
To maintain readability and consistency across the codebase, the following naming conventions are used throughout this project:

✅ Function Names
Use snake_case

Lowercase letters with underscores between words

Example:
void init_gpio(void);
int read_sensor_data(void);
✅ Macros and #define
Use ALL_CAPS with underscores between words

Typically used for constants, hardware register addresses, or configuration values

Example:
#define SYSTEM_CLOCK_HZ   16000000
#define LED_PIN           5
These conventions align with common practices in C, especially in embedded and bare-metal environments, making code more readable and predictable for collaborators and future maintenance.
