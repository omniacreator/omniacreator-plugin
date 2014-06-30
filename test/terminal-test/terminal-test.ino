#include "InterfaceLibrary.h"

ILCore core;
ILTerminal terminal0;
ILTerminal terminal1;

void setup()
{
  core.init();
  core.clearAll();

  core.println("========= Starting Test =========");

  // Print Version
  {
    char versionBuffer[16];
    core.getVersion(versionBuffer,
    sizeof(versionBuffer));

    core.print("\nVersion: ");
    core.println(versionBuffer);
    delay(1000);
  }

  // Print Compat Version
  {
    char compatibleVersionBuffer[16];
    core.getCompatVersion(compatibleVersionBuffer,
    sizeof(compatibleVersionBuffer));

    core.print("Compatible Version: ");
    core.println(compatibleVersionBuffer);
    delay(1000);
  }

  core.println("\n-------- Testing Numbers --------");

  core.print("\n-2147483648 DEC: ");
  core.println(-2147483648LL, DEC);
  core.print("-2147483648 BIN: ");
  core.println(-2147483648LL, BIN);
  core.print("-2147483648 HEX: ");
  core.println(-2147483648LL, HEX);
  core.print("-2147483648 OCT: ");
  core.println(-2147483648LL, OCT);
  delay(1000);

  core.print("\n-2147483647 DEC: ");
  core.println(-2147483647, DEC);
  core.print("-2147483647 BIN: ");
  core.println(-2147483647, BIN);
  core.print("-2147483647 HEX: ");
  core.println(-2147483647, HEX);
  core.print("-2147483647 OCT: ");
  core.println(-2147483647, OCT);
  delay(1000);

  core.print("\n-2 DEC: ");
  core.println(-2, DEC);
  core.print("-2 BIN: ");
  core.println(-2, BIN);
  core.print("-2 HEX: ");
  core.println(-2, HEX);
  core.print("-2 OCT: ");
  core.println(-2, OCT);
  delay(1000);

  core.print("\n-1 DEC: ");
  core.println(-1, DEC);
  core.print("-1 BIN: ");
  core.println(-1, BIN);
  core.print("-1 HEX: ");
  core.println(-1, HEX);
  core.print("-1 OCT: ");
  core.println(-1, OCT);
  delay(1000);

  core.print("\n0 DEC: ");
  core.println(0, DEC);
  core.print("0 BIN: ");
  core.println(0, BIN);
  core.print("0 HEX: ");
  core.println(0, HEX);
  core.print("0 OCT: ");
  core.println(0, OCT);
  delay(1000);

  core.print("\n1 DEC: ");
  core.println(1, DEC);
  core.print("1 BIN: ");
  core.println(1, BIN);
  core.print("1 HEX: ");
  core.println(1, HEX);
  core.print("1 OCT: ");
  core.println(1, OCT);
  delay(1000);

  core.print("\n2147483646 DEC: ");
  core.println(2147483646, DEC);
  core.print("2147483646 BIN: ");
  core.println(2147483646, BIN);
  core.print("2147483646 HEX: ");
  core.println(2147483646, HEX);
  core.print("2147483646 OCT: ");
  core.println(2147483646, OCT);
  delay(1000);

  core.print("\n2147483647 DEC: ");
  core.println(2147483647, DEC);
  core.print("2147483647 BIN: ");
  core.println(2147483647, BIN);
  core.print("2147483647 HEX: ");
  core.println(2147483647, HEX);
  core.print("2147483647 OCT: ");
  core.println(2147483647, OCT);
  delay(1000);

  core.println("\n--------- Testing Color ---------");

  core.write(CC_SET_FOREGROUND_TO_GRAY);
  core.println("\ngray");
  core.write(CC_SET_FOREGROUND_TO_BLUE);
  core.println("blue");
  core.write(CC_SET_FOREGROUND_TO_RED);
  core.println("red");
  core.write(CC_SET_FOREGROUND_TO_GREEN);
  core.println("green");
  core.write(CC_SET_FOREGROUND_TO_MAGENTA);
  core.println("magenta");
  core.write(CC_SET_FOREGROUND_TO_CYAN);
  core.println("cyan");
  core.write(CC_SET_FOREGROUND_TO_YELLOW);
  core.println("yellow");
  core.write(CC_SET_FOREGROUND_TO_BLACK);
  core.println("black");

  core.println("\n-------- Testing Control --------");

  core.write(CC_BEEP_SPEAKER);
  core.println("\nBeep Speaker");
  delay(1000);

  core.write(CC_BEEP_SPEAKER);
  core.println("Beep Speaker");
  delay(1000);

  core.println("\n------- Testing Terminals -------");

  terminal0.init(&core, "Terminal 0");
  terminal0.println("Text for Terminal 0");
  delay(1000);

  terminal1.init(&core, "Terminal 1");
  terminal1.println("Text for Terminal 1");
  delay(1000);

  core.println("\n========== Ending Test ==========");
}

void loop()
{
  char buffer[64 + 1] = {}; int bPointer = 0;

  core.print("\n> ");

  do
  {
    while(!core.available()) { core.idleTask(); }
  }
  while(((buffer[bPointer++] = core.read()) != '\n')
  && (bPointer != (sizeof(buffer) - 1)));

  core.print("< ");
  core.print(buffer);
}
