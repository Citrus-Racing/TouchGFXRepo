/*
 * CR_structs.h
 *
 * Assorted structs to help contain the mayhem
 */

#ifndef STRUCTS_CR_STRUCTS_H_
#define STRUCTS_CR_STRUCTS_H_


// Holds the GPIO port and pin number.
typedef struct {
	GPIO_TypeDef * GPIO_Port; // GPIOA, GPIOB...
	uint16_t GPIO_Pin; // GPIO_PIN_5, GPIO_PIN_6...
} CR_GPIO;

typedef enum {
	BUTTON_RELEASED,
	BUTTON_PRESSED,
	BUTTON_HOLD,
} CR_button_state;

#endif /* STRUCTS_CR_STRUCTS_H_ */
