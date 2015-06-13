const unsigned int C = 7;
const unsigned int B = 4;
const unsigned int A = 2;
const unsigned int POTS = A0;
const unsigned int NUM_JOINTS = 5;
const float J0 = 170/2;
const float J1 = 170/2;
const float J2 = 170/2;
const float J3 = 170/2;
const float J4 = 45/2;

const float P0_LOWER = 0;
const float P0_UPPER = 0;
const float P1_LOWER = 140;
const float P1_UPPER = 600;
const float P2_LOWER = 50;
const float P2_UPPER = 700;
const float P3_LOWER = 0;
const float P3_UPPER = 0;
const float P4_LOWER = 240;
const float P4_UPPER = 420;

int pot_val = 0;

void muxDigitalWrite(const unsigned int& c, const unsigned int& b, const unsigned int& a) {
  digitalWrite(C, c);
  digitalWrite(B, b);
  digitalWrite(A, a);
}

// MUX: MAX4617CPE
// C B A (A is LSB)
void setMux(const unsigned int& pot_num) {
  switch(pot_num) {
    case 0:
      muxDigitalWrite(0,0,0);
      break;
    case 1:
      muxDigitalWrite(0,0,1);
      break;
    case 2:
      muxDigitalWrite(0,1,0);
      break;
    case 3:
      muxDigitalWrite(0,1,1);
      break;
    case 4:
      muxDigitalWrite(1,0,0);
      break;
    default:
      Serial.println("Invalid pot selection");
  }
}

float getPotValue(const unsigned int& pot_num) {
  setMux(pot_num);
  return analogRead(POTS);
}

float getJointAngle(const unsigned int& joint_num) {
  float joint_val = getPotValue(joint_num);
  switch (joint_num) {
    case 0:
      joint_val = map(joint_val, P0_LOWER, P0_UPPER, J0, -J0);
      break;
    case 1:
      joint_val = map(joint_val, P1_LOWER, P1_UPPER, J1, -J1);
      break;
    case 2:
      joint_val = map(joint_val, P2_LOWER, P2_UPPER, J2, -J2);
      break;
    case 3:
      joint_val = map(joint_val, P3_LOWER, P3_UPPER, J3, -J3);
      break;
    case 4:
      joint_val = map(joint_val, P4_LOWER, P4_UPPER, J4, -J4);
      break;   
  }
  return joint_val; 
}

void getJointAngle(float* joints) {
  for (unsigned int i = 0; i < NUM_JOINTS - 1; ++i) {
    joints[i] = getJointAngle(i);
  } 
}

void setup() {
  Serial.begin(9600);
  pinMode(C, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(A, OUTPUT);
}

void loop() {
  Serial.println("Select a joint (0-4)");
  while (Serial.available() == 0);
  {
    int joint = Serial.parseInt();
    Serial.println(getJointAngle(joint));
  }
  delay(1000);
  
}
