/*
==============================================================================================================================================================================================
***tetris neo by SD132*** NO ANY CODE CHANGEING!
==============================================================================================================================================================================================
***DEVICE*** M5STACK+M5GO BASE
***NOTE***
*MUST HAVE M5GO
**LED BAR IS RUNNING
***FLASH SIZE 16M
****NO STICK SERIES
*****INSTALL LIBRARY:"M5Stack" "Adafruit NeoPixel"
***THANKS***
==============================================================================================================================================================================================        */

#include <M5Stack.h>
#include <Adafruit_NeoPixel.h>


#define M5STACK_FIRE_NEO_NUM_LEDS 10
#define M5STACK_FIRE_NEO_DATA_PIN 15

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(M5STACK_FIRE_NEO_NUM_LEDS, M5STACK_FIRE_NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);

#define NOTE_D0 -1
#define NOTE_D1 294
#define NOTE_D2 330
#define NOTE_D3 350
#define NOTE_D4 393
#define NOTE_D5 441
#define NOTE_D6 495
#define NOTE_D7 556

#define NOTE_DL1 147
#define NOTE_DL2 165
#define NOTE_DL3 175
#define NOTE_DL4 196
#define NOTE_DL5 221
#define NOTE_DL6 248
#define NOTE_DL7 278

#define NOTE_DH1 589
#define NOTE_DH2 661
#define NOTE_DH3 700
#define NOTE_DH4 786
#define NOTE_DH5 882
#define NOTE_DH6 990
#define NOTE_DH7 112

uint16_t BlockImage[8][12][12];
uint16_t backBuffer[240][120];
const int Length = 12;     
const int Width  = 10;     
const int Height = 20;     
int screen[Width][Height] = {0}; 
struct Point {int X, Y;};
struct Block {Point square[4][4]; int numRotate, color;};
Point pos; Block block;
int rot, fall_cnt = 0;
bool started = false, gameover = false;
boolean but_A = false, but_LEFT = false, but_RIGHT = false;
int game_speed = 25; // 25msec
Block blocks[7] = {
  {{{{-1,0},{0,0},{1,0},{2,0}},{{0,-1},{0,0},{0,1},{0,2}},
  {{0,0},{0,0},{0,0},{0,0}},{{0,0},{0,0},{0,0},{0,0}}},2,1},
  {{{{0,-1},{1,-1},{0,0},{1,0}},{{0,0},{0,0},{0,0},{0,0}},
  {{0,0},{0,0},{0,0},{0,0}},{{0,0},{0,0},{0,0},{0,0}}},1,2},
  {{{{-1,-1},{-1,0},{0,0},{1,0}},{{-1,1},{0,1},{0,0},{0,-1}},
  {{-1,0},{0,0},{1,0},{1,1}},{{1,-1},{0,-1},{0,0},{0,1}}},4,3},
  {{{{-1,0},{0,0},{0,1},{1,1}},{{0,-1},{0,0},{-1,0},{-1,1}},
  {{0,0},{0,0},{0,0},{0,0}},{{0,0},{0,0},{0,0},{0,0}}},2,4},
  {{{{-1,0},{0,0},{1,0},{1,-1}},{{-1,-1},{0,-1},{0,0},{0,1}},
  {{-1,1},{-1,0},{0,0},{1,0}},{{0,-1},{0,0},{0,1},{1,1}}},4,5},
  {{{{-1,1},{0,1},{0,0},{1,0}},{{0,-1},{0,0},{1,0},{1,1}},
  {{0,0},{0,0},{0,0},{0,0}},{{0,0},{0,0},{0,0},{0,0}}},2,6},
  {{{{-1,0},{0,0},{1,0},{0,-1}},{{0,-1},{0,0},{0,1},{-1,0}},
  {{-1,0},{0,0},{1,0},{0,1}},{{0,-1},{0,0},{0,1},{1,0}}},4,7}
};
extern uint8_t tetris_img[];

void setup(void) {
  Serial.begin(115200);
  M5.begin();                   // M5STACK INITIALIZE
  M5.Lcd.setBrightness(200);    // BRIGHTNESS = MAX 255
  M5.Lcd.fillScreen(BLACK);     // CLEAR SCREEN
  //M5.Lcd.invertDisplay(true);
  pixels.begin(); 
 Serial.println("console");
 Serial.println("[  ok  ]mounted boot/1041/t00.bin");
 Serial.println("<t00.bin>");
 Serial.println("checking main ver...");
 Serial.println("newest. bootver 0.6 mainver 7.1");
 M5.Speaker.tone(NOTE_D2, 200);
  make_block( 0, BLACK);        // Type No, Color
  make_block( 1, 0x00F0);       // DDDD     RED
  make_block( 2, 0xFBE4);       // DD,DD    PUPLE 
  make_block( 3, 0xFF00);       // D__,DDD  BLUE
  make_block( 4, 0xFF87);       // DD_,_DD  GREEN 
  make_block( 5, 0x87FF);       // __D,DDD  YELLO
  make_block( 6, 0xF00F);       // _DD,DD_  LIGHT GREEN
  make_block( 7, 0xF8FC);       // _D_,DDD  PINK
  
  // M5.Lcd.drawJpgFile(SD, "/tetris.jpg");     // Load background from SD
  M5.Lcd.drawJpg(tetris_img, 34215); // Load background from file data
  Serial.println("loading...");
  PutStartPos(); 
  // Start Position
  for (int i = 0; i < 4; ++i) screen[pos.X + 
   block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
  Serial.println("loaded.");
  Draw();                                    // Draw block
}

void loop() {
  if (gameover) return;
  Point next_pos;
  int next_rot = rot;
  GetNextPosRot(&next_pos, &next_rot);
  ReviseScreen(next_pos, next_rot);
  M5.update();
  delay(game_speed);  
  pixel();
}

void Draw() {                               // Draw 120x240 in the center
  for (int i = 0; i < Width; ++i) for (int j = 0; j < Height; ++j)
   for (int k = 0; k < Length; ++k) for (int l = 0; l < Length; ++l)
    backBuffer[j * Length + l][i * Length + k] = BlockImage[screen[i][j]][k][l];
    M5.Lcd.drawBitmap(100, 0, 120, 240, (uint8_t*)backBuffer);
}

void PutStartPos() {
  pos.X = 4; pos.Y = 1;
  block = blocks[random(7)];
  rot = random(block.numRotate);
  Serial.println("setup");
}

bool GetSquares(Block block, Point pos, int rot, Point* squares) {
  bool overlap = false;
  for (int i = 0; i < 4; ++i) {
    Point p;
    p.X = pos.X + block.square[rot][i].X;
    p.Y = pos.Y + block.square[rot][i].Y;
    overlap |= p.X < 0 || p.X >= Width || p.Y < 0 || p.Y >= 
      Height || screen[p.X][p.Y] != 0;
    squares[i] = p;
  }
  return !overlap;
}

void GameOver() {
  for (int i = 0; i < Width; ++i) for (int j = 0; j < Height; ++j)
  if (screen[i][j] != 0) screen[i][j] = 4;
  Serial.println("over");
  M5.Lcd.println("GAME OVER");
  M5.Speaker.tone(NOTE_D1, 200);
  gameover = true;
}

void ClearKeys() { but_A=false; but_LEFT=false; but_RIGHT=false;}

bool KeyPadLoop(){
  if(M5.BtnA.wasPressed()){ClearKeys();but_LEFT =true;return true;}
  if(M5.BtnB.wasPressed()){ClearKeys();but_RIGHT=true;return true;}
  if(M5.BtnC.wasPressed()){ClearKeys();but_A    =true;return true;}
  return false;
}

void GetNextPosRot(Point* pnext_pos, int* pnext_rot) {
  bool received = KeyPadLoop();
  if (but_A) started = true;
  if (!started) return;
  pnext_pos->X = pos.X;
  pnext_pos->Y = pos.Y;
  if ((fall_cnt = (fall_cnt + 1) % 10) == 0) pnext_pos->Y += 1;
  else if (received) {
    if (but_LEFT) { but_LEFT = false; pnext_pos->X -= 1;}
    else if (but_RIGHT) { but_RIGHT = false; pnext_pos->X += 1;}
    else if (but_A) { but_A = false;
      *pnext_rot = (*pnext_rot + block.numRotate - 1)%block.numRotate; 
    }
  }
}

void DeleteLine() {
  for (int j = 0; j < Height; ++j) {
    bool Delete = true;
    Serial.println("line detected,erase them");
    for (int i = 0; i < Width; ++i) if (screen[i][j] == 0) Delete = false;
    if (Delete) for (int k = j; k >= 1; --k) 
    for (int i = 0; i < Width; ++i) screen[i][k] = screen[i][k - 1];
  }
}

void ReviseScreen(Point next_pos, int next_rot) {
  if (!started) return;
  Point next_squares[4];
  for (int i = 0; i < 4; ++i) screen[pos.X + 
    block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = 0;
  if (GetSquares(block, next_pos, next_rot, next_squares)) {
   for (int i = 0; i < 4; ++i){
     screen[next_squares[i].X][next_squares[i].Y] = block.color;
   }
   pos = next_pos; rot = next_rot;
  }
  else {
   for (int i = 0; i < 4; ++i) screen[pos.X + 
    block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
   if (next_pos.Y == pos.Y + 1) {
    DeleteLine(); PutStartPos();
    if (!GetSquares(block, pos, rot, next_squares)) {
     for (int i = 0; i < 4; ++i) screen[pos.X + 
      block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
      GameOver();
    }
   }
  }
  Draw();
}
void make_block( int n , uint16_t color ){            // Make Block color       
  for ( int i =0 ; i < 12; i++ ) for ( int j =0 ; j < 12; j++ ){
    BlockImage[n][i][j] = color;                           // Block color
    if ( i == 0 || j == 0 ) BlockImage[n][i][j] = 0;       // BLACK Line
  } 
}


void pixel(){
  static int pixelNumber=0;// = random(0, M5STACK_FIRE_NEO_NUM_LEDS - 1);
  pixelNumber++;
  if(pixelNumber>9)pixelNumber=0;
  int r = 1<<random(0, 7);
  int g = 1<<random(0, 7);
  int b = 1<<random(0, 7);

  pixels.setPixelColor(pixelNumber, pixels.Color(r, g, b));     
  pixels.show();

  Serial.println("pixel load 0x01");

  delay(100);
  }
