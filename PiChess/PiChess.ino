#define STEPS 200 // per rev

#define PLAY_AS_WHITE 0

// linear motion
const double mm_step_xy = 0.2028;                        // mm per step (1.8 deg, radius of pulley = 6.75mm
const double mm_step_z = 8.3/STEPS;                     // mm per step (8.3mm/rev)
const double square_size = 57;                          // chess square size (mm)
const double vertical_steps = 99/mm_step_z;          // 100 mm in Z-direction

// home offset to a8/h1 square (black/white)
const double xhome_offset = 63;                         // 55mm offset
const double yhome_offset = 71;                         // 70mm offset

#define CLAW_STEPS 180
#define REBOUND_STEPS_XY 20
#define REBOUND_STEPS_Z 40
#define REBOUND_STEPS_CLAW 6

#define DEBOUNCE_MS 30
#define XY_DELAY 1000 // microseconds
#define Z_DELAY 700   // microseconds
#define CLAW_DELAY 600

#define MC_DIR 2 // claw
#define MC_STEP 3
#define MC_SLEEP 4

#define MZ_DIR 5 // Z motion lead screw
#define MZ_STEP 6
#define MZ_SLEEP 7

#define MY_DIR 23 // Y axis motor
#define MY_STEP 25
#define MY_SLEEP 27

#define MX1_DIR 31 // X axis motor (origin
#define MX1_STEP 33
#define MX1_SLEEP 35

#define MX2_DIR 39 // Y axis motor
#define MX2_STEP 41
#define MX2_SLEEP 43

// limit switches
#define PIN_LZ 52
#define PIN_LY 50
#define PIN_LX 48

String inputString = "";
bool stringComplete = false;

// track location of squares in mm from origin
int start_file_mm;
int start_rank_mm;
int dest_file_mm;
int dest_rank_mm;
int x_diff_mm;
int y_diff_mm;

void setup() {
    Serial.begin(9600);
    pinMode(MC_STEP,OUTPUT);
    pinMode(MC_DIR,OUTPUT);
    pinMode(MC_SLEEP,OUTPUT);

    pinMode(MZ_STEP,OUTPUT);
    pinMode(MZ_DIR,OUTPUT);
    pinMode(MZ_SLEEP,OUTPUT);

    pinMode(MY_STEP,OUTPUT);
    pinMode(MY_DIR,OUTPUT);
    pinMode(MY_SLEEP,OUTPUT);

    pinMode(MX1_STEP,OUTPUT);
    pinMode(MX1_DIR,OUTPUT);
    pinMode(MX1_SLEEP,OUTPUT);

    pinMode(MX2_STEP,OUTPUT);
    pinMode(MX2_DIR,OUTPUT);
    pinMode(MX2_SLEEP,OUTPUT);

    pinMode(PIN_LZ,INPUT_PULLUP);
    pinMode(PIN_LY,INPUT_PULLUP);
    pinMode(PIN_LX,INPUT_PULLUP);

    // start in sleep mode
    digitalWrite(MC_SLEEP,LOW);
    digitalWrite(MZ_SLEEP,LOW);
    digitalWrite(MY_SLEEP,LOW);
    digitalWrite(MX1_SLEEP,LOW);
    digitalWrite(MX2_SLEEP,LOW);

    inputString.reserve(200);
}

void loop() {
    
    if (stringComplete) {
        inputString.toUpperCase();
        inputString.trim(); // gets rid of Carriage return CR (ASCII 13)

        if (inputString.startsWith("SPIRAL ")) {
            // cmd = spiral 100 200
            int steps = inputString.substring(7,10).toInt();
            int delayms = inputString.substring(10).toInt();

            digitalWrite(MC_SLEEP,HIGH);
            cw(MC_DIR);
            mstep(MC_STEP,steps,delayms);
            delay(2000);
            ccw(MC_DIR);
            mstep(MC_STEP,steps,delayms);
            digitalWrite(MC_SLEEP,LOW);
        }

        if (inputString.startsWith("Z ")) {
            // cmd = Z 100 200
            int steps = inputString.substring(2,5).toInt();
            int delayms = inputString.substring(5).toInt();

            digitalWrite(MZ_SLEEP,HIGH);
            cw(MZ_DIR);
            mstep(MZ_STEP,steps,delayms);
            delay(3000);
            ccw(MZ_DIR);
            mstep(MZ_STEP,steps,delayms);
            digitalWrite(MZ_SLEEP,LOW);
        }

        if (inputString.startsWith("Y ")) {
            // cmd = Y 100 200
            int steps = inputString.substring(2,5).toInt();
            int delayms = inputString.substring(5).toInt();

            digitalWrite(MY_SLEEP,HIGH);
            cw(MY_DIR);
            mstep(MY_STEP,steps,delayms);
            delay(3000);
            ccw(MY_DIR);
            mstep(MY_STEP,steps,delayms);
            digitalWrite(MY_SLEEP,LOW);
        }

        if (inputString.startsWith("X ")) {
            // cmd = X 100 200
            int steps = inputString.substring(2,5).toInt();
            int delayms = inputString.substring(5).toInt();

            digitalWrite(MX1_SLEEP,HIGH);
            digitalWrite(MX2_SLEEP,HIGH);
            cw(MX1_DIR);
            cw(MX2_DIR);
            mxstep(steps,delayms);
            delay(3000);
            ccw(MX1_DIR);
            ccw(MX2_DIR);
            mxstep(steps,delayms);
            digitalWrite(MX1_SLEEP,LOW);
            digitalWrite(MX2_SLEEP,LOW);
        }

        if (inputString.startsWith("HOME")) {
            homeAll();
        }

        if (inputString.startsWith("PICK")) {
            pickUp();
        }

        if (inputString.startsWith("PLACE") ) {
            placeDown();  
        }

        if (inputString.startsWith("OFFSET") ) {
            xymove(xhome_offset,yhome_offset);
        }

        if (inputString.startsWith("MOVE ") ) {
            // cmd = MOVE -8 -8
            bool x_neg = false;
            bool y_neg = false;
            if (inputString[5] == '-') {
                x_neg = true;
            }
            if (inputString[8] == '-') {
                y_neg = true;
            }
            
            int x_mm = inputString.substring(6,7).toInt() * square_size;
            int y_mm = inputString.substring(9).toInt() * square_size;
            if (x_neg) {
                x_mm = -1 * x_mm;
            }
            if (y_neg) {
                y_mm = -1 * y_mm;
            }
            xymove(x_mm,y_mm);
        }

        if (inputString.startsWith("GO ") ) {
            // cmd = GO a8 h1
            String start = inputString.substring(3,5);
            String dest = inputString.substring(6);

            start_file_mm = 0;
            int start_rank_mm = 0;
            get_mm(start,&start_rank_mm, &start_file_mm);

            dest_file_mm = 0;
            dest_rank_mm = 0;
            get_mm(dest,&dest_rank_mm, &dest_file_mm);

            // move to first square and pick up
            xymove(start_file_mm + xhome_offset,start_rank_mm + yhome_offset);
            pickUp();

            // move to second square and place down
            x_diff_mm = dest_file_mm - start_file_mm;
            y_diff_mm = dest_rank_mm - start_rank_mm;
            xymove(x_diff_mm,y_diff_mm);
            placeDown();

            homeY();
            delay(200);
            homeX();
        }

        if (inputString.startsWith("NORMAL ") ) {
            // cmd = NORMAL a8 h1
            String start = inputString.substring(7,9);
            String dest = inputString.substring(10);
            
            start_file_mm = 0;
            start_rank_mm = 0;
            dest_file_mm = 0;
            dest_rank_mm = 0;
            get_mm(start,&start_rank_mm,&start_file_mm);
            get_mm(dest,&dest_rank_mm,&dest_file_mm);

            // Pick up piece
            xymove(start_file_mm + xhome_offset, start_rank_mm + yhome_offset);
            pickUp();

            x_diff_mm = dest_file_mm - start_file_mm;
            y_diff_mm = dest_rank_mm - start_rank_mm;

            // Place on destination square
            xymove(x_diff_mm,y_diff_mm);
            placeDown();

            homeY();
            delay(200);
            homeX();
        }

        if (inputString.startsWith("CAPTURE ") ) {
            // cmd = CAPTURE a8 h1
            String start = inputString.substring(8,10);
            String dest = inputString.substring(11);

            start_file_mm = 0;
            start_rank_mm = 0;
            dest_file_mm = 0;
            dest_rank_mm = 0;
            get_mm(start,&start_rank_mm,&start_file_mm);
            get_mm(dest,&dest_rank_mm,&dest_file_mm);

            // Go to dest first and capture that piece
            xymove(dest_file_mm + xhome_offset, dest_rank_mm + yhome_offset);
            pickUp();

            homeX();

            placeDown();

            // Go pick up and move the capturing piece (X has been homed now)
            x_diff_mm = dest_file_mm - start_file_mm;
            y_diff_mm = dest_rank_mm - start_rank_mm;
            xymove(start_file_mm + xhome_offset, -y_diff_mm);
            pickUp();

            // Place on destination square
            xymove(x_diff_mm,y_diff_mm);
            placeDown();

            homeY();
            delay(200);
            homeX();
        }

        if (inputString.startsWith("KINGSIDE ") ){
            // cmd = KINGSIDE e8 g8
            String start = inputString.substring(9,11);
            String dest = inputString.substring(12);

            start_file_mm = 0;
            start_rank_mm = 0;
            dest_file_mm = 0;
            dest_rank_mm = 0;
            get_mm(start,&start_rank_mm,&start_file_mm);
            get_mm(dest,&dest_rank_mm,&dest_file_mm);

            // Move king first
            xymove(start_file_mm + xhome_offset, start_rank_mm + yhome_offset);
            pickUp();

            x_diff_mm = dest_file_mm - start_file_mm;
            y_diff_mm = dest_rank_mm - start_rank_mm;

            // Place on destination square
            xymove(x_diff_mm,y_diff_mm);
            placeDown();

            // Pick up rook (one square to the right)
            xymove(square_size,0);
            pickUp();
            xymove(-2*square_size,0);
            placeDown();

            homeY();
            delay(200);
            homeX();
        }

        if (inputString.startsWith("QUEENSIDE ") ){
            // cmd = QUEENSIDE e8 c8
            String start = inputString.substring(10,12);
            String dest = inputString.substring(13);

            start_file_mm = 0;
            start_rank_mm = 0;
            dest_file_mm = 0;
            dest_rank_mm = 0;
            get_mm(start,&start_rank_mm,&start_file_mm);
            get_mm(dest,&dest_rank_mm,&dest_file_mm);

            // Move king first
            xymove(start_file_mm + xhome_offset, start_rank_mm + yhome_offset);
            pickUp();

            x_diff_mm = dest_file_mm - start_file_mm;
            y_diff_mm = dest_rank_mm - start_rank_mm;

            // Place on destination square
            xymove(x_diff_mm,y_diff_mm);
            placeDown();

            // Pick up rook (two squares to the left)
            xymove(-2*square_size,0);
            pickUp();
            xymove(3*square_size,0);
            placeDown();

            homeY();
            delay(200);
            homeX();
        }

        if (inputString.startsWith("PASSANT ") ){
            // cmd = PASSANT e8 c8
            String start = inputString.substring(8,10);
            String dest = inputString.substring(11);

            start_file_mm = 0;
            start_rank_mm = 0;
            dest_file_mm = 0;
            dest_rank_mm = 0;
            get_mm(start,&start_rank_mm,&start_file_mm);
            get_mm(dest,&dest_rank_mm,&dest_file_mm);

            // Move capturing pawn diagonally
            xymove(start_file_mm + xhome_offset, start_rank_mm + yhome_offset);
            pickUp();

            x_diff_mm = dest_file_mm - start_file_mm;
            y_diff_mm = dest_rank_mm - start_rank_mm;

            // Place on destination square
            xymove(x_diff_mm,y_diff_mm);
            placeDown();

            // Pick captured piece (always one rank backwards)
            xymove(0,-1*square_size);
            pickUp();

            homeX();
            delay(200);
            placeDown();
            delay(200);
            homeY();
        }
              
        inputString = "";
        stringComplete = false;
    }
    
}

// dir_pin LOW = CCW
// dir_pin HIGH = CW
void mstep(int step_pin, int steps, int speed_delay) {
    delay(3); // for waking from sleep
    for (int i = 0; i < steps; i++) {
        digitalWrite(step_pin, HIGH);
        delayMicroseconds(speed_delay);
        digitalWrite(step_pin, LOW);
        delayMicroseconds(speed_delay);
    }
}

void mxstep(int steps, int speed_delay) {
    delay(3); // for waking from sleep
    for (int i = 0; i < steps; i++) {
        digitalWrite(MX1_STEP, HIGH);
        digitalWrite(MX2_STEP,HIGH);
        delayMicroseconds(speed_delay);
        digitalWrite(MX1_STEP, LOW);
        digitalWrite(MX2_STEP,LOW);
        delayMicroseconds(speed_delay);
    }
}

// Ex: get_mm("a8")
// mm measured from square a8
void get_mm(String square, int* rank_mm, int* file_mm) {
    int fileno;
    int rank;
    if (!PLAY_AS_WHITE) {
        fileno = square[0] - 'A'; // convert to number of files away from 'a'
        rank = 8 - String(square[1]).toInt(); // convert to number of ranks away from 8
    } else {
        fileno = 'H' - square[0]; // convert to number of files away from 'h'
        rank = String(square[1]).toInt() - 1; // convert to number of ranks away from 1
    }
    *rank_mm = rank * square_size; // x distance
    *file_mm = fileno * square_size; // y distance
}

// positive mm = move out from home (a8/h1 square for black/white)
// negative mm = move towards home
void xymove(double mm_x, double mm_y) {
    int x_steps = round(mm_x/mm_step_xy);
    int y_steps = round(mm_y/mm_step_xy);

    // determine direction
    if (x_steps > 0) {
        ccw(MX1_DIR);
        ccw(MX2_DIR);
    } else {
        cw(MX1_DIR);
        cw(MX2_DIR);
    }

    if (y_steps > 0) {
        cw(MY_DIR);
    } else {
        ccw(MY_DIR);
    }
    
    x_steps = abs(x_steps);
    y_steps = abs(y_steps);

    // keep track of which has to move more
    int leftover_steps;
    int smaller_steps;
    if (x_steps > y_steps) {
        leftover_steps = x_steps;
        smaller_steps = y_steps;
    } else {
        leftover_steps = y_steps;
        smaller_steps = x_steps;
    }

    digitalWrite(MX1_SLEEP,HIGH);
    digitalWrite(MX2_SLEEP,HIGH);
    digitalWrite(MY_SLEEP,HIGH);
    delay(3);

    // step x and y; smaller steps will finish first
    for (int i = 0; i < smaller_steps; i++ ) {
        digitalWrite(MX1_STEP,HIGH);
        digitalWrite(MX2_STEP,HIGH);
        digitalWrite(MY_STEP,HIGH);
        delayMicroseconds(XY_DELAY);
        digitalWrite(MX1_STEP,LOW);
        digitalWrite(MX2_STEP,LOW);
        digitalWrite(MY_STEP,LOW);
        delayMicroseconds(XY_DELAY);
        leftover_steps--;  
    }

    // finish off the leftover steps
    if (x_steps > y_steps) {
        mxstep(leftover_steps,XY_DELAY);
    } else {
        mstep(MY_STEP,leftover_steps,XY_DELAY);
    }

    digitalWrite(MX1_SLEEP,LOW);
    digitalWrite(MX2_SLEEP,LOW);
    digitalWrite(MY_SLEEP,LOW);
}

void cw(int dir_pin) {
    digitalWrite(dir_pin,HIGH);
}

void ccw(int dir_pin) {
    digitalWrite(dir_pin,LOW);
}

void pickUp() {
    digitalWrite(MZ_SLEEP,HIGH);
    digitalWrite(MC_SLEEP,HIGH);
    delay(3);
    ccw(MZ_DIR);
    mstep(MZ_STEP,vertical_steps,Z_DELAY);
    delay(500);
    cw(MC_DIR);
    mstep(MC_STEP,CLAW_STEPS,CLAW_DELAY);
    
    cw(MZ_DIR);
    while ( digitalRead(PIN_LZ)) {
        digitalWrite(MZ_STEP, HIGH);
        delayMicroseconds(Z_DELAY);
        digitalWrite(MZ_STEP, LOW);
        delayMicroseconds(Z_DELAY);
    }
    ccw(MZ_DIR);
    mstep(MZ_STEP,REBOUND_STEPS_Z,Z_DELAY);

    digitalWrite(MZ_SLEEP,LOW);
    digitalWrite(MC_SLEEP,LOW);
}

void placeDown() {
    digitalWrite(MZ_SLEEP,HIGH);
    digitalWrite(MC_SLEEP,HIGH);
    delay(3);
    ccw(MZ_DIR);
    mstep(MZ_STEP,vertical_steps,Z_DELAY);
    delay(500);
    ccw(MC_DIR);
    mstep(MC_STEP,CLAW_STEPS,CLAW_DELAY);
    
    cw(MZ_DIR);
    while ( digitalRead(PIN_LZ)) {
        digitalWrite(MZ_STEP, HIGH);
        delayMicroseconds(Z_DELAY);
        digitalWrite(MZ_STEP, LOW);
        delayMicroseconds(Z_DELAY);
    }
    ccw(MZ_DIR);
    mstep(MZ_STEP,REBOUND_STEPS_Z,Z_DELAY);

    cw(MC_DIR);
    mstep(MC_STEP,REBOUND_STEPS_CLAW,CLAW_DELAY);

    digitalWrite(MZ_SLEEP,LOW);
    digitalWrite(MC_SLEEP,LOW);
}

void homeAll() {
    homeZ();
    delay(200);
    homeY();
    delay(200);
    homeX();
}

void homeX() {
     // X
    digitalWrite(MX1_SLEEP,HIGH);
    digitalWrite(MX2_SLEEP,HIGH);
    delay(3);
    cw(MX1_DIR);
    cw(MX2_DIR);
    while ( digitalRead(PIN_LX) ) {
        digitalWrite(MX1_STEP, HIGH);
        digitalWrite(MX2_STEP, HIGH);
        delayMicroseconds(XY_DELAY);
        digitalWrite(MX1_STEP, LOW);
        digitalWrite(MX2_STEP, LOW);
        delayMicroseconds(XY_DELAY);
    }
    ccw(MX1_DIR);
    ccw(MX2_DIR);
    mxstep(REBOUND_STEPS_XY,XY_DELAY);
    digitalWrite(MX1_SLEEP,LOW);
    digitalWrite(MX2_SLEEP,LOW);
}

void homeY() {
    // Y
    digitalWrite(MY_SLEEP,HIGH);
    delay(3);
    ccw(MY_DIR);
    while ( digitalRead(PIN_LY) ) {
        digitalWrite(MY_STEP, HIGH);
        delayMicroseconds(XY_DELAY);
        digitalWrite(MY_STEP, LOW);
        delayMicroseconds(XY_DELAY);
    }
    cw(MY_DIR);
    mstep(MY_STEP,REBOUND_STEPS_XY ,XY_DELAY);
    digitalWrite(MY_SLEEP,LOW);
}

void homeZ() {
    // Z
    digitalWrite(MZ_SLEEP,HIGH);
    delay(3);
    cw(MZ_DIR);
    while ( digitalRead(PIN_LZ) ) {
        digitalWrite(MZ_STEP, HIGH);
        delayMicroseconds(Z_DELAY);
        digitalWrite(MZ_STEP, LOW);
        delayMicroseconds(Z_DELAY);
    }
    ccw(MZ_DIR);
    mstep(MZ_STEP,REBOUND_STEPS_Z,Z_DELAY);
    digitalWrite(MZ_SLEEP,LOW);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char) Serial.read();
    if (inChar == '\n') {   // line feed (ASCII 10)
      stringComplete = true;
    } else {
      inputString += inChar;
    }  
  }  
}

//void countSteps() {
//	delay(3);
//	int total_steps = 0;
//	digitalWrite(M2_DIR,LOW); // CCW
//	while ( digitalRead(PIN_LZ) ) {
//		digitalWrite(M2_STEP,HIGH);
//		delay(1);
//		digitalWrite(M2_STEP,LOW);
//		delay(1);
//		total_steps++;
//	}
//	Serial.println(total_steps);
//}
