#include <can.h>
#include <mcp2515.h>

MCP2515 CANController(10); // Create CANController using 10th digital pin as CS

void setup() {
  /* Initialise serial */
  Serial.begin(9600);
  Serial.println("Initialised serial");
  /* Initialise mcp2515 CAN */
  Serial.println("Initialising CANController");
  CANController.reset();
  CANController.setBitrate(CAN_500KBPS, MCP_8MHZ);
  CANController.setNormalMode();
  Serial.println("Initialised CANController");
}

void sendCANMsg(int rpm) {
  if (rpm > 255) { rpm = 255; }
  if (rpm < 0) { rpm = 0; }

  can_frame escMsg;
  escMsg.can_id = 158; // ESC can id
  escMsg.can_dlc = 8;
  escMsg.data[0] = 0;
  escMsg.data[1] = 0;
  escMsg.data[2] = 0;
  escMsg.data[3] = 0;
  escMsg.data[4] = 1; // ESCEnable
  escMsg.data[5] = 100; // ESCCurrentLimit
  escMsg.data[6] = rpm; // ESCSpeedRequest
  escMsg.data[7] = 0; // ESCSpeedRequest 2
  CANController.sendMessage(&escMsg);
  Serial.print("rpm:");
  Serial.println(rpm);
  
  can_frame pmzMsg;
  pmzMsg.can_id = 417; // PMZ can id
  pmzMsg.can_dlc = 8;
  pmzMsg.data[0] = 0;
  pmzMsg.data[1] = 0;
  pmzMsg.data[2] = (rpm == 0) ? 0 : 56; // desired PowerMode * 8
  pmzMsg.data[3] = 0;
  pmzMsg.data[4] = 0;
  pmzMsg.data[5] = 0;
  pmzMsg.data[6] = 0;
  pmzMsg.data[7] = 0;
  CANController.sendMessage(&pmzMsg);  
}

logCANMessages() {
  if (CANController.readMessage(&CANMsg) == MCP2515::ERROR_OK) {
    Serial.print("CAN message recieved!\nID: ");
    Serial.println(CANMsg.can_id); //id of where the message came from
    Serial.print("LENGTH: ");
    Serial.println(CANMsg.can_dlc); //length of message in bytes
    // get first 8 bytes for testing (would do for loop over length from start point of message wanted in future)
    Serial.println(CANMsg.data[0]); 
    Serial.println(CANMsg.data[1]);
    Serial.println(CANMsg.data[2]);
    Serial.println(CANMsg.data[3]);
    Serial.println(CANMsg.data[4]);
    Serial.println(CANMsg.data[5]);
    Serial.println(CANMsg.data[6]);
    Serial.println(CANMsg.data[7]);
  }
}
void loop() {
  int rpm = (int)((float)(analogRead(A1)*0.2492668622f)); // read value from potentiometer (0->255)
  sendCANMsg(rpm);
}
