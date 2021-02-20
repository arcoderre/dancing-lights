#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define PIN           6
#define MATRIX_WIDTH  5
#define MATRIX_LENGTH 8

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MATRIX_WIDTH, MATRIX_LENGTH, PIN,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

char dancing_lights[16] = "Dancing Lights!";

void setup() {
  matrix.begin();
  matrix.fillScreen(0);
  matrix.show();
  
  Serial.begin(115200);
  writeText(dancing_lights, 90);
}

void loop() {
  rain();  
  rectanglesIn(matrix.Color(0, 45, 0));
  triangles(matrix.Color(0, 0, 50));  
  wave();  
  squares();
  spinner(matrix.Color(40, 0, 0));  
  rectanglesOut(matrix.Color(55, 55, 0));
  diamonds(matrix.Color(0, 25, 35));
}

////////////////////////////////////////////
// Pattern functions:                     //
////////////////////////////////////////////

// Raindrops ---------------------
uint16_t drop_high = matrix.Color(20, 20, 50);
uint16_t drop_med = matrix.Color(15, 15, 40);
uint16_t drop_low = matrix.Color(8, 8, 25);
uint16_t drop_off = matrix.Color(5, 5, 10);
u8 drop_count = 10;
int duration = 10000;
class Drop {
  public:
  long column;
  long current_row;

  void initialize() {
    column = random(MATRIX_WIDTH);
    current_row = random(-drop_count, 0);
  }

  void draw() {
    matrix.drawPixel(column, current_row, drop_high);
    matrix.drawPixel(column, current_row - 1, drop_med);
    matrix.drawPixel(column, current_row - 2, drop_low);
    matrix.show();
  }
  
  void step() {
    current_row++;
    
    // When a drop goes off-screen, re-initialize with new inputs
    if (current_row > MATRIX_LENGTH + 2) {
      initialize();
    }
  }
};

void rain() {
  Drop drops[drop_count];
  for (int num = 0; num < drop_count; num++) {
    drops[num] = Drop();
    drops[num].initialize();
  }
  long start_time = millis();
  while (millis() - start_time < duration) {
    matrix.fillScreen(drop_off);
    for (int num = 0; num < drop_count; num++) {
      drops[num].step();
      drops[num].draw();
    }
    delay(50);
  }
}

// Squares ------------------
// These colours are obnoxious, but chosen by the "client" :)
uint16_t colours[] = {
  matrix.Color(50, 20, 20),
  matrix.Color(20, 20, 20),
  matrix.Color(50, 50, 10),
  matrix.Color(50, 10, 10)
  };
int numColours = 4;
void squares() {
  for (int i = 0; i < numColours * 5; i++) {
    matrix.fillScreen(0);
    matrix.drawRect(0, 0, MATRIX_WIDTH, MATRIX_LENGTH, colours[i % (numColours)]);
    matrix.drawRect(1, 1, MATRIX_WIDTH - 2, MATRIX_LENGTH - 2, colours[(i + 1) % (numColours)]);
    matrix.drawLine(2, 2, MATRIX_WIDTH - 3, MATRIX_LENGTH - 3, colours[(i + 2) % (numColours)]);
    matrix.show();
    delay(500);
  }
}

// Waves -----------------
// A wave with a tracer bouncing up and down
uint16_t wave_light = matrix.Color(50, 50, 50);
uint16_t wave_med = matrix.Color(40, 40, 40);
uint16_t wave_low = matrix.Color(30, 30, 30);
uint16_t wave_off = matrix.Color(20, 20, 20);
void wave() {
  for (int pass = 0; pass < 10; pass++) {
    // Wave travels down:
    for (int offset = 0; offset < MATRIX_LENGTH - 1; offset++) {
      matrix.fillScreen(wave_off);
      matrix.drawLine(0, offset, MATRIX_WIDTH, offset, wave_light);
      matrix.drawLine(0, offset - 1, MATRIX_WIDTH, offset - 1, wave_med);
      matrix.drawLine(0, offset - 2, MATRIX_WIDTH, offset - 2, wave_low);
      matrix.show();
      delay(100);
    }
    // And then back up:
    for (int offset = MATRIX_LENGTH - 1; offset > 0; offset--) {
      matrix.fillScreen(wave_off);
      matrix.drawLine(0, offset, MATRIX_WIDTH, offset, wave_light);
      matrix.drawLine(0, offset + 1, MATRIX_WIDTH, offset + 1, wave_med);
      matrix.drawLine(0, offset + 2, MATRIX_WIDTH, offset + 2, wave_low);
      matrix.show();
      delay(100);
    }
  }
 
}

// Falling triangles, alternating left and right side
void triangles(uint16_t colour) {
  for (int pass = 0; pass < 8; pass++) {
    // Start off-screen and fall through. Origin is the top of the triangle
    for (int i = -MATRIX_WIDTH; i < MATRIX_LENGTH; i++) {
      matrix.fillScreen(0);
      matrix.drawLine(0, i, MATRIX_WIDTH - 1, i, colour);
      matrix.drawLine(0, i, MATRIX_WIDTH - 1, MATRIX_WIDTH - 1 + i, colour);
      matrix.drawLine(MATRIX_WIDTH - 1, i, MATRIX_WIDTH - 1, MATRIX_WIDTH - 1 + i, colour);
      matrix.show();
      delay(50);
    }

    for (int i = -MATRIX_WIDTH; i < MATRIX_LENGTH; i++) {
      matrix.fillScreen(0);
      matrix.drawLine(0, i, MATRIX_WIDTH - 1, i, colour);
      matrix.drawLine(0, i, 0, MATRIX_WIDTH - 1 + i, colour);
      matrix.drawLine(0, MATRIX_WIDTH - 1 + i, MATRIX_WIDTH - 1, i, colour);
      matrix.show();
      delay(50);
    }    
  }
}

// Spinning line, not great visuals unfortunately
void spinner(uint16_t colour) {
  for (int turns = 0; turns < 5; turns++) {
    for (int i = 0; i < MATRIX_WIDTH; i++) {
      matrix.fillScreen(0);
      matrix.drawLine(i, 0, MATRIX_WIDTH - 1 - i, MATRIX_LENGTH - 1, colour);
      matrix.show();
      delay(100);
    }
    for (int i = 1; i < MATRIX_LENGTH - 1; i++) {
      matrix.fillScreen(0);
      matrix.drawLine(0, MATRIX_LENGTH - 1 - i, MATRIX_WIDTH - 1, i, colour);
      matrix.show();
      delay(100);
    }
  }
}

// Sequences of nested rectanges (5x8, 3x6, 1x4)
//
// Note: these two fns could probably be combined with some clever indexing,
//       although it would likely hurt readability. 
void rectanglesIn(uint16_t colour) {
  for (int pass = 0; pass < 5; pass++) {
    for (int offset = 0; offset < 3; offset++) {
      matrix.fillScreen(0);
      matrix.drawRect(offset, offset, MATRIX_WIDTH - (2 * offset), MATRIX_LENGTH - (2 * offset), colour);
      matrix.show();
      delay(200);
    }
  }
}

void rectanglesOut(uint16_t colour) {
  for (int pass = 0; pass < 5; pass++) {
    for (int offset = 2; offset >= 0; offset--) {
      matrix.fillScreen(0);
      matrix.drawRect(offset, offset, MATRIX_WIDTH - (2 * offset), MATRIX_LENGTH - (2 * offset), colour);
      matrix.show();
      delay(200);
    }
  }
}

// Scrolling text. Used at startup
void writeText(char *text, int length) {
  int cursorLocation = matrix.width();
  matrix.setTextWrap(false);
  matrix.setTextColor(matrix.Color(25, 0, 25));
  while (--cursorLocation > -length) {
    matrix.fillScreen(0);
    matrix.setCursor(cursorLocation, 0);
    matrix.print(text);
    matrix.show();
    delay(60);
  }
}

// "Blue diamonds coming in from the side!"... with randomized size and direction
void diamonds(uint16_t colour) {
  for (int pass = 0; pass < 15; pass++) {
    int row = random(2, MATRIX_LENGTH - 2);
    int len = random(1, 5);
    int dir = random(0, 2) * 2 - 1;   // 1 or -1

    // The extra switching to make both directions work was probably not worth the penalty in readability
    
    // For symmetry, we define the starting point as (middle +/- offset), where the offset is middle (2)
    // plus or minus (middle plus the "radius"). We go from -len to 5 (board width) + len, and vice versa.
    for (int col = 2 + -dir * (len + 2); dir * col < dir * (2 + (len + 3) * dir); col = col + dir) {
      matrix.fillScreen(0);
      
      // Draw the middle row, then the progressively shorter offset rows:
      matrix.drawLine(col - len, row, col + len, row, colour);
      for (int offset = 1; offset <= len; offset++) {
        matrix.drawLine(col - len + offset, row - offset, col + len - offset, row - offset, colour); 
        matrix.drawLine(col - len + offset, row + offset, col + len - offset, row + offset, colour);
      }
      matrix.show();
      delay(100);
    }
  }
}
