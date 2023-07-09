#include
&lt;SoftwareSerial.h&gt;

#include &lt;FastLED.h&gt;
#define NUM_LEDS 30
#define DATA_PIN 6
CRGB LEDs[NUM_LEDS];
CRGB colorPlayer1 = CRGB::Blue;
CRGB colorPlayer2 = CRGB::Red;

SoftwareSerial SSerial(8, 9);

volatile int rowCount = 5;
volatile int colCount = 6;
volatile int currentPlayer = 1;
volatile int grid[6][5];
volatile int winningPositions[6][5];
volatile int dropCount;
volatile int messageDisplayed = 0;
volatile int gamewon = 0;
volatile int colorPattern = 1;
int drop(int column, int player);
int fourConnected(int player);
void reset();
void showValues();
void showLeds();
void changeColors();
void setup() {

randomSeed(analogRead(0));

Serial.begin(9600);
while (!Serial) {}
SSerial.begin(9600);
while (!SSerial) {}
Serial.println(&quot;SSerial started at 9600&quot;);
Serial.println(&quot; &quot;);
FastLED.delay(3000);
FastLED.addLeds&lt;WS2812B, DATA_PIN, GRB&gt;(LEDs, NUM_LEDS);
}
void loop() {

reset();

while (gamewon == 0) {

if (SSerial.available() &gt; 0) {
int selectedColumn = (SSerial.read() - 48) -

1;

play(selectedColumn);
}
else if (Serial.available() &gt; 0) {
int selectedColumn = (Serial.read() - 48) -

1;

play(selectedColumn);

}
else {
if (messageDisplayed == 0) {
Serial.print(&quot;Player &quot;);
Serial.print(currentPlayer);
Serial.print(&quot;, please select a column

between 1 and &quot;);

Serial.println(colCount);
messageDisplayed = 1;
}
}
}
}
void play(int selectedColumn) {
if (selectedColumn == 33 || selectedColumn == 65) {
// r or R (restart)
Serial.println(&quot;Game will restart&quot;);
reset();
}
else if (selectedColumn == 99-49 || selectedColumn == 67-
49) { // c or C to change colors

Serial.println(&quot;Colors changed.&quot;);
changeColors();
showLeds();
}
else if (selectedColumn &lt; 0 || selectedColumn &gt; colCount -
1) {

Serial.print(&quot;Invalid column, select between 1 and

&quot;);

Serial.println(colCount);
}
else if (selectedColumn &gt;= 0 &amp;&amp; selectedColumn &lt;= colCount)
{

int droppedOnRow = drop(selectedColumn,

currentPlayer);

if (droppedOnRow &gt;= 0) {
showValues();
showLeds();
dropCount++;
gamewon = fourConnected(currentPlayer);

if (gamewon == 0) {
currentPlayer = (currentPlayer == 1) ?

2 : 1;

Serial.print(&quot;Player &quot;);
Serial.print(currentPlayer, DEC);
Serial.print(&quot; selected column &quot;);
Serial.print(selectedColumn + 1, DEC);
Serial.print(&quot; and is dropped on row

&quot;);

Serial.print(droppedOnRow + 1, DEC);

Serial.print(&quot;. Total dropcount: &quot;);
Serial.println(dropCount);
if (dropCount == (rowCount *

colCount)) {

Serial.println(&quot;Game ended,

total reached, no winner.&quot;);

reset();
}
}
else if (gamewon &gt; 0) {
Serial.print(&quot;Game is won by player

&quot;);

Serial.println(currentPlayer);
showWinningPattern(currentPlayer);
reset();
}
messageDisplayed = 0;
}
else if (droppedOnRow == -1) {
Serial.println(&quot;Column is full, retry!&quot;);
}
}
}
int drop(int column, int player) {

for (int row = 0; row &lt; rowCount; row++)
{
if (grid[column][row] == 0) {
int offset = colCount * row;
LEDs[column + offset] = player == 1 ?

colorPlayer1 : colorPlayer2;
FastLED.show();
delay(200);
LEDs[column + offset] = CRGB::Black;
FastLED.show();
delay(200);
}
if (grid[column][row] == 0) {
grid[column][row] = player;
return row;
}
else if (row == (rowCount - 1)) {
return -1;
}
}
}
int fourConnected(int player) {

// Horizontal Check
for (int j = 0; j &lt; rowCount - 3; j++) {
for (int i = 0; i &lt; colCount; i++) {
if (grid[i][j] == player &amp;&amp; grid[i][j + 1] ==
player &amp;&amp; grid[i][j + 2] == player &amp;&amp; grid[i][j + 3] == player) {
winningPositions[i][j] = player;
winningPositions[i][j + 1] = player;
winningPositions[i][j + 2] = player;
winningPositions[i][j + 3] = player;
return 1;
}
}
}
// Vertical Check
for (int i = 0; i &lt; colCount - 3; i++) {
for (int j = 0; j &lt; rowCount; j++) {
if (grid[i][j] == player &amp;&amp; grid[i + 1][j] ==
player &amp;&amp; grid[i + 2][j] == player &amp;&amp; grid[i + 3][j] == player) {
winningPositions[i][j] = player;
winningPositions[i + 1][j] = player;
winningPositions[i + 2][j] = player;
winningPositions[i + 3][j] = player;
return 2;
}
}
}
// Ascending DiagonalCheck
for (int i = 3; i &lt; colCount; i++) {
for (int j = 0; j &lt; rowCount - 3; j++) {
if (grid[i][j] == player &amp;&amp; grid[i - 1][j +
1] == player &amp;&amp; grid[i - 2][j + 2] == player &amp;&amp; grid[i - 3][j + 3]
== player) {

winningPositions[i][j] = player;
winningPositions[i - 1][j + 1] =

player;

winningPositions[i - 2][j + 2] =

player;

winningPositions[i - 3][j + 3] =

player;

return 3;
}
}
}
// Descendin gDiagonalCheck
for (int i = 3; i &lt; colCount; i++) {
for (int j = 3; j &lt; rowCount; j++) {
if (grid[i][j] == player &amp;&amp; grid[i - 1][j -
1] == player &amp;&amp; grid[i - 2][j - 2] == player &amp;&amp; grid[i - 3][j - 3]
== player) {

winningPositions[i][j] = player;
winningPositions[i - 1][j - 1] =

player;

winningPositions[i - 2][j - 2] =

player;

winningPositions[i - 3][j - 3] =

player;

return 4;
}
}
}
return 0;
}
void reset() {
for (int row = 0; row &lt; rowCount; row++)
{
for (int column = 0; column &lt; colCount; column++)
{
grid[column][row] = 0;
winningPositions[column][row] = 0;
}
}

for (int led = 0; led &lt; NUM_LEDS; led++) {
LEDs[led] = CRGB::Black;
}
FastLED.show();
dropCount = 0;
currentPlayer = 1; // (int)random(1, 3);
gamewon = 0;
messageDisplayed = 0;
}

void showValues() {
for (int row = 0; row &lt; rowCount; row++)
{
Serial.print(grid[0][row]);
Serial.print(&quot;,&quot;);
Serial.print(grid[1][row]);
Serial.print(&quot;,&quot;);
Serial.print(grid[2][row]);
Serial.print(&quot;,&quot;);
Serial.print(grid[3][row]);
Serial.print(&quot;,&quot;);
Serial.print(grid[4][row]);
Serial.print(&quot;,&quot;);
Serial.print(grid[5][row]);
Serial.println();
}
}
void showLeds() {

for (int row = 0; row &lt; rowCount; row++)
{
int offset = colCount * row;

for (int col = 0; col &lt; colCount; col++) {
if (grid[col][row] == 1 &amp;&amp; grid[col][row] !=

2) {

LEDs[col + offset] = colorPlayer1;
}
else if (grid[col][row] == 2 &amp;&amp;

grid[col][row] != 1) {

LEDs[col + offset] = colorPlayer2;
}
else {
LEDs[col + offset] = CRGB::Black;
}
}
}
FastLED.show();
}

void showWinningPattern(int player) {

CRGB color = (player == 1) ? colorPlayer1 : colorPlayer2;
int black = 0;
// Flash the winning pattern
for (int i = 0; i &lt; 15; i++) {
for (int row = 0; row &lt; rowCount; row++)
{
int offset = colCount * row;
for (int col = 0; col &lt; colCount; col++) {
if (winningPositions[col][row] ==

player) {

LEDs[col + offset] = (black ==

0) ? CRGB::Black : color;
}
}
}
FastLED.show();
delay(200);
black = (black == 0) ? 1 : 0;
}
// Flash all
for (int i = 0; i &lt; 4; i++) {
allToColor(color);
delay(200);
allToColor(CRGB::Black);
delay(200);
}
}
void allToColor(CRGB color) {

for (int i = 0; i &lt; NUM_LEDS; i++)
{
LEDs[i] = color;
FastLED.show();
delay(10);
}
FastLED.show();
}
void changeColors() {

colorPattern++;

if (colorPattern == 1)
{
colorPlayer1 = CRGB::Blue;
colorPlayer2 = CRGB::Red;
}
else if (colorPattern == 2) {
colorPlayer1 = CRGB::Yellow;
colorPlayer2 = CRGB::Purple;
}
else if (colorPattern == 3) {
colorPlayer1 = CRGB::Green;
colorPlayer2 = CRGB::Orange;
}
else if (colorPattern == 4) {
colorPlayer1 = CRGB::Violet;
colorPlayer2 = CRGB::LightGreen;
}
else if (colorPattern == 5) {
colorPlayer1 = CRGB::Wheat;
colorPlayer2 = CRGB::DarkBlue;
}
else {
colorPattern = 1;
colorPlayer1 = CRGB::Blue;
colorPlayer2 = CRGB::Red;
}
}
